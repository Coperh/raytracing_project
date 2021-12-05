#pragma once
#include "Material.h"

class Primitive
{
public:
	Material mat;
	float3 pos;

	virtual bool Intersect(Ray* ray) = 0;


	virtual void test() { printf("Generic primitive\n"); }


	virtual float3 GetNormal(float3 point, float3 D) = 0;
};