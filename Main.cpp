#include "Object.hpp"
#include "Print.hpp"
#include "Read.hpp"
#include "Eval.hpp"
#include "Error.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;

	do {
		try {
			obj = read(std::cin);
			obj = eval(obj);
		}
		catch (Error error) {
			std::cerr << "Error: " << error.message() << std::endl;
			return 1;
		}

		std::cout << obj << std::endl;
	} while (obj);

	return 0;
}