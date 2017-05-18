#include "Object.hpp"
#include "Print.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;

	do {
		std::cin >> obj;

		print(std::cout, obj);
		std::cout << std::endl;
	} while (obj);

	return 0;
}