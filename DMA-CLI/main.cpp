#include <cstdio>
#include <cstdlib>

#include "core.h"

int main(int argc, char** argv)
{
	printf("Hello, World!\n");

	int a = 4;
	int b = 7;
	Complex* r = new Complex();
	Test(a, b, r);
	printf("Test(%d, %d) = %f + %fi\n", a, b, r->real, r->imag);

	return EXIT_SUCCESS;
}