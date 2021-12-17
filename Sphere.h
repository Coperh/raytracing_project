#pragma once

class Sphere: public Primitive{


public:


	Sphere(float3 position, float radius, Material material);

	
	float r2;
	

	float Intersect(Ray* ray); 

	float3 GetNormal(float3 point, float3 D);

	void test();

};