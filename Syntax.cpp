#include "Syntax.hpp"

Syntax::Syntax(std::vector<Rule> &&rules)
	: mRules(std::move(rules))
{
}

bool Syntax::transform(Object *object, Object *&ret, Object *nil)
{
	for (const Rule &rule : mRules) {
		std::map<std::string, Object*> matches;
		if (matchPattern(object, rule.pattern, matches)) {
			ret = applyTemplate(rule.templ, matches, nil);
			return true;
		}
	}

	return false;
}

bool Syntax::matchPattern(Object *object, Object *pattern, std::map<std::string, Object*> &matches)
{
	if (pattern->type() == Object::TypeAtom) {
		matches[pattern->stringValue()] = object;
		return true;
	}
	else if (pattern->type() == Object::TypeCons) {
		for (; pattern->type() != Object::TypeNone && object->type() != Object::TypeNone; pattern = pattern->consValue().cdr, object = object->consValue().cdr) {
			if (!matchPattern(object->consValue().car, pattern->consValue().car, matches)) {
				return false;
			}
		}
		if (pattern->type() == Object::TypeNone && object->type() == Object::TypeNone) {
			return true;
		}
	}

	return false;
}

Object *Syntax::applyTemplate(Object *templ, const std::map<std::string, Object*> &matches, Object *nil)
{
	Object *result = nil;
	Object *prev = nil;

	for (Object *cons = templ; cons->type() == Object::TypeCons; cons = cons->consValue().cdr) {
		Object *item = cons->consValue().car;
		if (item->type() == Object::TypeAtom) {
			const std::string &name = item->stringValue();
			std::map<std::string, Object*>::const_iterator it = matches.find(name);
			if (it != matches.end()) {
				item = it->second;
			}
		}
		else if (item->type() == Object::TypeCons) {
			item = applyTemplate(item, matches, nil);
		}

		Object *newCons = new Object();
		newCons->setCons(item, nil);
		if (prev == nil) {
			result = newCons;
		}
		else {
			prev->setCons(prev->consValue().car, newCons);
		}
		prev = newCons;
	}

	return result;
}