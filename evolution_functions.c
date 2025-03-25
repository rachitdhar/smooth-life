#include "evolution_functions.h"

float s1(float x, float a)
{
	float denominator = 1.0f + expf(-(x - a) * 4 / ALPHA);
	return (float)(1.0f / denominator);
}

float s2(float x, float a, float b)
{
	return s1(x, a) * (1.0f - s1(x, b));
}

float sm(float x, float y, float m)
{
	return x * (1.0f - s1(m, 0.5f)) + y * s1(m, 0.5f);
}

float s(float n, float m)
{
	return 2.0f * s2(n, sm(BIRTH_MIN, DEATH_MIN, m), sm(BIRTH_MAX, DEATH_MAX, m)) - 1.0f;
}