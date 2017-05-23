#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "Object.hpp"
#include "Syntax.hpp"

#include <map>

class Scope {
public:
	Scope(Scope *parent, std::map<std::string, Object*> &&variables);

	Object *get(const std::string &name);
	void set(const std::string &name, Object *value, bool create);
	bool contains(const std::string &name);

	Syntax *getSyntax(const std::string &name);
	void setSyntax(const std::string &name, Syntax *value, bool create);
	bool containsSyntax(const std::string &name);

private:
	Scope *mParent;
	std::map<std::string, Object*> mVariables;
	std::map<std::string, Syntax*> mSyntaxes;
};
#endif