#include "Context.hpp"
#include "Error.hpp"
#include "IO.hpp"

#include <cstdarg>
#include <sstream>

Context::Context()
{
	std::map<std::string, Object*> variables;

	mNil = new Object();
	mNil->setNone();
	variables["nil"] = mNil;
	mT = new Object();
	mT->setT();
	variables["t"] = mT;

	mRootScope = new Scope(0, std::move(variables));
}

Object *Context::eval(Object *object)
{
	return eval(object, mRootScope);
}

Object *Context::nil()
{
	return mNil;
}

Object *Context::t()
{
	return mT;
}

Object *Context::eval(Object *object, Scope *scope)
{
	switch (object->type()) {
	case Object::TypeNone:
	case Object::TypeInt:
	case Object::TypeString:
		return object;

	case Object::TypeAtom:
		return scope->get(object->stringValue());

	case Object::TypeCons:
		return evalCons(object, scope);

	default:
		return 0;
	}
}

void Context::checkType(Object *object, Object::Type type)
{
	if (object->type() != type) {
		std::stringstream ss;
		ss << "Type mismatch on " << object << " (expected " << type << " got " << object->type() << ")";
		throw Error(ss.str());
	}
}

void Context::evalArgs(Object *object, Scope *scope, int length, ...)
{
	va_list ap;
	va_start(ap, length);
	int objectLength = 0;
	for (Object *cons = cdr(object); cons != nil(); cons = cdr(cons)) {
		if (objectLength < length) {
			Object **arg = va_arg(ap, Object**);
			*arg = eval(car(cons), scope);
		}
		objectLength++;
	}

	if (objectLength != length) {
		std::stringstream ss;
		ss << "List " << object << " is of incorrect length (expected " << length + 1 << " got " << objectLength + 1 << ")";
		throw Error(ss.str());
	}
}

Object *Context::evalCons(Object *object, Scope *scope)
{
	Object *head = car(object);

	if (head->type() == Object::TypeAtom) {
		std::string name(head->stringValue());

		if (name == "+") {
			Object *a, *b;
			evalArgs(object, scope, 2, &a, &b);
			checkType(a, Object::TypeInt);
			checkType(b, Object::TypeInt);

			Object *ret = new Object();
			ret->setInt(a->intValue() + b->intValue());
			return ret;
		}
		else if (name == "-") {
			Object *a, *b;
			evalArgs(object, scope, 2, &a, &b);
			checkType(a, Object::TypeInt);
			checkType(b, Object::TypeInt);

			Object *ret = new Object();
			ret->setInt(a->intValue() - b->intValue());
			return ret;
		}
		else if (name == "*") {
			Object *a, *b;
			evalArgs(object, scope, 2, &a, &b);
			checkType(a, Object::TypeInt);
			checkType(b, Object::TypeInt);

			Object *ret = new Object();
			ret->setInt(a->intValue() * b->intValue());
			return ret;
		}
		else if (name == "/") {
			Object *a, *b;
			evalArgs(object, scope, 2, &a, &b);
			checkType(a, Object::TypeInt);
			checkType(b, Object::TypeInt);

			Object *ret = new Object();
			ret->setInt(a->intValue() / b->intValue());
			return ret;
		}
		else if (name == "set!") {
			Object *cons = cdr(object);
			Object *ret = nil();
			while (cons != nil()) {
				checkType(cons, Object::TypeCons);

				Object *name = car(cons);
				checkType(name, Object::TypeAtom);

				cons = cdr(cons);
				checkType(cons, Object::TypeCons);
				ret = eval(car(cons));

				scope->set(name->stringValue(), ret);

				cons = cdr(cons);
			}
			return ret;
		}
		else if (name == "quote") {
			checkType(cdr(object), Object::TypeCons);
			return car(cdr(object));
		}
		else if (name == "lambda") {
			Object *varsCons = cdr(object);
			checkType(varsCons, Object::TypeCons);

			std::vector<std::string> varVector;
			Object *varCons = car(varsCons);
			checkType(varCons, Object::TypeCons);
			for (; varCons != nil(); varCons = cdr(varCons)) {
				checkType(car(varCons), Object::TypeAtom);
				varVector.push_back(car(varCons)->stringValue());
			}
			char **vars = new char*[varVector.size()];
			for (int i = 0; i < varVector.size(); i++) {
				vars[i] = strdup(varVector[i].c_str());
			}
			Object *ret = new Object;
			ret->setLambda(varVector.size(), vars, cdr(varsCons));
			return ret;
		}
	}

	Object *lambda = eval(head, scope);
	Object *args = cdr(object);
	checkType(lambda, Object::Type::TypeLambda);

	std::map<std::string, Object*> vars;
	Object *argCons = args;
	for(int i=0; i<lambda->lambdaValue().numVariables; i++) {
		checkType(argCons, Object::Type::TypeCons);
		vars[lambda->lambdaValue().variables[i]] = eval(car(argCons));
		argCons = cdr(argCons);
	}

	if (argCons != nil()) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << lambda << ": " << args;
		throw Error(ss.str());
	}

	Scope *newScope = new Scope(scope, std::move(vars));
	Object *ret = 0;
	for (Object *cons = lambda->lambdaValue().body; cons != nil(); cons = cdr(cons)) {
		ret = eval(car(cons), newScope);
	}

	return ret;
}

Object *Context::car(Object *object)
{
	return object->consValue().car;
}

Object *Context::cdr(Object *object)
{
	return object->consValue().cdr;
}