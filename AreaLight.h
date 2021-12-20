#pragma once



class AreaLight: public Primitive {

public:
	AreaLight(float3 position, float3 normal, float width, Material material) {


	this->pos = position;
	//r2 = radius * radius;
	this->mat = material;
	this->N = normal;

	this->width = width;

	translation = mat4::Translate(normal - local_normal);

	area = width * width;

	}





	float3 N;
	float3 pos;
	//float r2;
	Material mat;
	float width;
	float area;

	mat4 translation;




	void GenPoints(float3 * points, int n)
	{

		// WangHash
		uint seed = 10;

		float xmin = pos.x - width / 2;
		float xmax = pos.x + width / 2;
		float ymin = pos.y - width / 2;
		float ymax = pos.y + width / 2;


		for (int i = 0; i < n; i++) {
			
			float randx = RandomFloat() * (xmax -xmin) + xmin;
			float randy = RandomFloat() * (ymax - ymin) + ymin;
			
			float3 new_float = translation.TransformPoint(float3(randx, randy, 0));
		
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


