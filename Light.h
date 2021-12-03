#pragma once
#include "myapp.h"

class Light {

public: 

	Light(float3 position, float intensity) {
	
		this->pos = position;

		this->intensity = intensity;
	
	}



	float3 pos;
	float intensity;




};