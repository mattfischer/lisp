#include "Datum.hpp"

#include <string.h>

Datum::Type Datum::type() const
{
	return mType;
}

void Datum::setNone()
{
	mType = TypeNone;
}

void Datum::setBool(bool value)
{
	mType = TypeBool;
	mData.boolValue = value;
}

void Datum::setInt(int value)
{
	mType = TypeInt;
	mData.intValue = value;
}

void Datum::setString(const std::string &value)
{
	mType = TypeString;
	mData.stringValue = new std::string(value);
}

void Datum::setSymbol(const std::string &value)
{
	mType = TypeSymbol;
	mData.stringValue = new std::string(value);
}

void Datum::setCons(Datum *car, Datum *cdr)
{
	mType = TypeCons;
	mData.consValue.car = car;
	mData.consValue.cdr = cdr;
}

void Datum::setLambda(std::vector<std::string> &&variables, Datum *body)
{
	mType = TypeLambda;
	
	Lambda *lambda = new Lambda;
	lambda->variables = std::move(variables);
	lambda->body = body;

	mData.lambdaValue = lambda;
}

void Datum::setNativeFunction(NativeFunction nativeFunction)
{
	mType = TypeNativeFunction;
	mData.nativeFunctionValue = nativeFunction;
}

int Datum::intValue() const
{
	return mData.intValue;
}

bool Datum::boolValue() const
{
	return mData.boolValue;
}

const std::string &Datum::stringValue() const
{
	return *mData.stringValue;
}

const Datum::Cons &Datum::consValue() const
{
	return mData.consValue;
}

const Datum::Lambda &Datum::lambdaValue() const
{
	return *mData.lambdaValue;
}

Datum::NativeFunction Datum::nativeFunctionValue() const
{
	return mData.nativeFunctionValue;
}

void Datum::dispose()
{
	switch (mType) {
	case TypeString:
	case TypeSymbol:
		delete mData.stringValue;
		break;

	case TypeLambda:
		delete mData.lambdaValue;
		break;

	default:
		break;
	}
}