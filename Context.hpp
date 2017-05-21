#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Object.hpp"

#include <map>

class Context {
public:
	Context();
	Context(Context *parent, std::map<std::string, Object*> &&variables);

	Object *eval(Object *object);

	Object *nil();
	Object *t();

private:
	Object *evalAtom(Object *object);
	Object *evalCons(Object *object);
	Object *evalLambda(Object *function, Object *args);
	void evalArgs(Object *object, int length, ...);
	void checkType(Object *object, Object::Type type);

	Object *getVariable(const std::string &name);
	void setVariable(const std::string &name, Object *value);

	Context *mParent;
	std::map<std::string, Object*> mVariables;
	Object *mNil;
	Object *mT;
};

#endif