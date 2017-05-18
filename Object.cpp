#include "Object.hpp"

#include <string.h>

Object::Type Object::type() const
{
	return mType;
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

int Object::intValue() const
{
	return mData.intValue;
}

const char *Object::stringValue() const
{
	return mData.stringValue;
}

Object *Object::carValue() const
{
	return mData.consValue.car;
}

Object *Object::cdrValue() const
{
	return mData.consValue.cdr;
}

void Object::dispose()
{
	switch (mType) {
	case TypeString:
	case TypeAtom:
		free(mData.stringValue);
		break;

	default:
		break;
	}
}