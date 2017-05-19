#include "Error.hpp"

Error::Error(const std::string &message)
	: mMessage(message)
{
}

const std::string &Error::message()
{
	return mMessage;
}

const char *Error::what()
{
	return mMessage.c_str();
}