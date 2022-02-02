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

mat4 directions[] = {
	mat4::RotateY(-PI / 4), // left
	mat4::RotateY(PI / 4), // right
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
cl::Kernel cast_rays;
cl::Kernel anti_alias;



PointLight lights[] = {
	{float3(75,75,0), 10.0f},
	{float3(-75,75,0), 10.0f}
};
const int num_lights = sizeof(lights) / sizeof(lights[0]);




float3 pos[] = { float3(0, 0, 50), float3(20, 0, 0),float3(-20, 0, 0), float3(0, 0, 0) };
float3 normals[] = { float3(0, 0, -1), float3(1, 0, 0), float3(-1, 0, 0), float3(0, 1, 0) };
float3 cols[] = { float3(1, 0, 1), float3(1, 1, 0), float3(0, 1, 1), float3(0, 1, 0) };
int num_prims = sizeof(pos) / sizeof(pos[0]);

vector<Primitive>  prims(4);
vector<Material>  mats(4);



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
	if (program.build({ device }) != CL_SUCCESS) {
		std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;

		cin.get();

		//exit(1);
	}


	// make kernels
	cl::Kernel generate_primary_rays = cl::Kernel(program, "generate_primary_rays");

	cast_rays = cl::Kernel(program, "cast_rays");



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

	cl::ImageFormat format;
	format.image_channel_order = CL_RGBA;
	format.image_channel_data_type = CL_FLOAT;

	image = cl::Image2D(context, CL_MEM_WRITE_ONLY, format, AA_Width, AA_Height);
	

	//queue.enqueueWriteBuffer(A, CL_TRUE, 0, sizeof(int) * TOTAL_RAYS, a.data());
	//queue.enqueueWriteBuffer(A, 0, a.size() * sizeof(double), a.data());


	generate_primary_rays.setArg(0, rays);
	generate_primary_rays.setArg(1, TOTAL_RAYS);
	generate_primary_rays.setArg(2, E);
	generate_primary_rays.setArg(3, d);
	generate_primary_rays.setArg(4, V);




	//mat4 rotation_matrix = mat4::RotateX(PI);


	queue.enqueueNDRangeKernel(generate_primary_rays, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));


	//queue.enqueueReadBuffer(rays, CL_TRUE, 0, TOTAL_RAYS * sizeof(Ray), a.data());
	//queue.enqueueWriteBuffer(rays, CL_TRUE, 0, sizeof(int) * TOTAL_RAYS, a.data());

	
	// set unchanging args
	
	

	cast_rays.setArg(1, prims_buffer);
	cast_rays.setArg(2, static_cast<int>(prims.size()));
	cast_rays.setArg(3, accumulated_colour);
	


	printf("Initialization complete\n");
}






// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	
	// clear the screen to black
	//screen->Clear(0);



	cl_int result;



	frames_rendered++;


	auto start = high_resolution_clock::now();

	cast_rays.setArg(0, rays);
	
	//printf("%f\n", prims[0].o);

	

	
	result = queue.enqueueNDRangeKernel(cast_rays, cl::NullRange, cl::NDRange(AA_Width, AA_Height), cl::NDRange(32, 16));


	if ( result != CL_SUCCESS) std::cerr << result << std::endl;




	//queue.enqueueReadImage(image, CL_TRUE, {0,0,0}, );

	
	std::vector<float3> pixels(SCRHEIGHT * SCRWIDTH);
	
	//cout << aa_res << endl;
	if (aa_res > 1) {
		result = queue.enqueueNDRangeKernel(anti_alias, cl::NullRange, cl::NDRange(SCRWIDTH, SCRHEIGHT), cl::NDRange(16, 16));
		//std::cerr << result << std::endl;
		queue.enqueueReadBuffer(image_buffer, CL_TRUE, 0, SCRHEIGHT * SCRWIDTH * sizeof(float3), pixels.data());
	}	
	else {
		queue.enqueueReadBuffer(accumulated_colour, CL_TRUE, 0, SCRHEIGHT* SCRWIDTH * sizeof(float3), pixels.data());
	}



	
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

	/*
	char lumtext[32];
	sprintf(lumtext, "Energy: %.2f", energy);
	screen->Print(
		lumtext,
		SCRWIDTH / 32,
		SCRHEIGHT / 16,
		0xFFFFFFFF);
	*/

	char FPS[20];
	std::sprintf(FPS, "FPS: %.2f", 1/seconds);
	screen->Print(
		FPS,
		SCRWIDTH / 32,
		(SCRHEIGHT / 16),
		0xFFFFFFFF);
	

	//printf("Frame %d rendered in %f seconds\n", frame_count, seconds);

	frame_count++;


}



