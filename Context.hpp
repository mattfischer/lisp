#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Object.hpp"

#include <istream>
#include <ostream>

class Context {
public:
	Object *read(std::istream &i);
	static void print(std::ostream &o, const Object *object);
	Object *eval(Object *object);

private:
	Object *evalFunction(Object *object);
	void evalArgs(Object *object, int length, ...);
	void checkType(Object *object, Object::Type type);
};

std::ostream &operator<<(std::ostream &o, const Object *object);

#endif