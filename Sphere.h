#pragma once
#include "myapp.h"
#include "Primitive.h"



class Sphere: public Primitive{


public:


	Sphere(float3 position, float radius, Material material) {	
		pos = position;
		r2 = radius * radius;
		mat = material;
	}

	
	float r2;
	

	bool Intersect(Ray *ray)
	{


		//printf("point 2\n");

		float3 sub = ray->O - pos;
		float distance_squared = dot(sub, sub);

		// if distance squared greater than radius squared, outside of circle
		if (distance_squared <= r2){

			//printf("Out circle\n");

			float3 C = pos - ray->O;
			float t = dot(C, ray->D);
			float3 Q = C - t * ray->D;
			float p2 = dot(Q, Q);
			if (p2 > r2) return false; // r2 = r * r
			t -= sqrt(r2 - p2);
	
			if (t > 0) 
				if (ray->t  < 0 || ray->t > t) {
				 ray->t = t;
				return true;
				}
			//printf("%f\n", ray->t);

			return false;
		}

		//printf("point 3\n");


		// else inside
		//printf("In circle\n");

		float a = dot(ray->D, ray->D);

		float3 OminC = ray->O - pos;

		float b = dot(2 * ray->D, OminC);

		float c = dot(OminC, OminC) - r2;


		float root_arg = (b * b) - (4 * a * c);


		//printf("point 4\n");

		// negative, not in circle
		if (root_arg < 0) return false;

		float root = sqrtf(root_arg);

		float a2 = a * 2;

		float t1 = (-b + root) / a2;

		float t2 = (-b - root) / a2;

		//printf("point 5\n");



		float t;

		if (t1 > 0)
		{
			if (t2 > 0) {

				t = fmin(t1, t2);

			}

			t = t1;
		}
		else t = t2;

		if (t > 0)
			if (ray->t  < 0 || ray->t > t) {
				ray->t = t;
				return true;
			}

		
		//printf("T1 %f, T2 %f\n", t1, t2);

		//printf("point 6\n");

		return false;

	}




	float3 GetNormal(float3 point, float3 D) {
	
	
	
		return normalize(point - pos);
	};




	void test() { printf("I am a sphere at point (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z); }

};