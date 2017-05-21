#include "Object.hpp"

#include <string.h>

Object::Type Object::type() const
{
	return mType;
}

void Object::setNone()
{
	mType = TypeNone;
}

void Object::setT()
{
	mType = TypeT;
}

void Object::setInt(int value)
{
	mType = TypeInt;
	mData.intValue = value;
}

void Object::setString(const std::string &value)
{
	mType = TypeString;
	mData.stringValue = new std::string(value);
}

void Object::setAtom(const std::string &value)
{
	mType = TypeAtom;
	mData.stringValue = new std::string(value);
}

void Object::setCons(Object *car, Object *cdr)
{
	mType = TypeCons;
	mData.consValue.car = car;
	mData.consValue.cdr = cdr;
}

void Object::setLambda(std::vector<std::string> &&variables, Object *body)
{
	mType = TypeLambda;
	
	Lambda *lambda = new Lambda;
	lambda->variables = std::move(variables);
	lambda->body = body;

	mData.lambdaValue = lambda;
}

int Object::intValue() const
{
	return mData.intValue;
}

const std::string &Object::stringValue() const
{
	return *mData.stringValue;
}

const Object::Cons &Object::consValue() const
{
	return mData.consValue;
}

const Object::Lambda &Object::lambdaValue() const
{
	return *mData.lambdaValue;
}

void Object::dispose()
{
	switch (mType) {
	case TypeString:
	case TypeAtom:
		delete mData.stringValue;
		break;

	case TypeLambda:
		delete mData.lambdaValue;
		break;

	default:
		break;
	}
}