#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Datum.hpp"
#include "Scope.hpp"
#include "DatumPool.hpp"
#include "Continuation.hpp"

class Context {
public:
	Context();

	Datum *eval(Datum *datum);

	DatumPool *pool();

private:
	Datum *evalDatum(Datum *datum, Scope *scope, Continuation &continuation, int frame);
	Datum *evalCons(Datum *datum, Scope *scope, Continuation &continuation, int frame);
	bool evalSpecialForm(Datum *datum, Scope *scope, Datum *&ret, Continuation &continuation, int frame);
	Datum *evalLambda(Datum *lambda, Datum *args, Scope *scope, Continuation &continuation, int frame);

	void checkType(Datum *datum, Datum::Type type);

	Datum *car(Datum *datum);
	Datum *cdr(Datum *datum);

	Scope *mRootScope;
	DatumPool *mPool;
};

#endif