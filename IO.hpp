#ifndef IO_HPP
#define IO_HPP

#include "Object.hpp"
#include "ObjectPool.hpp"

#include <istream>
#include <ostream>

class IO {
public:
	static Object *read(std::istream &i, ObjectPool *pool);
	static void print(std::ostream &o, const Object *object);
};

std::ostream &operator<<(std::ostream &o, const Object *object);
std::ostream &operator<<(std::ostream &o, Object::Type type);

#endif