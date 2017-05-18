#include "Eval.hpp"
#include "Print.hpp"

#include <iostream>
#include <cstring>

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

bool checkLength(Object *object, int length)
{
	if (!checkType(object, Object::TypeCons)) {
		return false;
	}

	Object *cons = object;
	int objectLength = 0;
	while (cons) {
		objectLength++;
		cons = cons->cdrValue();
	}

	if (objectLength != length) {
		std::cerr << "Error: List " << object << " is of incorrect length (expected " << length << " got " << objectLength << ")" << std::endl;
		return false;
	}

	return true;
}

Object *listItem(Object *object, int index)
{
	Object *cons = object;
	for (int i = 0; i < index; i++) {
		if (!cons) {
			return 0;
		}
		cons = cons->cdrValue();
	}

	return cons->carValue();
}

Object *evalFunction(Object *object)
{
	Object *car = object->carValue();

	if(!checkType(car, Object::TypeAtom)) {
		return 0;
	}

	if (!std::strcmp(car->stringValue(), "+")) {
		if (!checkLength(object, 3)) {
			return 0;
		}

		Object *a = listItem(object, 1);
		Object *b = listItem(object, 2);
		if (!checkType(a, Object::TypeInt) || !checkType(b, Object::TypeInt)) {
			return 0;
		}

		Object *ret = new Object();
		ret->setInt(a->intValue() + b->intValue());
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