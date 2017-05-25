#include "DatumPool.hpp"

Datum *DatumPool::newDatum()
{
	return new Datum();
}

Datum *DatumPool::newNone()
{
	Datum *datum = newDatum();
	datum->setNone();

	return datum;
}

Datum *DatumPool::newBool(bool value)
{
	Datum *datum = newDatum();
	datum->setBool(value);

	return datum;
}

Datum *DatumPool::newInt(int value)
{
	Datum *datum = newDatum();
	datum->setInt(value);
	
	return datum;
}

Datum *DatumPool::newSymbol(const std::string &value)
{
	Datum *datum = newDatum();
	datum->setSymbol(value);
	
	return datum;
}

Datum *DatumPool::newString(const std::string &value)
{
	Datum *datum = newDatum();
	datum->setString(value);
	
	return datum;
}

Datum *DatumPool::newCons(Datum *car, Datum *cdr)
{
	Datum *datum = newDatum();
	datum->setCons(car, cdr);
	
	return datum;
}

Datum *DatumPool::newLambda(std::vector<std::string> &&variables, Datum *body)
{
	Datum *datum = newDatum();
	datum->setLambda(std::move(variables), body);
	
	return datum;
}

Datum *DatumPool::newNativeFunction(Datum::NativeFunction nativeFunction)
{
	Datum *datum = newDatum();
	datum->setNativeFunction(nativeFunction);
	
	return datum;
}
