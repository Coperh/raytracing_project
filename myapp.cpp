#include "precomp.h"
#include "myapp.h"
#include "Sphere.h"
#include "Plane.h"


#include <limits>


float3 frame[SCRHEIGHT][SCRWIDTH];


TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 0, 0), V(0, 0, 1);

static float d = 1;


Sphere spheres[5][5];


Sphere s(float3(0,0, 100), 10);

Plane plane(float3(0, 0, 100), float3(0, -1, 1), 10);

Ray Rays[SCRHEIGHT][SCRWIDTH];

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	// anything that happens only once at application start goes here


	float3 center = E + d * V;



	printf("%f %f %f\n", center.x, center.y, center.z);


	float3 P0 = center + float3(-1, -1, 0);
	float3 P1 = center + float3(1, -1, 0);
	float3 P2 = center + float3(-1, 1, 0);




	for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) 
	{
		
		
		spheres[i][j].pos =float3(-60 + 30 *i, -60 + 30 * j, 100);

		spheres[i][j].r2 = 100;
	
	
	}





	for (int y = 0; y < SCRHEIGHT; y++) for (int x = 0; x < SCRWIDTH; x++) {


		float u = float(x) / SCRWIDTH;

		float v = float(y) / SCRHEIGHT;

		//printf("U V %f %f\n", u, v);

		float3 Puv = P0 + u * (P1 - P0) + v * (P2 - P0);

		float3 D = Puv - E;


		//printf("Puv %f %f %f\n", Puv.x, Puv.y, Puv.z);


		Rays[y][x].D = normalize(D);
		Rays[y][x].O = Puv;
		Rays[y][x].t = -1; // move to seperate function.
	}

	printf("sphere %f\n", s.r2);



	// test sphere


	Ray testr;
	testr.O = float3(0, 0, 0);
	testr.D = float3(0, 0, 1);
	testr.t = std::numeric_limits<float>::max();

	s.IntersectSphere(&testr);


	//printf("T value %f\n", testr.t);

}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	// clear the screen to black
	screen->Clear( 0 );
	// print something to the console window
	//printf( "hello world!\n" );
	// plot some colors


	/*
	for( int red = 0; red < 256; red++ ) for( int green = 0; green < 256; green++ )
	{
		int x = red, y = green;
		screen->Plot( x + 200, y + 100, (red << 16) + (green << 8) );
	}
	*/






	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){



		for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) 
		{
			
			spheres[i][j].IntersectSphere(&Rays[y][x]);
		
		
		}


		//s.IntersectSphere(&Rays[y][x]);



		//printf("%f\n", Rays[y][x].t);


		if (Rays[y][x].t > 0) {

			//frame[y][x].x = 255;
			//frame[y][x].y = 0;
			//frame[y][x].z = 0;

			screen->Plot(x, y, 0x000000);

		}

		else {
		
			screen->Plot(x, y, 0xffffff);
		}

		/*
		const float r = frame[y][x].x, g = frame[y][x].y, b = frame[y][x].z;
		const uint ir = min((uint)(r * 255), 255u);
		const uint ig = min((uint)(g * 255), 255u);
		const uint ib = min((uint)(b * 255), 255u);
		screen->Plot(x, y, (ir << 16) + (ig << 8) + ib);
		*/
	}



	// plot a white pixel in the bottom right corner
	screen->Plot(SCRWIDTH - 2, SCRHEIGHT - 2, 0xffffff);

}