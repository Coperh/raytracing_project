#include "precomp.h"
#include <Windows.h>

using namespace std::chrono;

TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 10, 0), V(0, 0, 1);
static float d = 1;


//float3 frame[SCRHEIGHT][SCRWIDTH];

const int aa_res = 1;
const int num_subpix = aa_res * aa_res;
const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;


const  int TOTAL_RAYS = AA_Height * AA_Width;
//Ray AntiAliasRays[AA_Height][AA_Width];



int degree = 32;
mat4 directions[] = {
	mat4::RotateY(-PI / degree), // left
	mat4::RotateY(PI / degree), // right
	};


int frame_count = 0;
int frames_rendered;


// opencl stuff
cl::Device device;
cl::Context context;
cl::CommandQueue queue;
cl::Program program;

// cl buffers
cl::Image2D image;
cl::Buffer image_buffer;
cl::Buffer accumulated_colour;

cl::Buffer rays;
cl::Buffer light_buffer;
cl::Buffer prims_buffer;
cl::Buffer mats_buffer;

// opencl kernels
cl::Kernel test_k; 
cl::Kernel generate_primary_rays;
cl::Kernel cast_rays;
cl::Kernel anti_alias;
cl::Kernel shade_intersections;
cl::Kernel direct_illumination;

PointLight lights[] = {
	{float3(5,15,0), 100.0f},
	{float3(-5,15,0), 100.0f}
};
const int num_lights = sizeof(lights) / sizeof(lights[0]);




float3 pos[] = { float3(0, 0, 20), float3(20, 0, 0),float3(-20, 0, 0), float3(0, 0, 0)};
float3 normals[] = { float3(0, 0, -1), float3(1, 0, 0), float3(-1, 0, 0), float3(0, 1, 0)};
float3 cols[] = { float3(1, 0, 1), float3(1, 1, 0), float3(0, 1, 1), float3(1, 0, 0)};
int num_prims = sizeof(pos) / sizeof(pos[0]);

vector<Primitive>  prims(num_prims);
vector<Material>  mats(num_prims);



void CreatePrims() {

	printf("Setting up primitives\n");

	for (int i = 0; i < num_prims; i++) 
	{
		float t = -(dot(normals[i], pos[i]));

		prims.at(i) = { pos[i], normals[i], t, i };
		mats.at(i) = {cols[i], 0, 1};
		
			
		//printf("%f\n", prims[i].pos.z);
	}

}






void MyApp::KeyUp(int key) {
	printf("Key %d up\n", key);


	// left (left arrow + A)
	if (key == 263 || key == 65) {
		
		//RotateWindow(&directions[0]);
		//ResetFrame();

	}

	// right (right arrow + D)
	if (key == 262 || key == 68) {
		
		//RotateWindow(&directions[1]);
		//ResetFrame();
	}

};
void MyApp::KeyDown(int key) {
	printf("Key %d down\n", key);

	// left (left arrow + A)
	if (key == 263 || key == 65);
	
	// right (right arrow + D)
	if (key == 262 || key == 68);


} 


void MyApp::Init()
{
	printf("Anti-aliassing: %d, Subpixels: %d\n", aa_res, num_subpix);
	printf("Screen res: %d X %d\n", SCRWIDTH, SCRHEIGHT);
	printf("AA Res: %d X %d\n", AA_Width, AA_Height);


	CreatePrims();


	vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);


	// get default platform
	if (platforms.size() == 0) {
		std::cout << " No platforms found.\n";
		exit(1);
	}
	cl::Platform default_platform = platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	// get device
	vector<cl::Device> devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if (devices.size() == 0) {
		std::cout << " No devices found.\n";
		exit(1);
	}
	// AMD CPU is not supported, only one device i.e. GPU
	device = devices[0];
	std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";


	// create context
	context = cl::Context({ device });

	// load program
	std::ifstream sourceFile("wavefront.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));

	program = cl::Program(context, source);

	// build program
	program.build({ device });
	std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;

	


	// Create OpenCL kernels
	generate_primary_rays = cl::Kernel(program, "generate_primary_rays");
	cast_rays = cl::Kernel(program, "cast_rays");
	shade_intersections = cl::Kernel(program, "shade_intersections");
	direct_illumination = cl::Kernel(program, "direct_illumination");


	// create queue
	queue = cl::CommandQueue(context);

	// Create buffers
	rays = cl::Buffer(context, CL_MEM_READ_WRITE, TOTAL_RAYS * sizeof(Ray));
	accumulated_colour = cl::Buffer(context, CL_MEM_READ_WRITE, TOTAL_RAYS * sizeof(float3));
	
	light_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, num_lights * sizeof(PointLight));
	queue.enqueueWriteBuffer(light_buffer, CL_TRUE, 0, num_lights * sizeof(PointLight), lights);

	prims_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, prims.size() * sizeof(Primitive));
	queue.enqueueWriteBuffer(prims_buffer, CL_TRUE, 0, prims.size() * sizeof(Primitive), prims.data());

	mats_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, mats.size() * sizeof(Material));
	queue.enqueueWriteBuffer(mats_buffer, CL_TRUE, 0, mats.size() * sizeof(Material), mats.data());




	// setupt Anti Alias
	if (aa_res > 1) {
		image_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, SCRWIDTH * SCRHEIGHT * sizeof(float3));
		anti_alias = cl::Kernel(program, "anti_alias");

		anti_alias.setArg(0, accumulated_colour);
		anti_alias.setArg(1, image_buffer);
		anti_alias.setArg(2, aa_res);		
	}


	/*
	cl::ImageFormat format;
	format.image_channel_order = CL_RGBA;
	format.image_channel_data_type = CL_FLOAT;
	image = cl::Image2D(context, CL_MEM_WRITE_ONLY, format, AA_Width, AA_Height);
	*/


	generate_primary_rays.setArg(0, rays);
	generate_primary_rays.setArg(1, TOTAL_RAYS);

	
	// set unchanging args
	generate_primary_rays.setArg(0, rays);
	generate_primary_rays.setArg(1, TOTAL_RAYS);
	generate_primary_rays.setArg(2, d);

	cast_rays.setArg(3, prims_buffer);
	cast_rays.setArg(4, static_cast<int>(prims.size()));
	
	shade_intersections.setArg(1, mats_buffer);
	shade_intersections.setArg(0, accumulated_colour);


	direct_illumination.setArg(0, accumulated_colour);
	direct_illumination.setArg(1, light_buffer);
	direct_illumination.setArg(2, num_lights);
	direct_illumination.setArg(3, prims_buffer);
	direct_illumination.setArg(4, static_cast<int>(prims.size()));


	printf("Initialization complete\n");
}






// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	cl_int result;

	// clear the screen to black
	//screen->Clear(0);



	// find roation matrix


	float3 v = float3(0, 0, 1) * V;
	float sin = length(v);
	float cos = dot(float3(0, 0, 1), V);


	//array < array<float, 3>, 3> rot_mat{0};


	mat4 rotate_mat4 = mat4::LookAt(float3(0,0,0), V);


	cl::Buffer  rotation_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, 16 * sizeof(float));
	result = queue.enqueueWriteBuffer(rotation_buffer, CL_TRUE, 0, 16 * sizeof(float), rotate_mat4.cell);
	if (result != CL_SUCCESS) std::cerr << result << std::endl;


	generate_primary_rays.setArg(3, E);
	generate_primary_rays.setArg(4, rotation_buffer);

	queue.enqueueNDRangeKernel(generate_primary_rays, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));


	


	



	frames_rendered++;


	auto start = high_resolution_clock::now();


	vector<Intersection> intersections(TOTAL_RAYS);

	cl::Buffer intersection_buffer(context, CL_MEM_READ_ONLY, TOTAL_RAYS * sizeof(Intersection));

	cl::Buffer intsection_count(context, CL_MEM_READ_WRITE, sizeof(int));


	cast_rays.setArg(0, rays);
	cast_rays.setArg(1, intersection_buffer);
	cast_rays.setArg(2, intsection_count);



	//printf("%f\n", prims[0].o);

	

	result = queue.enqueueNDRangeKernel(cast_rays, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));

	if ( result != CL_SUCCESS) std::cerr << result << std::endl;


	int * incs = new int(0);


	result = queue.enqueueReadBuffer(intsection_count, CL_TRUE, 0, sizeof(int), incs);
	if ( result != CL_SUCCESS) std::cerr << result << std::endl;
	
	printf("%d intersections\n", *incs);


	cl::Buffer directIllum;


	if (*incs == TOTAL_RAYS) {
	
	
		shade_intersections.setArg(2, intersection_buffer);


		directIllum = cl::Buffer(context, CL_MEM_READ_WRITE, TOTAL_RAYS * sizeof(Ray));
		shade_intersections.setArg(3, directIllum);
	
		result = queue.enqueueNDRangeKernel(shade_intersections, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));
		if (result != CL_SUCCESS) std::cerr << result << std::endl;
		
		direct_illumination.setArg(5, directIllum);
		result = queue.enqueueNDRangeKernel(direct_illumination, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));
		if (result != CL_SUCCESS) std::cerr << result << std::endl;




	}







	//queue.enqueueReadImage(image, CL_TRUE, {0,0,0}, );

	
	std::vector<float3> pixels(SCRHEIGHT * SCRWIDTH);
	
	// get colours, anti-alias if necessary
	if (aa_res > 1) {
		result = queue.enqueueNDRangeKernel(anti_alias, cl::NullRange, cl::NDRange(SCRWIDTH, SCRHEIGHT), cl::NDRange(16, 16));
		//std::cerr << result << std::endl;
		queue.enqueueReadBuffer(image_buffer, CL_TRUE, 0, SCRHEIGHT * SCRWIDTH * sizeof(float3), pixels.data());
	}	
	else {
		queue.enqueueReadBuffer(accumulated_colour, CL_TRUE, 0, SCRHEIGHT* SCRWIDTH * sizeof(float3), pixels.data());
	}


	// display pixels
	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){
		int id = x + y * SCRWIDTH;
		
		const float r = pixels[id].x, g = pixels[id].y, b = pixels[id].z;

		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);
	}
	


	auto stop = high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = stop - start;
	double seconds = duration.count() / 1000;

	char FPS[20];
	std::sprintf(FPS, "FPS: %.2f", 1/seconds);
	screen->Print(
		FPS,
		SCRWIDTH / 32,
		(SCRHEIGHT / 16),
		0xFFFFFFFF);
	
	//printf("Frame %d rendered in %f seconds\n", frame_count, seconds);

	frame_count++;



	V = directions[0].TransformVector(V);

}



