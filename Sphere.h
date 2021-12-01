#pragma once
#include "myapp.h"
#include "Material.h"


class Sphere{


public:


	Sphere(float3 position, float radius, Material material) {	
		pos = position;
		r2 = radius * radius;
		mat = material;
	}

	float3 pos;
	float r2;
	Material mat;

	bool Intersect(Ray *ray)
	{
		float3 C = pos - ray->O;
		float t = dot(C, ray->D);
		float3 Q = C - t * ray->D;
		float p2 = dot(Q, Q);
		if (p2 > r2) return false; // r2 = r * r
		t -= sqrt(r2 - p2);

		
		//printf("%f\n", t);
	
		if (t > 0) 
			if (ray->t  < 0 || ray->t > t) {
			 ray->t = t;
			return true;
			}
		

		//printf("%f\n", ray->t);

		return false;
	}

};