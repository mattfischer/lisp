#ifndef IO_HPP
#define IO_HPP

#include "Object.hpp"
#include "Context.hpp"

#include <istream>
#include <ostream>

class IO {
public:
	static Object *read(std::istream &i, Context *context);
	static void print(std::ostream &o, const Object *object);
};

std::ostream &operator<<(std::ostream &o, const Object *object);

#endif