#include "precomp.h"




Sphere::Sphere(float3 position, float radius, Material material) {
	pos = position;
	r2 = radius * radius;
	mat = material;
}


float Sphere::Intersect(Ray* ray)
{


	//printf("point 2\n");

	float3 sub = ray->O - pos;
	float distance_squared = dot(sub, sub);

	float t = -1;

	//printf("distance_squared %f  r2 %f\n", distance_squared, r2);

	// if distance squared greater than radius squared, outside of circle
	if (distance_squared >= r2) {

		//printf("Out circle\n");

		float3 C = pos - ray->O;
		t = dot(C, ray->D);

		//printf("t1: %f\n", t);

		float3 Q = C - t * ray->D;
		float p2 = dot(Q, Q);

		if (p2 > r2) return -1; // r2 = r * r

		//printf("p2: %f\n", r2 - p2);

		t -= sqrtf(r2 - p2);
		//printf("t1: %f\n", t);

	}
	else {
		// else inside

		float a = dot(ray->D, ray->D);

		float3 OminC = ray->O - pos;

		float b = dot(2 * ray->D, OminC);

		float c = dot(OminC, OminC) - r2;


		float root_arg = (b * b) - (4 * a * c);

		//printf("point 4\n");

		// negative, not in circle
		if (root_arg < 0) return -1;

		float root = sqrtf(root_arg);

		float a2 = a * 2;

		float t1 = (-b + root) / a2;

		float t2 = (-b - root) / a2;

		//printf("point 5\n");

		if (t1 > 0)
		{
			if (t2 > 0) {

				t = fmin(t1, t2);
			}
			t = t1;
		}
		else t = t2;
	}

	//printf("Found t: %f\n", t);
	return t;
}




float3 Sphere::GetNormal(float3 point, float3 D) {

	return normalize(point - pos);
};




void Sphere::test() { printf("I am a sphere at point (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z); }

