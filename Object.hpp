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
	Object *carValue() const;
	Object *cdrValue() const;
	int numVariables() const;
	char **variables() const;
	Object *lambdaBody() const;

	void dispose();

private:
	Type mType;

	union {
		int intValue;
		char *stringValue;
		struct { Object *car; Object *cdr; } consValue;
		struct { int numVariables; char **variables; Object *body; } lambdaValue;
	} mData;
};

#endif