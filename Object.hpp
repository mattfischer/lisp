#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <ostream>
#include <istream>
#include <vector>

struct Object {
	enum Type {
		TypeInt,
		TypeString,
		TypeAtom,
		TypeList
	};

	Type type;

};

struct ObjectInt : public Object {
	int value;

	ObjectInt(int _value);
};

struct ObjectString : public Object {
	std::string value;

	ObjectString(const std::string &_value);
};

struct ObjectAtom : public Object {
	std::string value;

	ObjectAtom(const std::string &_value);
};

struct Cons {
	Object *car;
	Cons *cdr;
};

struct ObjectList : public Object {
	struct Cons *value;

	ObjectList(int length, ...);
	ObjectList(const std::vector<Object*> &_value);
};

std::istream &operator>>(std::istream &i, Object *&object);
#endif