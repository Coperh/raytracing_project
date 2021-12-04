#include "precomp.h"
#include "myapp.h"
#include "Sphere.h"
#include "Plane.h"
#include "Material.h"
#include "Intersection.h"
#include "Light.h"





float3 frame[SCRHEIGHT][SCRWIDTH];


TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 0, 0), V(0, 0, 1);

static float d = 1;



Light lightsource(float3(75, -75, 0), 1);



Primitive *  primitives[] = {
	
	new Sphere(float3(50,-50, 100), 10, Material(Material::Type::diffuse, float3(255,0,0))),
	new Sphere(float3(0,0, 200), 100, Material(Material::Type::diffuse, float3(255,255,0)))
};


//Sphere(float3(50, 50, 100), 10, Material(Material::Type::diffuse, float3(255, 0, 0))),
//Sphere(float3(50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, 50, 100), 10, Material(Material::Type::diffuse, float3(0, 0, 255))),


Plane plane(float3(9, 0, 100), float3(0, -1, 1), 10);



int num_prim;




const int aa_res = 2;

const int num_subpix = aa_res * aa_res;

const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;


Ray AntiAliasRays[AA_Height][AA_Width];





void MyApp::Init()
{
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

	printf("Number of primitives %d\n", num_prim);


	primitives[0]->test();
	
	//primitives[0]->Intersect(&AntiAliasRays[0][0]);

}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	// clear the screen to black
	screen->Clear( 0 );

	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		// for every sub pixel

		float3 average_colour(0, 0, 0);

		for (int sub_y = 0; sub_y < aa_res; sub_y++) for(int sub_x = 0; sub_x < aa_res; sub_x++) {

			int local_y = y * aa_res + sub_y;
			int local_x = x * aa_res + sub_x;

			average_colour += Trace(lightsource, primitives, num_prim, AntiAliasRays[local_y][local_x]);
		}

		frame[y][x] = average_colour / num_subpix;



		//printf("Col %f %f %f\n", frame[y][x].x, frame[y][x].y, frame[y][x].z);


		
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);
		
	}




}

