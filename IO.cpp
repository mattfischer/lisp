#include "IO.hpp"

#include "Error.hpp"

#include <cctype>
#include <sstream>

static void eatWhitespace(std::istream &i)
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

Object *IO::read(std::istream &i, ObjectPool *pool)
{
	Object *object;

	eatWhitespace(i);

	if (i.fail() || i.eof()) {
		return pool->newNone();
	}

	char c = i.get();

	if (c == '\'') {
		object = new Object();
		Object *car = pool->newAtom("quote");
		Object *cdr = pool->newCons(read(i, pool), pool->newNone());
		object = pool->newCons(car, cdr);
	}
	else if (std::isdigit(c)) {
		int value;
		i.unget();
		i >> value;
		object = pool->newInt(value);
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
		object = pool->newString(value.c_str());
	}
	else if (c == '(' || c == '[')
	{
		char end = (c == '(') ? ')' : ']';

		Object *nil = pool->newNone();
		Object *prev = nil;
		object = nil;
		while (true) {
			eatWhitespace(i);
			c = i.get();
			if (c == end) {
				break;
			}
			else if (c == ')' || c == ']') {
				std::stringstream ss;
				ss << "Invalid character '" << c << "' in input";
				throw Error(ss.str());
			}
			i.unget();
			Object *item = read(i, pool);
			Object *newCons = pool->newCons(item, nil);
			if (prev == nil) {
				object = newCons;
			}
			else {
				prev->setCons(prev->consValue().car, newCons);
			}
			prev = newCons;
		}
	}
	else {
		std::string value;
		i.unget();
		while (!i.fail() && !i.eof()) {
			c = i.get();
			if (c == ')' || std::isspace(c) || i.fail() || i.eof()) {
				i.unget();
				break;
			}
			value.push_back(c);
		}

		if (value == "#t") {
			object = pool->newBool(true);
		}
		else if (value == "#f") {
			object = pool->newBool(false);
		}
		else {
			object = pool->newAtom(value);
		}
	}

	return object;
}

void IO::print(std::ostream &o, const Object *object)
{
	switch (object->type()) {
	case Object::TypeNone:
		o << "()";
		break;

	case Object::TypeBool:
		if (object->boolValue()) {
			o << "#t";
		}
		else {
			o << "#f";
		}
		break;

	case Object::TypeInt:
		o << object->intValue();
		break;

	case Object::TypeString:
		o << "\"" << object->stringValue() << "\"";
		break;

	case Object::TypeAtom:
		o << object->stringValue();
		break;

	case Object::TypeCons:
	{
		const Object *cons = object;
		o << "(";
		while (cons->type() != Object::TypeNone) {
			print(o, cons->consValue().car);
			cons = cons->consValue().cdr;
			if (cons->type() != Object::TypeNone) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}
}

std::ostream &operator<<(std::ostream &o, const Object *object)
{
	IO::print(o, object);
	return o;
}

std::ostream &operator<<(std::ostream &o, Object::Type type)
{
	switch (type) {
	case Object::TypeNone:
		o << "nil";
		break;

	case Object::TypeBool:
		o << "bool";
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
