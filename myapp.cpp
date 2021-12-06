#include "precomp.h"
#include "myapp.h"
#include "Sphere.h"
#include "Plane.h"
#include "Material.h"
#include "Intersection.h"
#include "Light.h"
#include <time.h>



float3 frame[SCRHEIGHT][SCRWIDTH];


TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 0, 0), V(0, 0, 1);

static float d = 0.5;




Light lightsources[] = { 
	Light(float3(-100, -200, 0), 200),
	Light(float3(100, -200, 0), 200) };

int num_light;

Primitive* primitives[] = {

	//new Sphere(float3(-50,-50, 100), 10, Material(Material::Type::diffuse, float3(255,0,0),1)),
	new Plane(float3(0, 0, 200), float3(0, 0, 1), Material(Material::Type::diffuse, float3(255, 0, 255),1)),// backwall
	new Plane(float3(200, 0, 0), float3(1, 0, 0), Material(Material::Type::diffuse, float3(200, 200, 200),1)), // right wall
	new Plane(float3(-200, 0, 0), float3(-1, 0, 0), Material(Material::Type::diffuse, float3(255, 0, 0),1)),// left wall
	new Plane(float3(0, 200, 0), float3(0, 1, 0), Material(Material::Type::reflect, float3(255, 255,0),0)), //floor

	new Sphere(float3(0, 0, 20), 10, Material(Material::Type::refract, float3(0, 255,0),0.8))
	//new Sphere(float3(0, 5, 10), 2, Material(Material::Type::diffuse, float3(0,0 ,255), 1))
};

int num_prim;


//Sphere(float3(50, 50, 100), 10, Material(Material::Type::diffuse, float3(255, 0, 0))),
//Sphere(float3(50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, 50, 100), 10, Material(Material::Type::diffuse, float3(0, 0, 255))),








const int aa_res = 2;

const int num_subpix = aa_res * aa_res;

const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;




Ray AntiAliasRays[AA_Height][AA_Width];



int frame_count = 0;



void MyApp::KeyUp(int key) {
	printf("Key %d up\n", key);


	// left (left arrow + A)
	if (key == 263 || key == 65)
	turning_left = false;
	
	// right (right arrow + D)
	if (key == 262 || key == 68)
		turning_right = false;
};
void MyApp::KeyDown(int key) {
	printf("Key%d down\n", key);

	// left (left arrow + A)
	if (key == 263 || key == 65)
	turning_left = true;

	// right (right arrow + D)
	if (key == 262 || key == 68)
	turning_right = true;


} 




void MyApp::Init()
{

	turning_left = false;
	turning_right = false;


	printf("Anti-aliassing: %d, Subpixels: %d\n", aa_res, num_subpix);

	// anything that happens only once at application start goes here

	float3 center = E + d * V;

	printf("%f %f %f\n", center.x, center.y, center.z);

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


	num_prim = sizeof(primitives) / sizeof(primitives[0]);

	num_light = sizeof(lightsources) / sizeof(lightsources[0]);


	printf("Number of primitives %d\n", num_prim);


	primitives[0]->test();
	//primitives[2]->test();
	//primitives[0]->Intersect(&AntiAliasRays[0][0]);


	Ray test_r = { float3(0,0,0), float3(0,0,1), 10 };

	Plane test_plane(float3(0, 0, 100), float3(0, 0, 1), Material(Material::Type::diffuse, float3(200, 200, 200), 1));

	printf("%f\n", test_plane.d);


	if (test_plane.Intersect(&test_r)) 
	{
		printf("Hit with %f\n", test_r.t);
	}
	else printf("Missed\n");

}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{


	time_t start, end;
	time(&start);

	// clear the screen to black
	screen->Clear( 0 );


	if (turning_left) {
		if (!turning_right) {
			// do something
			printf("Turning Left\n");
		}
	}
	else if (turning_right) 
	{
		// do something
		printf("Turning Right\n");
	}
	


	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		// for every sub pixel

		float3 average_colour(0, 0, 0);

		
		for (int sub_y = 0; sub_y < aa_res; sub_y++) for(int sub_x = 0; sub_x < aa_res; sub_x++) {

			int local_y = y * aa_res + sub_y;
			int local_x = x * aa_res + sub_x;

			average_colour = average_colour + Trace(primitives, num_prim, lightsources, num_light, AntiAliasRays[local_y][local_x]);
		}

		frame[y][x] = average_colour / num_subpix;
		

		//frame[y][x] = Trace(lightsource, primitives, num_prim, AntiAliasRays[y][x]);
		//printf("Col %f %f %f\n", frame[y][x].x, frame[y][x].y, frame[y][x].z);


		
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);


		
		
	}


	time(&end);
	double dif = difftime(start, end );

	printf("Frame %d rendered in %f seconds\n", frame_count, dif);



	frame_count++;

}

