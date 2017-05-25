#include "ObjectPool.hpp"

Object *ObjectPool::newObject()
{
	return new Object();
}

Object *ObjectPool::newNone()
{
	Object *object = newObject();
	object->setNone();

	return object;
}

Object *ObjectPool::newBool(bool value)
{
	Object *object = newObject();
	object->setBool(value);

	return object;
}

Object *ObjectPool::newInt(int value)
{
	Object *object = newObject();
	object->setInt(value);
	
	return object;
}

Object *ObjectPool::newAtom(const std::string &value)
{
	Object *object = newObject();
	object->setAtom(value);
	
	return object;
}

Object *ObjectPool::newString(const std::string &value)
{
	Object *object = newObject();
	object->setString(value);
	
	return object;
}

Object *ObjectPool::newCons(Object *car, Object *cdr)
{
	Object *object = newObject();
	object->setCons(car, cdr);
	
	return object;
}

Object *ObjectPool::newLambda(std::vector<std::string> &&variables, Object *body)
{
	Object *object = newObject();
	object->setLambda(std::move(variables), body);
	
	return object;
}

Object *ObjectPool::newNativeFunction(Object::NativeFunction nativeFunction)
{
	Object *object = newObject();
	object->setNativeFunction(nativeFunction);
	
	return object;
}

Object *ObjectPool::newEllipses()
{
	Object *object = newObject();
	object->setEllipses();

	return object;
}
