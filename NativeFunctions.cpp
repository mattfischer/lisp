#include "NativeFunctions.hpp"

#include "Error.hpp"

#include <sstream>
#include <cstdarg>

void getArgs(Object *object, int length, ...)
{
	va_list ap;
	va_start(ap, length);
	int objectLength = 0;
	for (Object *cons = object; cons->type() != Object::TypeNone; cons = cons->consValue().cdr) {
		if (objectLength < length) {
			Object::Type type = va_arg(ap, Object::Type);
			Object **arg = va_arg(ap, Object**);
			*arg = cons->consValue().car;
			if ((*arg)->type() != type) {
				std::stringstream ss;
				ss << "Type mismatch on " << *arg << " (expected " << type << " got " << (*arg)->type() << ")";
				throw Error(ss.str());
			}
		}
		objectLength++;
	}

	if (objectLength != length) {
		std::stringstream ss;
		ss << "List " << object << " is of incorrect length (expected " << length + 1 << " got " << objectLength + 1 << ")";
		throw Error(ss.str());
	}
}

Object *NativeFunctions::add(Object *args)
{
	Object *a, *b;
	getArgs(args, 2, Object::TypeInt, &a, Object::TypeInt, &b);

	Object *ret = new Object();
	ret->setInt(a->intValue() + b->intValue());
	return ret;
}

Object *NativeFunctions::subtract(Object *args)
{
	Object *a, *b;
	getArgs(args, 2, Object::TypeInt, &a, Object::TypeInt, &b);

	Object *ret = new Object();
	ret->setInt(a->intValue() - b->intValue());
	return ret;
}

Object *NativeFunctions::multiply(Object *args)
{
	Object *a, *b;
	getArgs(args, 2, Object::TypeInt, &a, Object::TypeInt, &b);

	Object *ret = new Object();
	ret->setInt(a->intValue() * b->intValue());
	return ret;
}

Object *NativeFunctions::divide(Object *args)
{
	Object *a, *b;
	getArgs(args, 2, Object::TypeInt, &a, Object::TypeInt, &b);

	Object *ret = new Object();
	ret->setInt(a->intValue() / b->intValue());
	return ret;
}
