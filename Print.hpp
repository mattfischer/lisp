#ifndef PRINT_HPP
#define PRINT_HPP

#include "Object.hpp"

#include <ostream>

void print(std::ostream &o, const Object *object);
std::ostream &operator<<(std::ostream &o, const Object *object);

#endif