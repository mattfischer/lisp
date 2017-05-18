#include "Object.hpp"
#include "Print.hpp"
#include "Read.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;

	do {
		obj = read(std::cin);
		print(std::cout, obj);
		std::cout << std::endl;
	} while (obj);

	return 0;
}