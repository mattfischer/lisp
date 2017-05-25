#ifndef IO_HPP
#define IO_HPP

#include "Datum.hpp"
#include "DatumPool.hpp"

#include <istream>
#include <ostream>

class IO {
public:
	static Datum *read(std::istream &i, DatumPool *pool);
	static void print(std::ostream &o, const Datum *datum);
};

std::ostream &operator<<(std::ostream &o, const Datum *datum);
std::ostream &operator<<(std::ostream &o, Datum::Type type);

#endif