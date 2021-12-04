#pragma once
#include "myapp.h"
#include "Primitive.h"

class Plane: public Primitive {

public:
	Plane(float3 position, float3 normal,  Material material) {
		N = normal;
		pos = position;
		d = -(dot(N,pos));
		mat = material;

	}
	float3 N;
	float d;


	bool Intersect(Ray* ray) {

		//printf("%f\n", ray->t);

		float denom = dot(N, ray->D);

		if (denom == 0) return false;

		float num = -(dot(ray->O, N) + d);

		float t = num / denom;


		//printf("%f\n", t);


		if (t > 0)
			if (ray->t  < 0 || ray->t > t) {

		
				ray->t = t;

				//printf("%f\n", ray->t);
				return true;
			}

		return false;
	}

	void test() {
		printf("I am a plane at point (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z);
	}


};