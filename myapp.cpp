#include "precomp.h"


using namespace std::chrono;


TheApp* CreateApp() { return new MyApp(); }


static float3 E(0, 0, 0), V(0, 0, 1);
static float d = 2;


float3 frame[SCRHEIGHT][SCRWIDTH];


const int aa_res = 1;
const int num_subpix = aa_res * aa_res;
const int AA_Height = SCRHEIGHT * aa_res;
const int AA_Width = SCRWIDTH * aa_res;

Ray AntiAliasRays[AA_Height][AA_Width];




mat4 directions[] = {
	mat4::RotateY(-PI / 4), // left
	mat4::RotateY(PI / 4), // right
	};



int num_light;
AreaLight* lights[] = {
	new AreaLight(float3(5, 10, 0), float3(0, -1, 0), 2, Material(Material::Type::light, float3(200, 200,200), 0.1))
};


/*Light pointLights[] = {
	//Light(float3(-5, -10, 0), 0.5),
	Light(float3(5, -10, 0), 0.5) };
*/

int num_prim;
Primitive* primitives[] = {

	//new Plane(float3(0, 0, 10), float3(0, 0, 1), Material(Material::Type::diffuse, float3(255, 0, 255),1)),// backwall
	//new Plane(float3(10, 0, 0), float3(1, 0, 0), Material(Material::Type::diffuse, float3(200, 200, 200),1)), // right wall
	//new Plane(float3(-10, 0, 0), float3(-1, 0, 0), Material(Material::Type::diffuse, float3(255, 0, 0),1)),// left wall

	//new Plane(float3(0, 0, -10), float3(0, 0, -1), Material(Material::Type::diffuse, float3(0, 255, 255),1)),// backwall

	new Plane(float3(0, -10, 0), float3(0, 1, 0), Material(Material::Type::diffuse, float3(255, 255,0),0.5), true), //floor

	//new Plane(float3(0, 0, 10), float3(1, 0, 1), Material(Material::Type::refract, float3(0, 0, 255),0.8)),// window
	//new Plane(float3(0, 0, 12), float3(1, 0, 1), Material(Material::Type::refract, float3(0, 0, 255),0.8)),// window takes to long to render

	new Sphere(float3(0, 0, 20), 2, Material(Material::Type::diffuse, float3(0, 255,0), 0.8)),
	new Sphere(float3(20, 0, 0), 2, Material(Material::Type::diffuse, float3(0, 0, 255), 0.8)),
	new Sphere(float3(20, 0, 0), 2, Material(Material::Type::diffuse, float3(255, 0, 0), 0.8)),
	//new Sphere(float3(0, 0, 5), 2, Material(Material::Type::diffuse, float3(0,0 ,255), 1))
};


//Sphere(float3(50, 50, 100), 10, Material(Material::Type::diffuse, float3(255, 0, 0))),
//Sphere(float3(50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, -50, 100), 10, Material(Material::Type::diffuse, float3(0, 255, 0))),
//Sphere(float3(-50, 50, 100), 10, Material(Material::Type::diffuse, float3(0, 0, 255))),




int frames_rendered;

int frame_count = 0;





void RotateWindow(mat4 * dir){
	
	V = dir->TransformVector(V);

	float3 center = E + d * V;
	
	mat4* direction;


	printf("Center: %f %f %f\n", center.x, center.y, center.z);
	printf("Dir %f %f %f\n", V.x, V.y, V.z);


	for (int y = 0; y < AA_Height; y++) for (int x = 0; x < AA_Width; x++) {

		AntiAliasRays[y][x].D = dir->TransformPoint(AntiAliasRays[y][x].D);
		AntiAliasRays[y][x].O = dir->TransformPoint(AntiAliasRays[y][x].O);
		AntiAliasRays[y][x].t = -1;
	}

}



void CalculateWindow() 
{

	float3 center = E + d * V;

	printf("Center: %f %f %f\n", center.x, center.y, center.z);
	printf("Dir %f %f %f\n", V.x, V.y, V.z);

	// flippde to get better coords
	float3 P0 = center + float3(-1, 1, 0);
	float3 P1 = center + float3(1, 1, 0);
	float3 P2 = center + float3(-1, -1, 0);

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

void ResetFrame() 
{

	frames_rendered = 0;

	for (int y = 0; y < AA_Height; y++) for (int x = 0; x < AA_Width; x++) {

		frame[y][x].x = 0;
		frame[y][x].y = 0;
		frame[y][x].z = 0;
	}

}



void MyApp::KeyUp(int key) {
	printf("Key %d up\n", key);


	// left (left arrow + A)
	if (key == 263 || key == 65) {
		
		RotateWindow(&directions[0]);
		ResetFrame();

	}

	// right (right arrow + D)
	if (key == 262 || key == 68) {
		
		RotateWindow(&directions[1]);
		ResetFrame();
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
	// anything that happens only once at application start goes here

	CalculateWindow();



	num_prim = sizeof(primitives) / sizeof(primitives[0]);
	num_light = sizeof(lights) / sizeof(lights);

	printf("Scene with %d primitives and %d lights\n", num_prim, num_light);



	float3 test_f(1, 1, 4);


	printf("Before: %f, %f, %f\n", test_f.x, test_f.y, test_f.z);


	mat4 rotation_matrix = mat4::RotateX(PI);


	printf("%f \n", rotation_matrix.cell[5]);


	test_f = rotation_matrix.TransformVector(test_f);

	printf("After: %f, %f, %f\n", test_f.x, test_f.y, test_f.z);


	printf("Initialization complete\n");
}



// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick( float deltaTime )
{
	
	frames_rendered++;

	milliseconds start = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);


	// clear the screen to black
	//screen->Clear( 0 );

	

	for (int y=0; y< SCRHEIGHT; y++) for (int x = 0; x< SCRWIDTH; x++){

		// for every sub pixel

		float3 average_colour(0, 0, 0);

		//printf("Start %d %d\n", y, x);

		for (int sub_y = 0; sub_y < aa_res; sub_y++) for(int sub_x = 0; sub_x < aa_res; sub_x++) {

			int local_y = y * aa_res + sub_y;
			int local_x = x * aa_res + sub_x;


			average_colour = average_colour + Pathtrace(primitives, num_prim, lights, num_light, AntiAliasRays[local_y][local_x], 0);


			//average_colour = average_colour + WhittedTrace(primitives, num_prim, pointLights, num_light, AntiAliasRays[local_y][local_x], 0);
		}


		frame[y][x] += average_colour / num_subpix;
	

			



		//frame[y][x] = Trace(lightsource, primitives, num_prim, AntiAliasRays[y][x]);
		//printf("Col %f %f %f\n", frame[y][x].x, frame[y][x].y, frame[y][x].z);


		
		const float r = frame[y][x].x / frames_rendered, g = frame[y][x].y / frames_rendered, b = frame[y][x].z / frames_rendered;




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



