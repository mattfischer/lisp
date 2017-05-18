#include "Read.hpp"

#include <cctype>

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

Object *read(std::istream &i)
{
	Object *object = 0;

	eatWhitespace(i);

	if (i.fail() || i.eof()) {
		return object;
	}

	char c = i.get();

	if (std::isdigit(c)) {
		int value;
		i.unget();
		i >> value;
		object = new Object();
		object->setInt(value);
	}
	else if (std::isalpha(c)) {
		std::string value;
		i.unget();
		while (!i.fail() && !i.eof()) {
			c = i.get();
			if (!std::isalpha(c)) {
				i.unget();
				break;
			}
			value.push_back(c);
		}

		if (value == "nil") {
			object = 0;
		}
		else {
			object = new Object();
			object->setAtom(value.c_str());
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
		object = new Object();
		object->setString(value.c_str());
	}
	else if (c == '(')
	{
		Object *prev = 0;
		while (true) {
			eatWhitespace(i);
			c = i.get();
			if (c == ')') {
				break;
			}
			i.unget();
			Object *car = read(i);
			Object *cons = new Object();
			cons->setCons(car, 0);
			if (prev) {
				prev->setCons(prev->carValue(), cons);
			} else {
				object = cons;
			} 
			prev = cons;
		}
	}

	return object;
}