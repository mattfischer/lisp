#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <exception>

class Error : public std::exception {
public:
	Error(const std::string &message);

	const std::string& message();
	const char *what();

private:
	std::string mMessage;
};
#endif