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
	{
		Object *ret;
		if (evalSpecialForm(object, scope, ret)) {
			return ret;
		}

		Object *evals = evalCons(object, scope);

		Object *function = car(evals);
		Object *args = cdr(evals);
		if (function->type() == Object::TypeLambda) {
			return evalLambda(function, args, scope);
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
	}

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

bool Context::evalSpecialForm(Object *object, Scope *scope, Object *&ret)
{
	const std::string &name = car(object)->stringValue();

	if (name == "set!" || name == "define") {
		bool isDefine = (name == "define");
		Object *cons = cdr(object);
		ret = nil();
		for(Object *cons = cdr(object); cons->type() == Object::TypeCons; cons = cdr(cdr(cons))) {
			Object *name = car(cons);
			checkType(name, Object::TypeAtom);

			checkType(cdr(cons), Object::TypeCons);
			Object *value = car(cdr(cons));
			ret = eval(value);

			scope->set(name->stringValue(), ret, isDefine);
		}
		return true;
	}
	else if (name == "quote") {
		checkType(cdr(object), Object::TypeCons);
		ret = car(cdr(object));
		return true;
	}
	else if (name == "lambda") {
		checkType(cdr(object), Object::TypeCons);
		Object *vars = car(cdr(object));

		std::vector<std::string> variables;
		for (Object *var = vars; var->type() == Object::TypeCons; var = cdr(var)) {
			checkType(car(var), Object::TypeAtom);
			variables.push_back(car(var)->stringValue());
		}
		ret = new Object;
		ret->setLambda(std::move(variables), cdr(cdr(object)));
		return true;
	}
	
	return false;
}

Object *Context::evalCons(Object *object, Scope *scope)
{
	Object *ret = nil();
	Object *prev = nil();
	for (Object *cons = object; cons != nil(); cons = cdr(cons)) {
		Object *item = eval(car(cons), scope);
		Object *newCons = new Object();
		newCons->setCons(item, nil());
		if (prev == nil()) {
			ret = newCons;
		}
		else {
			prev->setCons(car(prev), newCons);
		}
		prev = newCons;
	}

	return ret;
}

Object *Context::evalLambda(Object *lambda, Object *args, Scope *scope)
{
	std::map<std::string, Object*> vars;
	Object *arg = args;
	for (int i = 0; i < lambda->lambdaValue().variables.size(); i++) {
		if (arg == nil()) {
			break;
		}
		checkType(arg, Object::Type::TypeCons);
		vars[lambda->lambdaValue().variables[i]] = car(arg);
		arg = cdr(arg);
	}

	if (arg != nil() || vars.size() < lambda->lambdaValue().variables.size()) {
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