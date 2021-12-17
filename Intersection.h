#pragma once

float3 ReflectRay(float3 D, float3 N);


bool AnyIntersection(Primitive* objects[], int n, Ray ray);



int NearestIntersection(Primitive* objects[], int n, Ray* r, float3* intersection, float3* normal);