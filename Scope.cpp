#include "Scope.hpp"

#include "Error.hpp"

#include <sstream>

Scope::Scope(Scope *parent, std::map<std::string, Object*> &&variables)
	: mParent(parent), mVariables(std::move(variables))
{
}

Object *Scope::get(const std::string &name)
{
	std::map<std::string, Object*>::iterator it = mVariables.find(name);
	if (it == mVariables.end()) {
		if (mParent) {
			return mParent->get(name);
		}
		else {
			std::stringstream ss;
			ss << "No symbol " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		return it->second;
	}
}

void Scope::set(const std::string &name, Object *value)
{
	std::map<std::string, Object*>::iterator it = mVariables.find(name);
	if (it == mVariables.end()) {
		if (mParent) {
			mParent->set(name, value);
		}
		else {
			std::stringstream ss;
			ss << "No symbol " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		it->second = value;
	}
}
