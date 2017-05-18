#include "Print.hpp"

void print(std::ostream &o, const Object *object)
{
	if (!object) {
		o << "nil";
		return;
	}

	switch (object->type()) {
	case Object::TypeInt:
		o << object->intValue();
		break;

	case Object::TypeString:
		o << "\"" << object->stringValue() << "\"";
		break;

	case Object::TypeAtom:
		o << object->stringValue();
		break;

	case Object::TypeCons:
	{
		const Object *cons = object;
		o << "(";
		while (cons) {
			print(o, cons->carValue());
			cons = cons->cdrValue();
			if (cons) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}
}

std::ostream &operator<<(std::ostream &o, const Object *object)
{
	print(o, object);
	return o;
}