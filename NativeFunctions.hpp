#ifndef NATIVE_FUNCTIONS_HPP
#define NATIVE_FUNCTIONS_HPP

#include "Object.hpp"

class NativeFunctions {
public:
	static Object *add(Object *args);
	static Object *subtract(Object *args);
	static Object *multiply(Object *args);
	static Object *divide(Object *args);
};
#endif