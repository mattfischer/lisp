#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include "Object.hpp"

class ObjectPool
{
public:
	Object *newObject();
	
	Object *newNone();
	Object *newInt(int value);
	Object *newBool(bool value);
	Object *newSymbol(const std::string &value);
	Object *newString(const std::string &value);
	Object *newCons(Object *car, Object *cdr);
	Object *newLambda(std::vector<std::string> &&variables, Object *body);
	Object *newNativeFunction(Object::NativeFunction nativeFunction);
};
#endif