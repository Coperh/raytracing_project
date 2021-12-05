#pragma once
#include "myapp.h"




class Material 
{

public:
	enum class Type
	{
		diffuse,
		mirror,
		glass
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

};