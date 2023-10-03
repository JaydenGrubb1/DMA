#include "core.h"

void Test(int a, int b, Complex *c)
{
	c->real = a + b;
	c->imag = a - b;
}