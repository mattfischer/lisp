#ifndef DATUM_HPP
#define DATUM_HPP

#include <string>
#include <vector>

class DatumPool;

class Datum {
public:
	enum Type {
		TypeNone,
		TypeBool,
		TypeInt,
		TypeString,
		TypeSymbol,
		TypeCons,
		TypeLambda,
		TypeNativeFunction
	};

	struct Cons {
		Datum *car;
		Datum *cdr;
	};

	struct Lambda {
		std::vector<std::string> variables;
		Datum *body;
	};

	typedef Datum *(*NativeFunction)(Datum*, DatumPool*);

	Type type() const;

	void setNone();
	void setBool(bool value);
	void setInt(int value);
	void setString(const std::string &value);
	void setSymbol(const std::string &value);
	void setCons(Datum *car, Datum *cdr);
	void setLambda(std::vector<std::string> &&variables, Datum *body);
	void setNativeFunction(NativeFunction nativeFunction);

	int intValue() const;
	bool boolValue() const;
	const std::string &stringValue() const;
	const Cons &consValue() const;
	const Lambda &lambdaValue() const;
	NativeFunction nativeFunctionValue() const;

	void dispose();

private:
	Type mType;

	union {
		int intValue;
		bool boolValue;
		std::string *stringValue;
		Cons consValue;
		Lambda *lambdaValue;
		NativeFunction nativeFunctionValue;
	} mData;
};

#endif