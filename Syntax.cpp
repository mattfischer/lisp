#include "Syntax.hpp"

Syntax::Syntax(std::vector<Rule> &&rules)
	: mRules(std::move(rules))
{
}

bool Syntax::transform(Object *object, Object *&ret, Object *nil)
{
	for (const Rule &rule : mRules) {
		std::map<std::string, Object*> matches;
		if (matchPattern(object, rule.pattern, matches, nil)) {
			applyTemplate(rule.templ, matches, nil, ret);
			return true;
		}
	}

	return false;
}

Object *car(Object *object)
{
	return object->consValue().car;
}

Object *cdr(Object *object)
{
	return object->consValue().cdr;
}

bool Syntax::matchPattern(Object *object, Object *pattern, std::map<std::string, Object*> &matches, Object *nil)
{
	if (pattern->type() == Object::TypeAtom) {
		Object *newCons = new Object();
		newCons->setCons(object, nil);

		const std::string &name = pattern->stringValue();
		std::map<std::string, Object*>::iterator it = matches.find(name);
		if (it == matches.end()) {
			matches[name] = newCons;
		}
		else {
			Object *cons = matches[name];
			while (cdr(cons)->type() == Object::TypeCons) {
				cons = cdr(cons);
			}
			cons->setCons(car(cons), newCons);
		}
		return true;
	}
	else if (pattern->type() == Object::TypeCons) {
		for (; pattern->type() != Object::TypeNone && object->type() != Object::TypeNone;) {
			bool ellipses = false;
			if (cdr(pattern)->type() == Object::TypeCons && car(cdr(pattern))->type() == Object::TypeEllipses) {
				while (object->type() == Object::TypeCons && matchPattern(car(object), car(pattern), matches, nil)) {
					object = cdr(object);
				}
				pattern = cdr(cdr(pattern));
			}
			else {
				if (!matchPattern(car(object), car(pattern), matches, nil)) {
					return false;
				}
				pattern = cdr(pattern);
				object = cdr(object);
			}
		}

		if (pattern->type() == Object::TypeNone && object->type() == Object::TypeNone) {
			return true;
		}
	}

	return false;
}

bool Syntax::applyTemplate(Object *templ, std::map<std::string, Object*> &matches, Object *nil, Object *&result)
{
	Object *prev = nil;
	bool ret = false;

	if (templ->type() == Object::TypeAtom) {
		const std::string &name = templ->stringValue();
		std::map<std::string, Object*>::const_iterator it = matches.find(name);
		if (it != matches.end()) {
			Object *cons = it->second;
			result = car(cons);
			matches[name] = cdr(cons);
			ret = (matches[name]->type() == Object::TypeCons);
		}
		else {
			result = templ;
			ret = false;
		}
	}
	else if (templ->type() == Object::TypeCons) {
		ret = true;
		for (Object *cons = templ; cons->type() == Object::TypeCons; cons = cdr(cons)) {
			bool ellipses = false;
			if (cdr(cons)->type() == Object::TypeCons && car(cdr(cons))->type() == Object::TypeEllipses) {
				ellipses = true;
			}

			bool canRepeat = false;
			do {
				Object *item;
				canRepeat = applyTemplate(car(cons), matches, nil, item);
				if (!canRepeat) {
					ret = false;
				}

				Object *newCons = new Object();
				newCons->setCons(item, nil);
				if (prev == nil) {
					result = newCons;
				}
				else {
					prev->setCons(car(prev), newCons);
				}
				prev = newCons;
			} while (ellipses && canRepeat);

			if (ellipses) {
				cons = cdr(cons);
			}
		}
	}

	return ret;
}