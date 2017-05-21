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
		std::vector<std::string> variables;
		Object *body;
	};

	Type type() const;

	void setNone();
	void setT();
	void setInt(int value);
	void setString(const std::string &value);
	void setAtom(const std::string &value);
	void setCons(Object *car, Object *cdr);
	void setLambda(std::vector<std::string> &&variables, Object *body);

	int intValue() const;
	const std::string &stringValue() const;
	const Cons &consValue() const;
	const Lambda &lambdaValue() const;

	void dispose();

private:
	Type mType;

	union {
		int intValue;
		std::string *stringValue;
		Cons consValue;
		Lambda *lambdaValue;
	} mData;
};

#endif