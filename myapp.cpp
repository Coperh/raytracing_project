#include "precomp.h"
#include "myapp.h"
#include "Sphere.h"
#include "Plane.h"
#include "Material.h"



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


void NearestIntersection(Sphere objects[], int n, Ray* r, float3 * intersection, float3* normal, Material * material ) {
	r->t = -1;



	for (int i = 0; i < n; i++)
	{
		
		
		
		if (objects[i].Intersect(r)) {
		
			material->type =  objects[i].mat.type;
			material->colour = objects[i].mat.colour;
		}
	}


	

	//printf("%f\n", r->t);
}


float3 Trace(Sphere objects[],int n,  Ray ray) {

	float3 N;
	float3 I;
	Material mat;
	float colour;

	NearestIntersection(objects, n,  &ray, &I, &N, &mat);




	if (ray.t < 0)
		return float3(0, 0, 0);

	if (mat.type == Material::Type::diffuse)
		return mat.colour;

	else if (mat.type == Material::Type::mirror);
	else if (mat.type == Material::Type::glass);

	return float3(0, 0, 0);
}


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

		frame[y][x] = Trace(Spheres, 4, Rays[y][x]);


		//printf("%f\n", Rays[y][x].t);

		/*
		if (Rays[y][x].t > 0) {

			screen->Plot(x, y, 0x000000);
		}
		else {
			screen->Plot(x, y, 0xffffff);
		}
		*/
		
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);
		
	}




}

