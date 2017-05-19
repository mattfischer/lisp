#include "Eval.hpp"
#include "Print.hpp"

#include <iostream>
#include <cstring>
#include <cstdarg>

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

bool checkType(Object *object, Object::Type type)
{
	Object::Type objectType;

	if (object) {
		objectType = object->type();
	} else {
		objectType = Object::TypeNone;
	}

	if (objectType != type) {
		std::cerr << "Error: Type mismatch on " << object << " (expected " << type << " got " << objectType << ")" << std::endl;
		return false;
	}

	return true;
}

bool checkArgs(Object *object, int length, ...)
{
	if (!checkType(object, Object::TypeCons)) {
		return false;
	}

	Object *cons = object->cdrValue();
	va_list ap;
	va_start(ap, length);
	for (int i = 0; i < length; i++) {
		if (!cons) {
			std::cerr << "Error: List " << object << " is of incorrect length (expected " << length << ")" << std::endl;
			return false;
		}

		Object::Type type = va_arg(ap, Object::Type);
		if (!checkType(cons->carValue(), type)) {
			return false;
		}
		cons = cons->cdrValue();
	}

	if (cons) {
		std::cerr << "Error: List " << object << " is of incorrect length (expected " << length << ")" << std::endl;
		return false;
	}

	return true;
}

void evalArgs(Object *object, int length, ...)
{
	Object *cons = object->cdrValue();

	va_list ap;
	va_start(ap, length);
	for (int i = 0; i < length; i++) {
		Object **arg = va_arg(ap, Object**);
		*arg = eval(cons->carValue());
		cons = cons->cdrValue();
	}
}

Object *evalFunction(Object *object)
{
	Object *car = object->carValue();

	if(!checkType(car, Object::TypeAtom)) {
		return 0;
	}

	if (!std::strcmp(car->stringValue(), "+")) {
		if (!checkArgs(object, 2, Object::TypeInt, Object::TypeInt)) {
			return 0;
		}

		Object *a, *b;
		evalArgs(object, 2, &a, &b);

		Object *ret = new Object();
		ret->setInt(a->intValue() + b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "-")) {
		if (!checkArgs(object, 2, Object::TypeInt, Object::TypeInt)) {
			return 0;
		}

		Object *a, *b;
		evalArgs(object, 2, &a, &b);

		Object *ret = new Object();
		ret->setInt(a->intValue() - b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "*")) {
		if (!checkArgs(object, 2, Object::TypeInt, Object::TypeInt)) {
			return 0;
		}

		Object *a, *b;
		evalArgs(object, 2, &a, &b);

		Object *ret = new Object();
		ret->setInt(a->intValue() * b->intValue());
		return ret;
	}
	else if (!std::strcmp(car->stringValue(), "/")) {
		if (!checkArgs(object, 2, Object::TypeInt, Object::TypeInt)) {
			return 0;
		}

		Object *a, *b;
		evalArgs(object, 2, &a, &b);

		Object *ret = new Object();
		ret->setInt(a->intValue() / b->intValue());
		return ret;
	}

	std::cerr << "Error: No function " << car->stringValue() << std::endl;
	return 0;
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