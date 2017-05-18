#include "Object.hpp"

#include <cctype>
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

void eatWhitespace(std::istream &i)
{
	while (!i.fail() && !i.eof())
	{
		char c = i.get();
		if (!std::isspace(c))
		{
			i.unget();
			break;
		}
	}
}

std::istream &operator >> (std::istream &i, Object *&object)
{
	object = 0;

	eatWhitespace(i);

	if (i.fail() || i.eof()) {
		return i;
	}

	char c = i.get();

	if (std::isdigit(c)) {
		int value;
		i.unget();
		i >> value;
		object = new ObjectInt(value);
	}
	else if (std::isalpha(c)) {
		std::string value;
		i.unget();
		while (!i.fail() && !i.eof()) {
			c = i.get();
			if(!std::isalpha(c)) {
				i.unget();
				break;
			}
			value.push_back(c);
		}

		if (value == "nil") {
			object = 0;
		}
		else {
			object = new ObjectAtom(value);
		}
	}
	else if (c == '\"')
	{
		std::string value;
		while (!i.fail() && !i.eof()) {
			c = i.get();
			if (c == '\"') {
				break;
			}
			value.push_back(c);
		}
		object = new ObjectString(value);
	}
	else if (c == '(')
	{
		std::vector<Object*> objects;
		while (true) {
			eatWhitespace(i);
			c = i.get();
			if (c == ')') {
				break;
			}
			i.unget();
			Object *object;
			i >> object;
			objects.push_back(object);
		}

		object = new ObjectList(objects);
	}

	return i;
}