#include "Eval.hpp"
#include "Print.hpp"

#include <iostream>
#include <cstring>

Object *evalFunction(Object *object)
{
	Object *car = object->carValue();
	if (car->type() != Object::TypeAtom) {
		std::cerr << "Error: Type mismatch evaluating " << object << std::endl;
		return 0;
	}

	if (!std::strcmp(car->stringValue(), "+")) {
		int value = 0;
		Object *cons = object->cdrValue();
		while (cons) {
			Object *consEval = eval(cons->carValue());
			if (consEval->type() != Object::TypeInt) {
				std::cerr << "Error: " << consEval << " is non-integral" << std::endl;
				return 0;
			}
			value += consEval->intValue();
			cons = cons->cdrValue();
		}
		Object *ret = new Object();
		ret->setInt(value);
		return ret;
	}

	std::cerr << "Error: No function " << car->stringValue() << std::endl;
	return 0;
}

Object *eval(Object *object)
{
	if (!object) {
		return object;
	}

	switch (object->type()) {
	case Object::TypeInt:
	case Object::TypeString:
	case Object::TypeAtom:
		return object;

	default:
		break;
	}

	return evalFunction(object);
}