#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <vector>

class Object {
public:
	enum Type {
		TypeNone,
		TypeT,
		TypeInt,
		TypeString,
		TypeAtom,
		TypeCons,
		TypeLambda
	};

	struct Cons {
		Object *car;
		Object *cdr;
	};

	struct Lambda {
		int numVariables;
		char **variables;
		Object *body;
	};

	Type type() const;

	void setNone();
	void setT();
	void setInt(int value);
	void setString(const char *value);
	void setAtom(const char *value);
	void setCons(Object *car, Object *cdr);
	void setLambda(int numVariables, char **variables, Object *body);

	int intValue() const;
	const char *stringValue() const;
	const Cons &consValue() const;
	const Lambda &lambdaValue() const;

	void dispose();

private:
	Type mType;

	union {
		int intValue;
		char *stringValue;
		Cons consValue;
		Lambda lambdaValue;
	} mData;
};

#endif