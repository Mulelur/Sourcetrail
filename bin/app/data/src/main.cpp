#include "header.h"

int main();
void foo();
int sum(int a, int b);
int diff(int a, int b);

int main()
{
	int a = sum(1, 2);
	int b = diff(a, 3);
	int c = a * b;
	return 0;
}

int sum(int a, int b);

int diff(int a, int b);

void foo()
{
	std::string foo = "bar";
}

int diff(int a, int b);

int sum(int a, int b)
{
	return a + b;
}

int diff(int a, int b)
{
	return a - b;
}
