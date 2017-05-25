#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include "Object.hpp"
#include "ObjectPool.hpp"

#include <map>
#include <string>

class Syntax {
public:
	struct Rule {
		Object *pattern;
		Object *templ;
	};

	Syntax(std::vector<Rule> &&rules);

	bool transform(Object *object, Object *&ret, ObjectPool *pool);

private:
	bool matchPattern(Object *object, Object *pattern, std::map<std::string, Object*> &matches, ObjectPool *pool);
	bool applyTemplate(Object *templ, std::map<std::string, Object*> &matches, ObjectPool *pool, Object *&result);

	std::vector<Rule> mRules;
};

#endif