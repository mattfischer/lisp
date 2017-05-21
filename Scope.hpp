#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "Object.hpp"

#include <map>

class Scope {
public:
	Scope(Scope *parent, std::map<std::string, Object*> &&variables);

	Object *get(const std::string &name);
	void set(const std::string &name, Object *value);

private:
	Scope *mParent;
	std::map<std::string, Object*> mVariables;

};
#endif