#include "Object.hpp"
#include "Print.hpp"
#include "Read.hpp"
#include "Eval.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;

	do {
		obj = read(std::cin);
		obj = eval(obj);
		std::cout << obj << std::endl;
	} while (obj);

	return 0;
}