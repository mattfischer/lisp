#include "Eval.hpp"
#include "Print.hpp"
#include "Error.hpp"

#include <iostream>
#include <cstring>
#include <cstdarg>
#include <sstream>

std::ostream &operator<<(std::ostream &o, Object::Type type)
{
	switch (type) {
	case Object::TypeNone:
		o << "nil";
		break;

	case Object::TypeInt:
		o << "int";
		break;

	case Object::TypeString:
		o << "string";
		break;

	case Object::TypeAtom:
		o << "atom";
		break;

	case Object::TypeCons:
		o << "cons";
		break;
	}

	return o;
}

void checkType(Object *object, Object::Type type)
{
	Object::Type objectType;

	if (object) {
		objectType = object->type();
	} else {
		objectType = Object::TypeNone;
	}

	if (objectType != type) {
		std::stringstream ss;
		ss << "Type mismatch on " << object << " (expected " << type << " got " << objectType << ")";
		throw Error(ss.str());
	}
}

void evalArgs(Object *object, int length, ...)
{
	va_list ap;
	va_start(ap, length);
	int objectLength = 0;
	for (Object *cons = object->cdrValue(); cons; cons = cons->cdrValue()) {
		if (objectLength < length) {
			Object **arg = va_arg(ap, Object**);
			*arg = eval(cons->carValue());
		}
		objectLength++;
	}

	if (objectLength != length) {
		std::stringstream ss;
		ss << "List " << object << " is of incorrect length (expected " << length + 1 << " got " << objectLength + 1 << ")";
		throw Error(ss.str());
	}
}

Object *evalFunction(Object *object)
{
	Object *car = object->carValue();

	checkType(car, Object::TypeAtom);

	if (!std::strcmp(car->stringValue(), "+")) {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() + b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "-")) {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() - b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "*")) {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() * b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "/")) {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() / b->intValue());
		return ret;
	}

	std::stringstream ss;
	ss << "No function " << car->stringValue();
	throw Error(ss.str());
}

Object *eval(Object *object)
{
	if (!object) {
		return object;
	}

	switch (object->type()) {
	case Object::TypeInt:
	case Object::TypeString:
	case Object::TypeAtom:
		return object;

	default:
		break;
	}

	return evalFunction(object);
}