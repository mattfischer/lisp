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

void Scope::set(const std::string &name, Object *value, bool create)
{
	std::map<std::string, Object*>::iterator it = mVariables.find(name);
	if (it == mVariables.end()) {
		if (create) {
			mVariables[name] = value;
		}
		else if (mParent) {
			mParent->set(name, value, false);
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

bool Scope::contains(const std::string &name)
{
	if (mVariables.find(name) != mVariables.end()) {
		return true;
	}
	else if (mParent) {
		return mParent->contains(name);
	}

	return false;
}

Syntax *Scope::getSyntax(const std::string &name)
{
	std::map<std::string, Syntax*>::iterator it = mSyntaxes.find(name);
	if (it == mSyntaxes.end()) {
		if (mParent) {
			return mParent->getSyntax(name);
		}
		else {
			std::stringstream ss;
			ss << "No syntax " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		return it->second;
	}
}

void Scope::setSyntax(const std::string &name, Syntax *value, bool create)
{
	std::map<std::string, Syntax*>::iterator it = mSyntaxes.find(name);
	if (it == mSyntaxes.end()) {
		if (create) {
			mSyntaxes[name] = value;
		}
		else if (mParent) {
			mParent->setSyntax(name, value, false);
		}
		else {
			std::stringstream ss;
			ss << "No syntax " << name << " defined";
			throw Error(ss.str());
		}
	}
	else {
		it->second = value;
	}
}

bool Scope::containsSyntax(const std::string &name)
{
	if (mSyntaxes.find(name) != mSyntaxes.end()) {
		return true;
	}
	else if (mParent) {
		return mParent->containsSyntax(name);
	}

	return false;
}
