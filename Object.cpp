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
		cons->car = va_arg(ap, Object*);

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

ObjectList::ObjectList(const std::vector<Object*> &_value)
{
	type = Object::TypeList;

	int i = 0;
	Cons *prevCons = 0;
	while (i < _value.size()) {
		Cons *cons = new Cons;
		cons->car = _value[i];

		if (prevCons) {
			prevCons->cdr = cons;
		}
		else {
			value = cons;
		}
		prevCons = cons;
		i++;
	}

	prevCons->cdr = 0;
}
