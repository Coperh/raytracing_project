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

	Material(Type type, float3 colour) {
	
		this->type = type;
		this->colour = colour;
	}



	Type type;

	float3 colour;


};