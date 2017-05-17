#include "Object.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	Object *obj = new ObjectList(3, new ObjectString("foo"), new ObjectAtom("bar"), new ObjectList(2, new ObjectString("baz"), new ObjectInt(5)));

	std::cout << obj << std::endl;

	return 0;
}