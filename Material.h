#pragma once

class Material 
{

public:
	enum class Type
	{
		diffuse,
		reflect,
		refract,
		light
	};



	Material() {};

	Material(Type type, float3 colour, float intensity, bool is_checker = false) {
	
		this->type = type;
		this->colour = colour;

		this->intensity = intensity;

		this->is_checker = is_checker;
	}



	Type type;

	float3 colour;

	float intensity; // how intense reflection/refaction is

	bool is_checker;

	// quartz glass
	float index = 1.458;

};