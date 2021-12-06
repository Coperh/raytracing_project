#pragma once
#include "myapp.h"




class Material 
{

public:
	enum class Type
	{
		diffuse,
		reflect,
		refract
	};





	Material() {};

	Material(Type type, float3 colour, float intensity) {
	
		this->type = type;
		this->colour = colour;

		this->intensity = intensity;
	}



	Type type;

	float3 colour;

	float intensity; // how intense reflection

	// quartz glass
	float index = 1.458;

};