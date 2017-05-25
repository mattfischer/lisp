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
	mPool = new DatumPool;

	std::map<std::string, Datum*> functions = NativeFunctions::functions(mPool);

	mRootScope = new Scope(0, std::move(functions));

	for (const char *syntax : syntaxes) {
		std::stringstream ss;
		ss << syntax;
		Datum *datum = IO::read(ss, mPool);
		eval(datum);
	}
}

Datum *Context::eval(Datum *datum)
{
	return eval(datum, mRootScope);
}

DatumPool *Context::pool()
{
	return mPool;
}

Datum *Context::eval(Datum *datum, Scope *scope)
{
	switch (datum->type()) {
	case Datum::TypeNone:
	case Datum::TypeInt:
	case Datum::TypeString:
		return datum;

	case Datum::TypeSymbol:
		return scope->get(datum->stringValue());

	case Datum::TypeCons:
	{
		Datum *ret;
		if (evalSpecialForm(datum, scope, ret)) {
			return ret;
		}

		Datum *evals = evalCons(datum, scope);

		Datum *function = car(evals);
		Datum *args = cdr(evals);
		if (function->type() == Datum::TypeLambda) {
			return evalLambda(function, args, scope);
		}
		else if (function->type() == Datum::TypeNativeFunction)
		{
			return function->nativeFunctionValue()(args, mPool);
		}
		else {
			std::stringstream ss;
			ss << "Datum " << function << " must be a valid function.";
			throw Error(ss.str());
		}
	}

	default:
		return mPool->newNone();
	}
}

void Context::checkType(Datum *datum, Datum::Type type)
{
	if (datum->type() != type) {
		std::stringstream ss;
		ss << "Type mismatch on " << datum << " (expected " << type << " got " << datum->type() << ")";
		throw Error(ss.str());
	}
}

bool Context::evalSpecialForm(Datum *datum, Scope *scope, Datum *&ret)
{
	const std::string &name = car(datum)->stringValue();

	if (name == "set!" || name == "define") {
		bool isDefine = (name == "define");
		Datum *cons = cdr(datum);
		ret = mPool->newNone();
		for (Datum *cons = cdr(datum); cons->type() == Datum::TypeCons; cons = cdr(cdr(cons))) {
			Datum *name = car(cons);
			checkType(name, Datum::TypeSymbol);

			Datum *value = car(cdr(cons));
			ret = eval(value);

			scope->set(name->stringValue(), ret, isDefine);
		}
		return true;
	}
	else if (name == "quote") {
		ret = car(cdr(datum));
		return true;
	}
	else if (name == "lambda") {
		Datum *vars = car(cdr(datum));

		std::vector<std::string> variables;
		for (Datum *var = vars; var->type() == Datum::TypeCons; var = cdr(var)) {
			checkType(car(var), Datum::TypeSymbol);
			variables.push_back(car(var)->stringValue());
		}
		ret = mPool->newLambda(std::move(variables), cdr(cdr(datum)));
		return true;
	}
	else if (name == "if") {
		Datum *test = cdr(datum);
		Datum *consequent = cdr(test);
		Datum *alternative = cdr(consequent);

		Datum *result = eval(car(test));
		if (result->type() != Datum::TypeBool || result->boolValue()) {
			ret = eval(car(consequent));
		}
		else {
			ret = eval(car(alternative));
		}
		return true;
	}
	else if (name == "define-syntax") {
		checkType(car(cdr(datum)), Datum::TypeSymbol);
		std::string keyword = car(cdr(datum))->stringValue();

		Datum *syntaxRules = car(cdr(cdr(datum)));
		checkType(car(syntaxRules), Datum::TypeSymbol);
		if (car(syntaxRules)->stringValue() != "syntax-rules") {
			std::stringstream ss;
			ss << "Invalid expression " << syntaxRules << " in define-syntax";
			throw Error(ss.str());
		}

		checkType(cdr(syntaxRules), Datum::TypeCons);
		Datum *lits = car(cdr(syntaxRules));
		std::vector<std::string> literals;
		for (Datum *cons = lits; cons->type() == Datum::TypeCons; cons = cdr(cons)) {
			Datum *lit = car(cons);
			checkType(lit, Datum::TypeSymbol);
			literals.push_back(lit->stringValue());
		}

		std::vector<Syntax::Rule> rules;
		for (Datum *cons = cdr(cdr(syntaxRules)); cons->type() == Datum::TypeCons; cons = cdr(cons)) {
			Datum *rule = car(cons);
			Datum *pattern = car(rule);
			checkType(pattern, Datum::TypeCons);

			Datum *templ = car(cdr(rule));
			checkType(templ, Datum::TypeCons);

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
		if (syntax->transform(datum, ret, mPool)) {
			ret = eval(ret, scope);
			return true;
		}
	}
	
	return false;
}

Datum *Context::evalCons(Datum *datum, Scope *scope)
{
	Datum *nil = mPool->newNone();
	Datum *ret = nil;
	Datum *prev = nil;
	for (Datum *cons = datum; cons->type() == Datum::TypeCons; cons = cdr(cons)) {
		Datum *item = eval(car(cons), scope);
		Datum *newCons = mPool->newCons(item, nil);
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

Datum *Context::evalLambda(Datum *lambda, Datum *args, Scope *scope)
{
	std::map<std::string, Datum*> vars;
	Datum *arg = args;
	for (int i = 0; i < lambda->lambdaValue().variables.size(); i++) {
		if (arg->type() != Datum::TypeCons) {
			break;
		}
		vars[lambda->lambdaValue().variables[i]] = car(arg);
		arg = cdr(arg);
	}

	if (arg->type() != Datum::TypeNone || vars.size() < lambda->lambdaValue().variables.size()) {
		std::stringstream ss;
		ss << "Incorrect number of arguments to function " << lambda << ": " << args;
		throw Error(ss.str());
	}

	Scope *newScope = new Scope(scope, std::move(vars));
	Datum *ret = 0;
	for (Datum *cons = lambda->lambdaValue().body; cons->type() == Datum::TypeCons; cons = cdr(cons)) {
		ret = eval(car(cons), newScope);
	}
	return ret;

}
Datum *Context::car(Datum *datum)
{
	checkType(datum, Datum::TypeCons);
	return datum->consValue().car;
}

Datum *Context::cdr(Datum *datum)
{
	checkType(datum, Datum::TypeCons);
	return datum->consValue().cdr;
}