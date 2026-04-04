#include "random.h"

static unsigned int s_seed = 1;

void random_set_seed(int seed)
{
	s_seed = (seed == 0) ? 1 : seed;
}

int random_int()
{
	s_seed ^= s_seed << 13;

	s_seed ^= s_seed >> 17;

	s_seed ^= s_seed << 5;

	int value = s_seed & RANDOM_MAX;

	return value;
}

int random_int_below(int x)
{
	return random_int() % x;
}

int random_int_in_range(int l, int h)
{
	return l + random_int_below(h - l + 1);
}

double random_real()
{
	return (double)random_int() / RANDOM_MAX;
}

double random_real_below(double x)
{
	return random_real() * x;
}

double random_real_in_range(double l, double h)
{
	return l + random_real_below(h - l);
}
