#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include "Object.hpp"

#include <map>
#include <string>

class Syntax {
public:
	struct Rule {
		Object *pattern;
		Object *templ;
	};

	Syntax(std::vector<Rule> &&rules);

	bool transform(Object *object, Object *&ret, Object *nil);

private:
	bool matchPattern(Object *object, Object *pattern, std::map<std::string, Object*> &matches, Object *nil);
	bool applyTemplate(Object *templ, std::map<std::string, Object*> &matches, Object *nil, Object *&result);

	std::vector<Rule> mRules;
};

#endif