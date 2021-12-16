#pragma once


#include<limits>



float3 ReflectRay(float3 D, float3 N) {
	return D - 2 * (dot(D, N) * N);
}



bool AnyIntersection(Primitive* objects[], int n, Ray ray) {

	// any intersection less than distance of ray

	//printf("2: %f %f %f\n", ray.D.x, ray.D.y, ray.D.z);


	for (int i = 0; i < n; i++)
	{
		//printf("Sphape: %d, T:%f\n",i, r.t);

		// only returns true if there is an intersection nearer than the lightsource


		float t = objects[i]->Intersect(&ray);


		if (t > 0 && t < ray.t) {

			return true;
		}
	}

	//printf("boop\n");
	return false;
}



int NearestIntersection(Primitive* objects[], int n, Ray* r, float3* intersection, float3* normal) {

	r->t = -1;

	int nearest = -1;

	float best_t = FLT_MAX;


	for (int i = 0; i < n; i++)
	{


		float new_t = objects[i]->Intersect(r);

		//printf("new t %f\n", new_t);

		if (new_t > 0 && new_t < best_t) {

			//printf("new t %f\n", new_t);

			best_t = new_t;

			nearest = i;
		}
		
	}

	if (nearest >= 0) {



		//printf("best t %f\n", best_t);

		//r->t = best_t;

		float3 I = r->O + r->D * best_t;

		intersection->x = I.x;
		intersection->y = I.y;
		intersection->z = I.z;

		float3 N = objects[nearest]->GetNormal(I, r->D);

		normal->x = N.x;
		normal->y = N.y;
		normal->z = N.z;
	}
	return nearest;
}
