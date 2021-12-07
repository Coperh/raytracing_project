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

		this->is_checker == false;
	}



	Type type;

	float3 colour;

	float intensity; // how intense reflection/refaction is

	bool is_checker;

	// quartz glass
	float index = 1.458;

};