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







Sphere  Spheres[] = {
	Sphere(float3(50,50, 100), 10, Material(Material::Type::diffuse, float3(255,0,0) ) ),
	Sphere(float3(50,-50, 100), 10, Material(Material::Type::diffuse, float3(0,255,0)) ),
	Sphere(float3(-50,-50, 100), 10, Material(Material::Type::diffuse, float3(0,255,0)) ) ,
	Sphere(float3(-50,50, 100), 10, Material(Material::Type::diffuse, float3(255,255,0)))};



Plane plane(float3(0, 0, 100), float3(0, -1, 1), 10);

Ray Rays[SCRHEIGHT][SCRWIDTH];



Light lightsource(float3(0, 0, 50), 0.8);




void MyApp::Init()
{
	// anything that happens only once at application start goes here

	float3 center = E + d * V;

	printf("%f %f %f\n", center.x, center.y, center.z);

	float3 P0 = center + float3(-1, -1, 0);
	float3 P1 = center + float3(1, -1, 0);
	float3 P2 = center + float3(-1, 1, 0);



	for (int y = 0; y < SCRHEIGHT; y++) for (int x = 0; x < SCRWIDTH; x++) {


		float u = float(x) / SCRWIDTH;
		float v = float(y) / SCRHEIGHT;
		float3 Puv = P0 + u * (P1 - P0) + v * (P2 - P0);
		float3 D = Puv - E;
		Rays[y][x].D = normalize(D);
		Rays[y][x].O = Puv;
		Rays[y][x].t = -1;
	}



}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	// clear the screen to black
	screen->Clear( 0 );

	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		frame[y][x] = Trace(lightsource, Spheres, 4, Rays[y][x]);


		//printf("Col %f %f %f\n", frame[y][x].x, frame[y][x].y, frame[y][x].z);


		
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);
		
	}




}

