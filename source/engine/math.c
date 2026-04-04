#include "math.h"

double square(double x)
{
	return x * x;
}

double clamp(double x, double l, double h)
{
	return fmin(fmax(x, l), h);
}

double signum(double x)
{
	return (x > 0.0) - (x < 0.0);
}

double frac(double x)
{
	return x - floor(x);
}

int isquare(int x)
{
	return x * x;
}

int imin(int x, int y)
{
	return x < y ? x : y;
}

int imax(int x, int y)
{
	return x > y ? x : y;
}
