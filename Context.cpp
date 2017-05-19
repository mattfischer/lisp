#include "Context.hpp"
#include "Error.hpp"

#include <cctype>
#include <cstdarg>
#include <sstream>

Context::Context()
{
	mParent = 0;
}

Context::Context(Context *parent, std::map<std::string, Object*> &&variables)
	: mParent(parent),
	mVariables(std::move(variables))
{
}

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

Object *Context::read(std::istream &i)
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
			}
			else {
				object = cons;
			}
			prev = cons;
		}
	}
	else {
		std::string value;
		i.unget();
		while (!i.fail() && !i.eof()) {
			c = i.get();
			if (c == ')' || std::isspace(c)) {
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

	return object;
}

void Context::print(std::ostream &o, const Object *object)
{
	if (!object) {
		o << "nil";
		return;
	}

	switch (object->type()) {
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
		while (cons) {
			print(o, cons->carValue());
			cons = cons->cdrValue();
			if (cons) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}
}

Object *Context::eval(Object *object)
{
	if (!object) {
		return object;
	}

	switch (object->type()) {
	case Object::TypeInt:
	case Object::TypeString:
		return object;

	case Object::TypeAtom:
		return getVariable(object->stringValue());

	case Object::TypeCons:
		return evalCons(object);

	default:
		return 0;
	}
}

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

void Context::checkType(Object *object, Object::Type type)
{
	Object::Type objectType;

	if (object) {
		objectType = object->type();
	}
	else {
		objectType = Object::TypeNone;
	}

	if (objectType != type) {
		std::stringstream ss;
		ss << "Type mismatch on " << object << " (expected " << type << " got " << objectType << ")";
		throw Error(ss.str());
	}
}

void Context::evalArgs(Object *object, int length, ...)
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

Object *Context::evalCons(Object *object)
{
	Object *car = object->carValue();

	if (car->type() == Object::TypeCons) {
		return evalLambda(car, object->cdrValue());
	}

	checkType(car, Object::TypeAtom);

	std::string name(car->stringValue());

	if (name == "+") {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() + b->intValue());
		return ret;
	}
	else if (name == "-") {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() - b->intValue());
		return ret;
	}
	else if (name == "*") {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() * b->intValue());
		return ret;
	}
	else if (name == "/") {
		Object *a, *b;
		evalArgs(object, 2, &a, &b);
		checkType(a, Object::TypeInt);
		checkType(b, Object::TypeInt);

		Object *ret = new Object();
		ret->setInt(a->intValue() / b->intValue());
		return ret;
	}
	else if (name == "let") {
		Object *cons = object->cdrValue();
		checkType(cons, Object::TypeCons);

		Object *varsCons = cons->carValue();
		checkType(varsCons, Object::TypeCons);
		std::map<std::string, Object*> vars;
		for (; varsCons; varsCons = varsCons->cdrValue()) {
			Object *varCons = varsCons->carValue();
			checkType(varCons, Object::TypeCons);

			checkType(varCons->carValue(), Object::TypeAtom);
			checkType(varCons->cdrValue(), Object::TypeCons);
			vars[varCons->carValue()->stringValue()] = eval(varCons->cdrValue()->carValue());
		}
		Context context(this, std::move(vars));
		Object *ret = 0;
		for (Object *item = cons->cdrValue(); item; item = item->cdrValue()) {
			ret = context.eval(item->carValue());
		}
		return ret;
	}
	else if (name == "setq") {
		Object *cons = object->cdrValue();
		checkType(cons, Object::TypeCons);
		checkType(cons->cdrValue(), Object::TypeCons);

		Object *name = cons->carValue();
		checkType(name, Object::TypeAtom);

		Object *value = eval(cons->cdrValue()->carValue());

		setVariable(name->stringValue(), value);
		return value;
	}

	std::stringstream ss;
	ss << "No function " << car->stringValue() << " defined";
	throw Error(ss.str());
}

Object *Context::evalLambda(Object *function, Object *args)
{
	checkType(function->carValue(), Object::Type::TypeAtom);
	if (std::string(function->carValue()->stringValue()) != "lambda") {
		std::stringstream ss;
		ss << "Function call requires lambda expression, got " << function;
		throw Error(ss.str());
	}

	Object *varsCons = function->cdrValue();
	std::map<std::string, Object*> vars;
	Object *varCons = varsCons->carValue();
	checkType(varCons, Object::TypeCons);
	Object *argCons = args;
	for (; varCons && argCons; varCons = varCons->cdrValue(), argCons = argCons->cdrValue()) {
		checkType(varCons->carValue(), Object::TypeAtom);
		vars[varCons->carValue()->stringValue()] = eval(argCons->carValue());
	}

	if (varCons || argCons) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << function << ": " << args;
		throw Error(ss.str());
	}

	Context context(this, std::move(vars));
	Object *ret = 0;
	for (Object *cons = varsCons->cdrValue(); cons; cons = cons->cdrValue()) {
		ret = context.eval(cons->carValue());
	}

	return ret;
}

Object *Context::getVariable(const std::string &name)
{
	std::map<std::string, Object*>::iterator it = mVariables.find(name);
	if (it == mVariables.end()) {
		if (mParent) {
			return mParent->getVariable(name);
		}
		else {
			std::stringstream ss;
			ss << "No symbol " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		return it->second;
	}
}

void Context::setVariable(const std::string &name, Object *value)
{
	std::map<std::string, Object*>::iterator it = mVariables.find(name);
	if (it == mVariables.end()) {
		if (mParent) {
			mParent->setVariable(name, value);
		}
		else {
			std::stringstream ss;
			ss << "No symbol " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		it->second = value;
	}
}

std::ostream &operator<<(std::ostream &o, const Object *object)
{
	Context::print(o, object);
	return o;
}
