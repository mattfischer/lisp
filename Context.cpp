#include "Context.hpp"
#include "Error.hpp"
#include "IO.hpp"
#include "NativeFunctions.hpp"

#include <cstdarg>
#include <sstream>

Context::Context()
{
	std::map<std::string, Object*> variables;

	Object *object;

	object = new Object();
	object->setNone();
	variables["nil"] = object;
	mNil = object;

	object = new Object();
	object->setT();
	variables["t"] = object;
	mT = object;
	
	std::map<std::string, Object*> functions = NativeFunctions::functions();
	variables.insert(functions.begin(), functions.end());

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

Object *Context::evalSpecialForm(Object *object, Scope *scope, bool &handled)
{
	const std::string &name = car(object)->stringValue();
	handled = true;

	if (name == "set!") {
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

		std::vector<std::string> variables;
		Object *varCons = car(varsCons);
		checkType(varCons, Object::TypeCons);
		for (; varCons != nil(); varCons = cdr(varCons)) {
			checkType(car(varCons), Object::TypeAtom);
			variables.push_back(car(varCons)->stringValue());
		}
		Object *ret = new Object;
		ret->setLambda(std::move(variables), cdr(varsCons));
		return ret;
	}

	handled = false;
	return 0;
}

Object *Context::evalCons(Object *object, Scope *scope)
{
	Object *function = car(object);
	Object *args = cdr(object);

	if (function->type() == Object::TypeAtom) {
		bool handled;
		Object *ret = evalSpecialForm(object, scope, handled);
		if (handled) {
			return ret;
		}
	}

	function = eval(function, scope);
	if (function->type() == Object::TypeLambda) {
		std::map<std::string, Object*> vars;
		Object *argCons = args;
		for (int i = 0; i < function->lambdaValue().variables.size(); i++) {
			checkType(argCons, Object::Type::TypeCons);
			vars[function->lambdaValue().variables[i]] = eval(car(argCons));
			argCons = cdr(argCons);
		}

		if (argCons != nil()) {
			std::stringstream ss;
			ss << "Incorrect number of arguments to function " << function << ": " << args;
			throw Error(ss.str());
		}

		Scope *newScope = new Scope(scope, std::move(vars));
		Object *ret = 0;
		for (Object *cons = function->lambdaValue().body; cons != nil(); cons = cdr(cons)) {
			ret = eval(car(cons), newScope);
		}
		return ret;
	}
	else if (function->type() == Object::TypeNativeFunction)
	{
		return function->nativeFunctionValue()(args);
	}
	else {
		std::stringstream ss;
		ss << "Object " << function << " must be a valid function.";
		throw Error(ss.str());
	}

	return 0;
}

Object *Context::car(Object *object)
{
	return object->consValue().car;
}

Object *Context::cdr(Object *object)
{
	return object->consValue().cdr;
}