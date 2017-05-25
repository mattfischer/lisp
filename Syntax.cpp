#include "Syntax.hpp"

Syntax::Syntax(std::vector<Rule> &&rules)
	: mRules(std::move(rules))
{
}

bool Syntax::transform(Object *object, Object *&ret, ObjectPool *pool)
{
	for (const Rule &rule : mRules) {
		std::map<std::string, Object*> matches;
		if (matchPattern(object, rule.pattern, matches, pool)) {
			applyTemplate(rule.templ, matches, pool, ret);
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

bool Syntax::matchPattern(Object *object, Object *pattern, std::map<std::string, Object*> &matches, ObjectPool *pool)
{
	if (pattern->type() == Object::TypeAtom) {
		const std::string &name = pattern->stringValue();
		std::map<std::string, Object*>::iterator it = matches.find(name);
		if (it == matches.end()) {
			matches[name] = pool->newCons(object, pool->newNone());
		}
		else {
			Object *cons = matches[name];
			while (cdr(cons)->type() == Object::TypeCons) {
				cons = cdr(cons);
			}
			cons->setCons(car(cons), pool->newCons(object, cdr(cons)));
		}
		return true;
	}
	else if (pattern->type() == Object::TypeCons) {
		for (; pattern->type() != Object::TypeNone && object->type() != Object::TypeNone;) {
			bool ellipses = false;
			if (cdr(pattern)->type() == Object::TypeCons && car(cdr(pattern))->type() == Object::TypeAtom && car(cdr(pattern))->stringValue() == "...") {
				while (object->type() == Object::TypeCons && matchPattern(car(object), car(pattern), matches, pool)) {
					object = cdr(object);
				}
				pattern = cdr(cdr(pattern));
			}
			else {
				if (!matchPattern(car(object), car(pattern), matches, pool)) {
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

bool Syntax::applyTemplate(Object *templ, std::map<std::string, Object*> &matches, ObjectPool *pool, Object *&result)
{
	Object *nil = pool->newNone();
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
			if (cdr(cons)->type() == Object::TypeCons && car(cdr(cons))->type() == Object::TypeAtom && car(cdr(cons))->stringValue() == "...") {
				ellipses = true;
			}

			bool canRepeat = false;
			do {
				Object *item;
				canRepeat = applyTemplate(car(cons), matches, pool, item);
				if (!canRepeat) {
					ret = false;
				}

				Object *newCons = pool->newCons(item, nil);
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