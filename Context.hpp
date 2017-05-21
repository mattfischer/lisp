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
	void evalArgs(Object *object, Scope *scope, int length, ...);
	void checkType(Object *object, Object::Type type);

	Object *car(Object *object);
	Object *cdr(Object *object);

	Object *mNil;
	Object *mT;
	Scope *mRootScope;
};

#endif