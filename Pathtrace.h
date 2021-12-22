#pragma once




float3 Sample(Primitive* objects[], int n, AreaLight* lights[], int m, Ray ray, int bounce);


class Pathtrace 
{
public:
	static float3 DiffuseReflection(Primitive* objects[], int n, AreaLight* lights[], int m,  float3 diffuse, float3 interseciton, float3 normal);


};
