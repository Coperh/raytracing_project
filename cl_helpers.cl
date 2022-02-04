
#include "cl_helpers.h"


float intersect_plane(struct Prim* plane, struct Ray* ray)
{


	float denom = dot(*prim.N, *ray.D);

	if (denom != 0)
	{
		float num = -(dot(*ray.O, prim.N) + prim.o);
		float t = num / denom;


		return t;
	}
	return -1;
}