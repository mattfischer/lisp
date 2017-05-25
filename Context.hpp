#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Datum.hpp"
#include "Scope.hpp"
#include "DatumPool.hpp"

class Context {
public:
	Context();

	Datum *eval(Datum *datum);

	DatumPool *pool();

private:
	Datum *eval(Datum *datum, Scope *scope);
	Datum *evalCons(Datum *datum, Scope *scope);
	bool evalSpecialForm(Datum *datum, Scope *scope, Datum *&ret);
	Datum *evalLambda(Datum *lambda, Datum *args, Scope *scope);

	void checkType(Datum *datum, Datum::Type type);

	Datum *car(Datum *datum);
	Datum *cdr(Datum *datum);

	Scope *mRootScope;
	DatumPool *mPool;
};

#endif