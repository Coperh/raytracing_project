#pragma once

// structures compatible with both C++ and OpenCL


typedef struct Ray {

	float3 O;
	float3 D;
	float t;
	int id;
};

typedef struct Intersection {

	float3 I; // 12 byte
	float3 N; // 12 byte
	float3 D; // 12 byte
	float t; // 4 bytes
	int mat; //4 bytes
	int id; // 4 btyes
};

typedef struct PointLight {
	float3 pos;
	float intensity;
};

typedef struct Primitive {
	float3 pos;
	float3 N;
	float o;
	int mat;
};

typedef struct Material {
	float3 colour;
	int type;
	float intensity;
	
};