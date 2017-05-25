#include "Syntax.hpp"

Syntax::Syntax(std::vector<Rule> &&rules)
	: mRules(std::move(rules))
{
}

bool Syntax::transform(Datum *datum, Datum *&ret, DatumPool *pool)
{
	for (const Rule &rule : mRules) {
		std::map<std::string, Datum*> matches;
		if (matchPattern(datum, rule.pattern, matches, pool)) {
			applyTemplate(rule.templ, matches, pool, ret);
			return true;
		}
	}

	return false;
}

Datum *car(Datum *datum)
{
	return datum->consValue().car;
}

Datum *cdr(Datum *datum)
{
	return datum->consValue().cdr;
}

bool Syntax::matchPattern(Datum *datum, Datum *pattern, std::map<std::string, Datum*> &matches, DatumPool *pool)
{
	if (pattern->type() == Datum::TypeSymbol) {
		const std::string &name = pattern->stringValue();
		std::map<std::string, Datum*>::iterator it = matches.find(name);
		if (it == matches.end()) {
			matches[name] = pool->newCons(datum, pool->newNone());
		}
		else {
			Datum *cons = matches[name];
			while (cdr(cons)->type() == Datum::TypeCons) {
				cons = cdr(cons);
			}
			cons->setCons(car(cons), pool->newCons(datum, cdr(cons)));
		}
		return true;
	}
	else if (pattern->type() == Datum::TypeCons) {
		for (; pattern->type() != Datum::TypeNone && datum->type() != Datum::TypeNone;) {
			bool ellipses = false;
			if (cdr(pattern)->type() == Datum::TypeCons && car(cdr(pattern))->type() == Datum::TypeSymbol && car(cdr(pattern))->stringValue() == "...") {
				while (datum->type() == Datum::TypeCons && matchPattern(car(datum), car(pattern), matches, pool)) {
					datum = cdr(datum);
				}
				pattern = cdr(cdr(pattern));
			}
			else {
				if (!matchPattern(car(datum), car(pattern), matches, pool)) {
					return false;
				}
				pattern = cdr(pattern);
				datum = cdr(datum);
			}
		}

		if (pattern->type() == Datum::TypeNone && datum->type() == Datum::TypeNone) {
			return true;
		}
	}

	return false;
}

bool Syntax::applyTemplate(Datum *templ, std::map<std::string, Datum*> &matches, DatumPool *pool, Datum *&result)
{
	Datum *nil = pool->newNone();
	Datum *prev = nil;
	bool ret = false;

	if (templ->type() == Datum::TypeSymbol) {
		const std::string &name = templ->stringValue();
		std::map<std::string, Datum*>::const_iterator it = matches.find(name);
		if (it != matches.end()) {
			Datum *cons = it->second;
			result = car(cons);
			matches[name] = cdr(cons);
			ret = (matches[name]->type() == Datum::TypeCons);
		}
		else {
			result = templ;
			ret = false;
		}
	}
	else if (templ->type() == Datum::TypeCons) {
		ret = true;
		for (Datum *cons = templ; cons->type() == Datum::TypeCons; cons = cdr(cons)) {
			bool ellipses = false;
			if (cdr(cons)->type() == Datum::TypeCons && car(cdr(cons))->type() == Datum::TypeSymbol && car(cdr(cons))->stringValue() == "...") {
				ellipses = true;
			}

			bool canRepeat = false;
			do {
				Datum *item;
				canRepeat = applyTemplate(car(cons), matches, pool, item);
				if (!canRepeat) {
					ret = false;
				}

				Datum *newCons = pool->newCons(item, nil);
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