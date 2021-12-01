#pragma once
#include "myapp.h"


class Plane {


public:

	Plane(float3 normal, float3 point, float scalar) {
	
		N = normal;
		P = point;
		d = scalar;

	}


	float3 N;
	float3 P;
	float d;




};