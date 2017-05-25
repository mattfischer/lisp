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

Datum *IO::read(std::istream &i, DatumPool *pool)
{
	Datum *datum;

	eatWhitespace(i);

	if (i.fail() || i.eof()) {
		return pool->newNone();
	}

	char c = i.get();

	if (c == '\'') {
		datum = new Datum();
		Datum *car = pool->newSymbol("quote");
		Datum *cdr = pool->newCons(read(i, pool), pool->newNone());
		datum = pool->newCons(car, cdr);
	}
	else if (std::isdigit(c)) {
		int value;
		i.unget();
		i >> value;
		datum = pool->newInt(value);
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
		datum = pool->newString(value.c_str());
	}
	else if (c == '(' || c == '[')
	{
		char end = (c == '(') ? ')' : ']';

		Datum *nil = pool->newNone();
		Datum *prev = nil;
		datum = nil;
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
			Datum *item = read(i, pool);
			Datum *newCons = pool->newCons(item, nil);
			if (prev == nil) {
				datum = newCons;
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
			datum = pool->newBool(true);
		}
		else if (value == "#f") {
			datum = pool->newBool(false);
		}
		else {
			datum = pool->newSymbol(value);
		}
	}

	return datum;
}

void IO::print(std::ostream &o, const Datum *datum)
{
	switch (datum->type()) {
	case Datum::TypeNone:
		o << "()";
		break;

	case Datum::TypeBool:
		if (datum->boolValue()) {
			o << "#t";
		}
		else {
			o << "#f";
		}
		break;

	case Datum::TypeInt:
		o << datum->intValue();
		break;

	case Datum::TypeString:
		o << "\"" << datum->stringValue() << "\"";
		break;

	case Datum::TypeSymbol:
		o << datum->stringValue();
		break;

	case Datum::TypeCons:
	{
		const Datum *cons = datum;
		o << "(";
		while (cons->type() != Datum::TypeNone) {
			print(o, cons->consValue().car);
			cons = cons->consValue().cdr;
			if (cons->type() != Datum::TypeNone) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}
}

std::ostream &operator<<(std::ostream &o, const Datum *datum)
{
	IO::print(o, datum);
	return o;
}

std::ostream &operator<<(std::ostream &o, Datum::Type type)
{
	switch (type) {
	case Datum::TypeNone:
		o << "nil";
		break;

	case Datum::TypeBool:
		o << "bool";
		break;

	case Datum::TypeInt:
		o << "int";
		break;

	case Datum::TypeString:
		o << "string";
		break;

	case Datum::TypeSymbol:
		o << "symbol";
		break;

	case Datum::TypeCons:
		o << "cons";
		break;
	}

	return o;
}
