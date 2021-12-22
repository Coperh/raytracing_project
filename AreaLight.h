#pragma once



class AreaLight: public Primitive {

public:
	AreaLight(float3 position, float3 normal, float width, Material material) {


	this->pos = position;
	//r2 = radius * radius;
	this->mat = material;
	this->N = normal;





	this->width = width;
	this->halfwidth = this->width / 2;

	this->translation = mat4::Translate(normal - local_normal);

	this->area = width * width;



	}





	float3 N;
	float3 pos;

	float3 a;
	float3 b;



	//float r2;
	Material mat;
	float width;
	float halfwidth;
	float area;

	mat4 translation;




	void GenPoints(float3 * points, int n)
	{
		// WangHash
		uint seed = 10;

		float xmin = pos.x - halfwidth;
		float xmax = pos.x + halfwidth;
		float zmin = pos.z - halfwidth;
		float zmax = pos.z + halfwidth;

		for (int i = 0; i < n; i++) {
			
			float randx = RandomFloat() * (xmax -xmin) + xmin;
			float randz = RandomFloat() * (zmax - zmin) + zmin;
			
			float3 new_float(float3(randx, pos.y , randz));
		
			points[i] = new_float;
		}
	}


	float Intersect(Ray* ray) {

		return 0.0f;
	};


	float3 GetNormal(float3 point, float3 D) {

		if (dot(D, N) > 0)
			return -N;
		return N;
	};

private:

	float3 const local_normal{ 0.0,0.0f,1.0f };

};


