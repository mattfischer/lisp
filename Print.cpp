#include "Print.hpp"

void print(std::ostream &o, const Object *object)
{
	if (!object) {
		o << "nil";
		return;
	}

	switch (object->type) {
	case Object::TypeInt:
		o << ((ObjectInt*)object)->value;
		break;

	case Object::TypeString:
		o << "\"" << ((ObjectString*)object)->value << "\"";
		break;

	case Object::TypeAtom:
		o << ((ObjectAtom*)object)->value;
		break;

	case Object::TypeList:
	{
		o << "(";
		Cons *cons = ((ObjectList*)object)->value;
		while (cons) {
			print(o, cons->car);
			cons = cons->cdr;
			if (cons) {
				o << " ";
			}
		}
		o << ")";
		break;
	}
	}
}
