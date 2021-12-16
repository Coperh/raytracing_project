#pragma once
#include "myapp.h"
#include "Primitive.h"

class Plane: public Primitive {

public:
	Plane(float3 position, float3 normal, Material material, bool is_checker = false) {
		N = normal;
		pos = position;
		d = -(dot(N,pos));
		mat = material;

		mat.is_checker = is_checker;

	}
	float3 N;
	float d;


	float Intersect(Ray* ray) {

		//printf("%f\n", ray->t);

		float denom = dot(N, ray->D);

		if (denom == 0) return -1;

		float num = -(dot(ray->O, N) + d);

		float t = num / denom;


		//printf("Pred %f\n", t);

		return t;


	}

	void test() {
		printf("I am a plane at point (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z);
	}

	float3 GetNormal(float3 point, float3 D) {

		if (dot(D, N) > 0)
			return -N;
		return N;
	};
};