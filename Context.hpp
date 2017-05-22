#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Object.hpp"
#include "Scope.hpp"

class Context {
public:
	Context();

	Object *eval(Object *object);

	Object *nil();
	Object *t();

private:
	Object *eval(Object *object, Scope *scope);
	Object *evalCons(Object *object, Scope *scope);
	bool evalSpecialForm(Object *object, Scope *scope, Object *&ret);
	Object *evalLambda(Object *lambda, Object *args, Scope *scope);

	void checkType(Object *object, Object::Type type);

	Object *car(Object *object);
	Object *cdr(Object *object);

	Object *mNil;
	Object *mT;
	Scope *mRootScope;
};

#endif