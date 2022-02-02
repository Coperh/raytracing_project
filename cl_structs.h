


typedef struct Ray {

	float3 O;
	float3 D;
	float t;
	int id;

};


typedef struct PointLight {
	float3 pos;
	float itensity;
};


typedef struct Primitive {
	float3 pos;
	float3 N;
	float o;
	int mat;
};

typedef struct Material {
	float3 colour;
	int mat;
	float intensity;
	
};