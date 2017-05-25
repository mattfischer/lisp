#include "NativeFunctions.hpp"

#include "Error.hpp"
#include "IO.hpp"

#include <sstream>
#include <cstdarg>

void getArgs(Datum *datum, int length, ...)
{
	va_list ap;
	va_start(ap, length);
	int datumLength = 0;
	for (Datum *cons = datum; cons->type() != Datum::TypeNone; cons = cons->consValue().cdr) {
		if (datumLength < length) {
			Datum::Type type = va_arg(ap, Datum::Type);
			Datum **arg = va_arg(ap, Datum**);
			*arg = cons->consValue().car;
			if ((*arg)->type() != type) {
				std::stringstream ss;
				ss << "Type mismatch on " << *arg << " (expected " << type << " got " << (*arg)->type() << ")";
				throw Error(ss.str());
			}
		}
		datumLength++;
	}

	if (datumLength != length) {
		std::stringstream ss;
		ss << "Argument list " << datum << " is of incorrect length (expected " << length << " got " << datumLength << ")";
		throw Error(ss.str());
	}
}

Datum *add(Datum *args, DatumPool *pool)
{
	Datum *a, *b;
	getArgs(args, 2, Datum::TypeInt, &a, Datum::TypeInt, &b);

	Datum *ret = new Datum();
	ret->setInt(a->intValue() + b->intValue());
	return ret;
}

Datum *subtract(Datum *args, DatumPool *pool)
{
	Datum *a, *b;
	getArgs(args, 2, Datum::TypeInt, &a, Datum::TypeInt, &b);

	Datum *ret = new Datum();
	ret->setInt(a->intValue() - b->intValue());
	return ret;
}

Datum *multiply(Datum *args, DatumPool *pool)
{
	Datum *a, *b;
	getArgs(args, 2, Datum::TypeInt, &a, Datum::TypeInt, &b);

	Datum *ret = new Datum();
	ret->setInt(a->intValue() * b->intValue());
	return ret;
}

Datum *divide(Datum *args, DatumPool *pool)
{
	Datum *a, *b;
	getArgs(args, 2, Datum::TypeInt, &a, Datum::TypeInt, &b);

	Datum *ret = new Datum();
	ret->setInt(a->intValue() / b->intValue());
	return ret;
}

std::map<std::string, Datum*> NativeFunctions::functions(DatumPool *pool)
{
	std::map<std::string, Datum*> functions;

	functions["+"] = pool->newNativeFunction(add);
	functions["-"] = pool->newNativeFunction(subtract);
	functions["*"] = pool->newNativeFunction(multiply);
	functions["/"] = pool->newNativeFunction(divide);

	return functions;
}
