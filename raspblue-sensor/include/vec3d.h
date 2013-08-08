#ifndef __H_vec3d__
#define __H_vec3d__

#include <stdio.h>

typedef struct vec3d  vec3d;
struct vec3d {
        double x;
        double y;
        double z;
};

int trilateration(vec3d *const result1, vec3d *const result2,
                  const vec3d p1, const double r1,
                  const vec3d p2, const double r2,
                  const vec3d p3, const double r3,
                  const double maxzero);

#endif /* __H_vec3d__ */