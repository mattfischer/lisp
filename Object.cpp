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

void Object::setString(const char *value)
{
	mType = TypeString;
	mData.stringValue = strdup(value);
}

void Object::setAtom(const char *value)
{
	mType = TypeAtom;
	mData.stringValue = strdup(value);
}

void Object::setCons(Object *car, Object *cdr)
{
	mType = TypeCons;
	mData.consValue.car = car;
	mData.consValue.cdr = cdr;
}

void Object::setLambda(int numVariables, char **variables, Object *body)
{
	mType = TypeLambda;
	mData.lambdaValue.numVariables = numVariables;
	mData.lambdaValue.variables = variables;
	mData.lambdaValue.body = body;
}

int Object::intValue() const
{
	return mData.intValue;
}

const char *Object::stringValue() const
{
	return mData.stringValue;
}

const Object::Cons &Object::consValue() const
{
	return mData.consValue;
}

const Object::Lambda &Object::lambdaValue() const
{
	return mData.lambdaValue;
}

void Object::dispose()
{
	switch (mType) {
	case TypeString:
	case TypeAtom:
		free(mData.stringValue);
		break;

	case TypeLambda:
		for (int i = 0; i < mData.lambdaValue.numVariables; i++) {
			free(mData.lambdaValue.variables[i]);
		}
		free(mData.lambdaValue.variables);
		break;

	default:
		break;
	}
}