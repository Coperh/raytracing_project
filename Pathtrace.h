#pragma once




float3 Pathtrace(Primitive* objects[], int n, AreaLight* lights[], int m, Ray ray, int bounce);


class Pathtrace 
{
public:
	static void DirectIllumination(Primitive* objects[], int n, AreaLight* lights[], int m, float3 interseciton, float3 normal);


};
