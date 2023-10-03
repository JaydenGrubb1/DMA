#include <cstdio>
#include <cstdlib>

#include "core.h"

int main(int argc, char** argv)
{
	printf("Hello, World!\n");

	int a = 4;
	int b = 7;
	int r = Test(a, b);
	printf("%d + %d = %d\n", a, b, r);

	return EXIT_SUCCESS;
}