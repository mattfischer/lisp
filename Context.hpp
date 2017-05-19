#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Object.hpp"

#include <istream>
#include <ostream>
#include <map>

class Context {
public:
	Context();
	Context(Context *parent, std::map<std::string, Object*> &&variables);

	Object *read(std::istream &i);
	static void print(std::ostream &o, const Object *object);
	Object *eval(Object *object);

private:
	Object *evalAtom(Object *object);
	Object *evalCons(Object *object);
	void evalArgs(Object *object, int length, ...);
	void checkType(Object *object, Object::Type type);

	void setVariable(const std::string &name, Object *value);

	Context *mParent;
	std::map<std::string, Object*> mVariables;
};

std::ostream &operator<<(std::ostream &o, const Object *object);

#endif