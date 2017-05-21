#include "Context.hpp"
#include "Error.hpp"

#include <cctype>
#include <cstdarg>
#include <sstream>

Context::Context()
{
	mParent = 0;
	mNil = new Object();
	mNil->setNone();
	mVariables["nil"] = mNil;
	mT = new Object();
	mT->setT();
	mVariables["t"] = mT;
}

Context::Context(Context *parent, std::map<std::string, Object*> &&variables)
	: mParent(parent),
	mVariables(std::move(variables))
{
	mNil = mParent->nil();
	mT = mParent->t();
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
	Object *object = nil();

	eatWhitespace(i);

	if (i.fail() || i.eof()) {
		return object;
	}

	char c = i.get();

	if (c == '\'') {
		object = new Object();
		Object *car = new Object();
		car->setAtom("quote");
		Object *cdr = new Object();
		cdr->setCons(read(i), nil());
		object->setCons(car, cdr);
	}
	else if (std::isdigit(c)) {
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
		Object *prev = nil();
		while (true) {
			eatWhitespace(i);
			c = i.get();
			if (c == ')') {
				break;
			}
			i.unget();
			Object *car = read(i);
			Object *cons = new Object();
			cons->setCons(car, nil());
			if (prev == nil()) {
				object = cons;
			}
			else {
				prev->setCons(prev->carValue(), cons);
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
			object = nil();
		}
		else if (value == "t") {
			object = t();
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
	switch (object->type()) {
	case Object::TypeNone:
		o << "nil";
		break;

	case Object::TypeT:
		o << "t";
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
			print(o, cons->carValue());
			cons = cons->cdrValue();
			if (cons->type() != Object::TypeNone) {
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
	switch (object->type()) {
	case Object::TypeNone:
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

Object *Context::nil()
{
	return mNil;
}

Object *Context::t()
{
	return mT;
}

std::ostream &operator<<(std::ostream &o, Object::Type type)
{
	switch (type) {
	case Object::TypeNone:
		o << "nil";
		break;

	case Object::TypeT:
		o << "t";
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
	if (object->type() != type) {
		std::stringstream ss;
		ss << "Type mismatch on " << object << " (expected " << type << " got " << object->type() << ")";
		throw Error(ss.str());
	}
}

void Context::evalArgs(Object *object, int length, ...)
{
	va_list ap;
	va_start(ap, length);
	int objectLength = 0;
	for (Object *cons = object->cdrValue(); cons != nil(); cons = cons->cdrValue()) {
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
		for (; varsCons != nil(); varsCons = varsCons->cdrValue()) {
			Object *varCons = varsCons->carValue();
			checkType(varCons, Object::TypeCons);

			checkType(varCons->carValue(), Object::TypeAtom);
			checkType(varCons->cdrValue(), Object::TypeCons);
			vars[varCons->carValue()->stringValue()] = eval(varCons->cdrValue()->carValue());
		}
		Context context(this, std::move(vars));
		Object *ret = 0;
		for (Object *item = cons->cdrValue(); item != nil(); item = item->cdrValue()) {
			ret = context.eval(item->carValue());
		}
		return ret;
	}
	else if (name == "set!") {
		Object *cons = object->cdrValue();
		Object *ret = nil();
		while (cons != nil()) {
			checkType(cons, Object::TypeCons);

			Object *name = cons->carValue();
			checkType(name, Object::TypeAtom);

			cons = cons->cdrValue();
			checkType(cons, Object::TypeCons);
			ret = eval(cons->carValue());

			setVariable(name->stringValue(), ret);

			cons = cons->cdrValue();
		}
		return ret;
	}
	else if (name == "quote") {
		checkType(object->cdrValue(), Object::TypeCons);
		return object->cdrValue()->carValue();
	}
	else {
		Object *lambda = getVariable(name);
		return evalLambda(lambda, object->cdrValue());
	}
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
	for (; varCons != nil() && argCons != nil(); varCons = varCons->cdrValue(), argCons = argCons->cdrValue()) {
		checkType(varCons->carValue(), Object::TypeAtom);
		vars[varCons->carValue()->stringValue()] = eval(argCons->carValue());
	}

	if (varCons != nil() || argCons != nil()) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << function << ": " << args;
		throw Error(ss.str());
	}

	Context context(this, std::move(vars));
	Object *ret = 0;
	for (Object *cons = varsCons->cdrValue(); cons != nil(); cons = cons->cdrValue()) {
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
