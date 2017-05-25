#ifndef DATUM_POOL_HPP
#define DATUM_POOL_HPP

#include "Datum.hpp"

class DatumPool
{
public:
	Datum *newDatum();
	
	Datum *newNone();
	Datum *newInt(int value);
	Datum *newBool(bool value);
	Datum *newSymbol(const std::string &value);
	Datum *newString(const std::string &value);
	Datum *newCons(Datum *car, Datum *cdr);
	Datum *newLambda(std::vector<std::string> &&variables, Datum *body);
	Datum *newNativeFunction(Datum::NativeFunction nativeFunction);
};
#endif