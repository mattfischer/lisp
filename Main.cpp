#include "Object.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;

	do {
		std::cin >> obj;
		std::cout << obj << std::endl;
	} while (obj);

	return 0;
}