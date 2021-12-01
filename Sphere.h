#pragma once
#include "myapp.h"

class Sphere{


public:


	Sphere() {}


	Sphere(float3 position, float radius) {
		
		pos = position;

		r2 = radius * radius;
	
	}



	float3 pos;

	float r2;


	void IntersectSphere(Ray *ray)
	{
		float3 C = pos - ray->O;
		float t = dot(C, ray->D);
		float3 Q = C - t * ray->D;
		float p2 = dot(Q, Q);
		if (p2 > r2) return; // r2 = r * r
		t -= sqrt(r2 - p2);

		//printf("%f\n", t);
		//if ((t < ray->t) && (t > 0)) ray->t = t; // Need to get min 
		if(t > 0) ray->t = t;
	}

};