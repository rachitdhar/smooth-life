#ifndef EVOLUTION_FUNCTIONS_H
#define EVOLUTION_FUNCTIONS_H

#include <math.h>

#define BIRTH_MIN	0.278f
#define BIRTH_MAX	0.365f
#define DEATH_MIN	0.267f
#define DEATH_MAX	0.445f
#define ALPHA		0.028f
#define DT			0.05f

float s1(float x, float a);
float s2(float x, float a, float b);
float sm(float x, float y, float m);
float s(float n, float m);

#endif