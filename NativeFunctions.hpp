#ifndef NATIVE_FUNCTIONS_HPP
#define NATIVE_FUNCTIONS_HPP

#include "Datum.hpp"
#include "DatumPool.hpp"

#include <map>
#include <string>

class NativeFunctions {
public:
	static std::map<std::string, Datum*> functions(DatumPool *pool);
};
#endif