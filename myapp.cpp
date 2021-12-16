#include "precomp.h"
#include "myapp.h"
#include "Sphere.h"
#include "Plane.h"
#include "Material.h"
#include "WhittedTrace.h"
#include "Light.h"
#include <chrono>


using namespace std::chrono;





TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 0, 0), V(0, 0, 1);
static float d = 0.5;


float3 frame[SCRHEIGHT][SCRWIDTH];


const int aa_res = 1;
const int num_subpix = aa_res * aa_res;
const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;

Ray AntiAliasRays[AA_Height][AA_Width];





int current_dir = 0;

float3 dir8[] =
{
	float3(0,0,1),
	float3(1,0,1),
	float3(1,0,0),
	float3(1,0,-1),
	float3(0,0,-1),
	float3(-1,0,-1),
	float3(-1,0,0),
	float3(-1,0,1)
};


int num_light;
Light lightsources[] = { 
	Light(float3(-5, -10, 0), 0.5),
	Light(float3(5, -10, 0), 0.5) };


int num_prim;
Primitive* primitives[] = {

	new Plane(float3(0, 0, 10), float3(0, 0, 1), Material(Material::Type::diffuse, float3(255, 0, 255),1)),// backwall
	new Plane(float3(10, 0, 0), float3(1, 0, 0), Material(Material::Type::diffuse, float3(200, 200, 200),1)), // right wall
	new Plane(float3(-10, 0, 0), float3(-1, 0, 0), Material(Material::Type::diffuse, float3(255, 0, 0),1)),// left wall

	//new Plane(float3(0, 0, -10), float3(0, 0, -1), Material(Material::Type::diffuse, float3(0, 255, 255),1)),// backwall

	new Plane(float3(0, 10, 0), float3(0, 1, 0), Material(Material::Type::reflect, float3(255, 255,0),0.5), true), //floor

	//new Plane(float3(0, 0, 10), float3(1, 0, 1), Material(Material::Type::refract, float3(0, 0, 255),0.8)),// window
	//new Plane(float3(0, 0, 12), float3(1, 0, 1), Material(Material::Type::refract, float3(0, 0, 255),0.8)),// window takes to long to render

	new Sphere(float3(0, 0, 5), 2, Material(Material::Type::refract, float3(0, 255,0), 0.5))
	//new Sphere(float3(0, 0, 5), 2, Material(Material::Type::diffuse, float3(0,0 ,255), 1))
};



//Sphere(float3(50, 50, 100), 10, Material(Material::Type::diffuse, float3(255, 0, 0))),
//Sphere(float3(50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, 50, 100), 10, Material(Material::Type::diffuse, float3(0, 0, 255))),




int frame_count = 0;




void CalculateWindow() 
{

	V = dir8[current_dir];

	float3 center = E + d * V;

	printf("Center: %f %f %f\n", center.x, center.y, center.z);
	printf("Dir %d: %f %f %f\n", current_dir, V.x, V.y, V.z);

	float3 P0 = center + float3(-1, -1, 0);
	float3 P1 = center + float3(1, -1, 0);
	float3 P2 = center + float3(-1, 1, 0);

	for (int y = 0; y < AA_Height; y++) for (int x = 0; x < AA_Width; x++) {

		float u = float(x) / AA_Width;
		float v = float(y) / AA_Height;
		float3 Puv = P0 + u * (P1 - P0) + v * (P2 - P0);
		float3 D = Puv - E;

		AntiAliasRays[y][x].D = normalize(D);
		AntiAliasRays[y][x].O = Puv;
		AntiAliasRays[y][x].t = -1;
	}

}





void MyApp::KeyUp(int key) {
	printf("Key %d up\n", key);


	// left (left arrow + A)
	if (key == 263 || key == 65) {
		
		current_dir --;

		if (current_dir < 0) current_dir = 7;

		CalculateWindow();


	}

	// right (right arrow + D)
	if (key == 262 || key == 68) {
		/*
			mat4 right = mat4::RotateZ(45);

			quat temp;
			temp.fromMatrix(right);

			V = temp.rotateVector(V);
			*/
		current_dir ++;

		if (current_dir > 7) current_dir = 0;
		


		CalculateWindow();


	}

};
void MyApp::KeyDown(int key) {
	printf("Key%d down\n", key);

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
	// anything that happens only once at application start goes here

	CalculateWindow();




	num_prim = sizeof(primitives) / sizeof(primitives[0]);

	num_light = sizeof(lightsources) / sizeof(lightsources[0]);


	printf("Scene with %d primitives and %d lights\n", num_prim, num_light);



	//primitives[0]->test();

	/*
	for (int i = 0; i < num_prim; i++) {
	
		printf("Shape %d, is %d\n", i, primitives[i]->mat.is_checker);
	
	}
	*/

	printf("Initialization complete\n");
}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{


	milliseconds start = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);


	//time_t start, end;
	//time(&start);

	// clear the screen to black
	screen->Clear( 0 );

	

	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		// for every sub pixel

		float3 average_colour(0, 0, 0);

		//printf("Start %d %d\n", y, x);

		for (int sub_y = 0; sub_y < aa_res; sub_y++) for(int sub_x = 0; sub_x < aa_res; sub_x++) {

			int local_y = y * aa_res + sub_y;
			int local_x = x * aa_res + sub_x;

			average_colour = average_colour + Trace(primitives, num_prim, lightsources, num_light, AntiAliasRays[local_y][local_x], 0);
		}




		frame[y][x] = average_colour / num_subpix;
	

		//frame[y][x] = Trace(lightsource, primitives, num_prim, AntiAliasRays[y][x]);
		//printf("Col %f %f %f\n", frame[y][x].x, frame[y][x].y, frame[y][x].z);


		
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);

		//printf("Done %d %d\n", y, x);
	}


	milliseconds end = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);

	double time = (end - start).count()/1000;

	printf("Frame %d rendered in %f seconds\n", frame_count, time);



	frame_count++;

}



