#include "Object.hpp"

#include <stdarg.h>

ObjectInt::ObjectInt(int _value)
{
	type = Object::TypeInt;
	value = _value;
}

ObjectString::ObjectString(const std::string &_value)
{
	type = Object::TypeString;
	value = _value;
}

ObjectAtom::ObjectAtom(const std::string &_value)
{
	type = Object::TypeAtom;
	value = _value;
}

ObjectList::ObjectList(int length, ...)
{
	type = Object::TypeList;

	va_list ap;
	va_start(ap, length);
	int i = 0;
	Cons *prevCons = 0;
	while (i < length) {
		Cons *cons = new Cons;
		cons->car = va_arg(ap, Object*);;

		if (prevCons) {
			prevCons->cdr = cons;
		}
		else {
			value = cons;
		}
		prevCons = cons;
		i++;
	}

	va_end(ap);
	prevCons->cdr = 0;
}

std::ostream &operator<<(std::ostream &o, const Object *object)
{
	if (!object) {
		o << "nil";
		return o;
	}

	switch (object->type) {
	case Object::TypeInt:
		o << ((ObjectInt*)object)->value;
		break;

	case Object::TypeString:
		o << "\"" << ((ObjectString*)object)->value << "\"";
		break;

	case Object::TypeAtom:
		o << ((ObjectAtom*)object)->value;
		break;

	case Object::TypeList:
	{
		o << "(";
		Cons *cons = ((ObjectList*)object)->value;
		while (cons) {
			o << cons->car;
			cons = cons->cdr;
			if (cons) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}

	return o;
}

