#include "Context.hpp"
#include "Error.hpp"
#include "IO.hpp"
#include "NativeFunctions.hpp"
#include "Syntax.hpp"

#include <cstdarg>
#include <sstream>

const char *syntaxes[] = {
	"(define-syntax begin (syntax-rules () [(_ x ...) ((lambda () x ...))] ))",
	"(define-syntax let (syntax-rules () [(_ ((x y) ...) z ...) ((lambda (x ...) z ...) y ...)] ))"
};

Context::Context()
{
	mPool = new ObjectPool;

	std::map<std::string, Object*> functions = NativeFunctions::functions(mPool);

	mRootScope = new Scope(0, std::move(functions));

	for (const char *syntax : syntaxes) {
		std::stringstream ss;
		ss << syntax;
		Object *object = IO::read(ss, mPool);
		eval(object);
	}
}

Object *Context::eval(Object *object)
{
	return eval(object, mRootScope);
}

ObjectPool *Context::pool()
{
	return mPool;
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
			return function->nativeFunctionValue()(args, mPool);
		}
		else {
			std::stringstream ss;
			ss << "Object " << function << " must be a valid function.";
			throw Error(ss.str());
		}
	}

	default:
		return mPool->newNone();
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
		ret = mPool->newNone();
		for (Object *cons = cdr(object); cons->type() == Object::TypeCons; cons = cdr(cdr(cons))) {
			Object *name = car(cons);
			checkType(name, Object::TypeAtom);

			Object *value = car(cdr(cons));
			ret = eval(value);

			scope->set(name->stringValue(), ret, isDefine);
		}
		return true;
	}
	else if (name == "quote") {
		ret = car(cdr(object));
		return true;
	}
	else if (name == "lambda") {
		Object *vars = car(cdr(object));

		std::vector<std::string> variables;
		for (Object *var = vars; var->type() == Object::TypeCons; var = cdr(var)) {
			checkType(car(var), Object::TypeAtom);
			variables.push_back(car(var)->stringValue());
		}
		ret = mPool->newLambda(std::move(variables), cdr(cdr(object)));
		return true;
	}
	else if (name == "if") {
		Object *test = cdr(object);
		Object *consequent = cdr(test);
		Object *alternative = cdr(consequent);

		Object *result = eval(car(test));
		if (result->type() != Object::TypeBool || result->boolValue()) {
			ret = eval(car(consequent));
		}
		else {
			ret = eval(car(alternative));
		}
		return true;
	}
	else if (name == "define-syntax") {
		checkType(car(cdr(object)), Object::TypeAtom);
		std::string keyword = car(cdr(object))->stringValue();

		Object *syntaxRules = car(cdr(cdr(object)));
		checkType(car(syntaxRules), Object::TypeAtom);
		if (car(syntaxRules)->stringValue() != "syntax-rules") {
			std::stringstream ss;
			ss << "Invalid expression " << syntaxRules << " in define-syntax";
			throw Error(ss.str());
		}

		checkType(cdr(syntaxRules), Object::TypeCons);
		Object *lits = car(cdr(syntaxRules));
		std::vector<std::string> literals;
		for (Object *cons = lits; cons->type() == Object::TypeCons; cons = cdr(cons)) {
			Object *lit = car(cons);
			checkType(lit, Object::TypeAtom);
			literals.push_back(lit->stringValue());
		}

		std::vector<Syntax::Rule> rules;
		for (Object *cons = cdr(cdr(syntaxRules)); cons->type() == Object::TypeCons; cons = cdr(cons)) {
			Object *rule = car(cons);
			Object *pattern = car(rule);
			checkType(pattern, Object::TypeCons);

			Object *templ = car(cdr(rule));
			checkType(templ, Object::TypeCons);

			Syntax::Rule newRule;
			newRule.pattern = pattern;
			newRule.templ = templ;
			rules.push_back(newRule);
		}
		Syntax *syntax = new Syntax(std::move(rules));
		scope->setSyntax(keyword, syntax, true);
		ret = mPool->newNone();
		return true;
	}
	else if (scope->containsSyntax(name)) {
		Syntax *syntax = scope->getSyntax(name);
		if (syntax->transform(object, ret, mPool)) {
			ret = eval(ret, scope);
			return true;
		}
	}
	
	return false;
}

Object *Context::evalCons(Object *object, Scope *scope)
{
	Object *nil = mPool->newNone();
	Object *ret = nil;
	Object *prev = nil;
	for (Object *cons = object; cons->type() == Object::TypeCons; cons = cdr(cons)) {
		Object *item = eval(car(cons), scope);
		Object *newCons = mPool->newCons(item, nil);
		if (prev == nil) {
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
		if (arg->type() != Object::TypeCons) {
			break;
		}
		vars[lambda->lambdaValue().variables[i]] = car(arg);
		arg = cdr(arg);
	}

	if (arg->type() != Object::TypeNone || vars.size() < lambda->lambdaValue().variables.size()) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << lambda << ": " << args;
		throw Error(ss.str());
	}

	Scope *newScope = new Scope(scope, std::move(vars));
	Object *ret = 0;
	for (Object *cons = lambda->lambdaValue().body; cons->type() == Object::TypeCons; cons = cdr(cons)) {
		ret = eval(car(cons), newScope);
	}
	return ret;

}
Object *Context::car(Object *object)
{
	checkType(object, Object::TypeCons);
	return object->consValue().car;
}

Object *Context::cdr(Object *object)
{
	checkType(object, Object::TypeCons);
	return object->consValue().cdr;
}