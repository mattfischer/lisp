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
	for (Object *cons = object->cdrValue(); cons != nil(); cons = cons->cdrValue()) {
		if (objectLength < length) {
			Object **arg = va_arg(ap, Object**);
			*arg = eval(cons->carValue(), scope);
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
	Object *car = object->carValue();

	if (car->type() == Object::TypeAtom) {

		std::string name(car->stringValue());

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
			Scope *newScope = new Scope(scope, std::move(vars));
			Object *ret = 0;
			for (Object *item = cons->cdrValue(); item != nil(); item = item->cdrValue()) {
				ret = eval(item->carValue(), newScope);
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

				scope->set(name->stringValue(), ret);

				cons = cons->cdrValue();
			}
			return ret;
		}
		else if (name == "quote") {
			checkType(object->cdrValue(), Object::TypeCons);
			return object->cdrValue()->carValue();
		}
		else if (name == "lambda") {
			Object *varsCons = object->cdrValue();
			checkType(varsCons, Object::TypeCons);

			std::vector<std::string> varVector;
			Object *varCons = varsCons->carValue();
			checkType(varCons, Object::TypeCons);
			for (; varCons != nil(); varCons = varCons->cdrValue()) {
				checkType(varCons->carValue(), Object::TypeAtom);
				varVector.push_back(varCons->carValue()->stringValue());
			}
			char **vars = new char*[varVector.size()];
			for (int i = 0; i < varVector.size(); i++) {
				vars[i] = strdup(varVector[i].c_str());
			}
			Object *ret = new Object;
			ret->setLambda(varVector.size(), vars, varsCons->cdrValue());
			return ret;
		}
	}

	Object *lambda = eval(car, scope);
	Object *args = object->cdrValue();
	checkType(lambda, Object::Type::TypeLambda);

	std::map<std::string, Object*> vars;
	Object *argCons = args;
	for(int i=0; i<lambda->numVariables(); i++) {
		checkType(argCons, Object::Type::TypeCons);
		vars[lambda->variables()[i]] = eval(argCons->carValue());
		argCons = argCons->cdrValue();
	}

	if (argCons != nil()) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << lambda << ": " << args;
		throw Error(ss.str());
	}

	Scope *newScope = new Scope(scope, std::move(vars));
	Object *ret = 0;
	for (Object *cons = lambda->lambdaBody(); cons != nil(); cons = cons->cdrValue()) {
		ret = eval(cons->carValue(), newScope);
	}

	return ret;
}
