#include "precomp.h"
#include <Windows.h>

using namespace std::chrono;

TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 10, 0), V(0, -0.2, 0.8);
static float d = 2;


float3 frame[SCRHEIGHT][SCRWIDTH];

const int aa_res = 1;
const int num_subpix = aa_res * aa_res;
const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;

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

cl::Kernel test_k;



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



	//Kernel myKernel("wavefront.cl", "test_kern");


	//OpenCL
	//clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1,&device, NULL);



	vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);



	if (platforms.size() == 0) {
		std::cout << " No platforms found.\n";
		exit(1);
	}


	cl::Platform default_platform = platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	vector<cl::Device> devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);


	if (devices.size() == 0) {
		std::cout << " No devices found.\n";
		exit(1);
	}

	device = devices[0];
	std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";



	context = cl::Context({ device });


	//queue = clCreateCommandQueue(context, device, (cl_command_queue_properties)0, NULL);



	std::ifstream sourceFile("wavefront.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));

	program = cl::Program(context, source);



	if (program.build({ device }) != CL_SUCCESS) {
		std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;


		cin.get();

		//exit(1);
	}






	queue = cl::CommandQueue(context);

	

	const int N = 100;


	std::vector<Ray> a(N);

	// Allocate device buffers and transfer input data to device.
	cl::Buffer A(context, CL_MEM_READ_WRITE, a.size() * sizeof(Ray));



	//queue.enqueueWriteBuffer(A, 0, a.size() * sizeof(double), a.data());


	test_k = cl::Kernel(program,"test_kern");

	test_k.setArg(0, A);

	queue.enqueueNDRangeKernel(test_k, cl::NullRange, N, cl::NullRange);

	//mat4 rotation_matrix = mat4::RotateX(PI);


	queue.enqueueReadBuffer(A, CL_TRUE, 0, a.size() * sizeof(Ray), a.data());



	//printf("Boop %f\n", a[12]);


	printf("Initialization complete\n");
}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	
	frames_rendered++;

	//double energy = 0;

	auto start = high_resolution_clock::now();


	// clear the screen to black
	screen->Clear( 0 );

	
	
	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		// for every sub pixel

		float3 average_colour(0, 0, 0);

		//printf("Start %d %d\n", y, x);


		frame[y][x] += average_colour / num_subpix;

		
		const float r = frame[y][x].x / frames_rendered, g = frame[y][x].y / frames_rendered, b = frame[y][x].z / frames_rendered;


		//energy += r + g + b;


		//float3 test_col(1,1,0);
		//const float r = test_col.x, g = test_col.y, b = test_col.z ;



		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);



		//if(ib == 255) printf("ib %d, in %f \n", ib, b);
	}
	

	


	auto stop = high_resolution_clock::now();

	std::chrono::duration<double, std::milli> duration = stop - start;

	double seconds = duration.count() / 1000;

	//printf("Frame %d rendered in %f seconds\n", frame_count, seconds);


	
	/*
	char lumtext[32];
	sprintf(lumtext, "Energy: %.2f", energy);
	screen->Print(
		lumtext,
		SCRWIDTH / 32,
		SCRHEIGHT / 16,
		0xFFFFFFFF);


	char FPS[20];
	sprintf(FPS, "FPS   : %.2f", 1/seconds);
	screen->Print(
		FPS,
		SCRWIDTH / 32,
		(SCRHEIGHT / 16) +10,
		0xFFFFFFFF);
	*/

	frame_count++;


}



