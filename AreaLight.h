#pragma once



class AreaLight: public Primitive {

	AreaLight(float3 position, float3 normal, float width, Material material) {


	this->pos = position;
	//r2 = radius * radius;
	this->mat = material;
	this->N = normal;

	this->width = width;




	}

	float3 N;
	float3 pos;
	//float r2;
	Material mat;
	float width;




	float* GenPoints(int n) 
	{
	

	
	
	}



	float Intersect(Ray* ray) {

		return 0.0f;
	};


	float3 GetNormal(float3 point, float3 D) {

		if (dot(D, N) > 0)
			return -N;
		return N;
	};


};


