#include "Context.hpp"
#include "Object.hpp"
#include "Error.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj;
	Context context;

	do {
		try {
			obj = context.read(std::cin);
			obj = context.eval(obj);
			std::cout << obj << std::endl;
		}
		catch (Error error) {
			std::cerr << "Error: " << error.message() << std::endl;
			return 1;
		}
	} while (obj);

	return 0;
}