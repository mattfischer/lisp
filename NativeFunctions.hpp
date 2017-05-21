#ifndef NATIVE_FUNCTIONS_HPP
#define NATIVE_FUNCTIONS_HPP

#include "Object.hpp"

#include <map>
#include <string>

class NativeFunctions {
public:
	static std::map<std::string, Object*> functions();
};
#endif