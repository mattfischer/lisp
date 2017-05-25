#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include "Datum.hpp"
#include "DatumPool.hpp"

#include <map>
#include <string>

class Syntax {
public:
	struct Rule {
		Datum *pattern;
		Datum *templ;
	};

	Syntax(std::vector<Rule> &&rules);

	bool transform(Datum *datum, Datum *&ret, DatumPool *pool);

private:
	bool matchPattern(Datum *datum, Datum *pattern, std::map<std::string, Datum*> &matches, DatumPool *pool);
	bool applyTemplate(Datum *templ, std::map<std::string, Datum*> &matches, DatumPool *pool, Datum *&result);

	std::vector<Rule> mRules;
};

#endif