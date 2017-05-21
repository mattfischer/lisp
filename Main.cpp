#include "Context.hpp"
#include "Object.hpp"
#include "Error.hpp"
#include "IO.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int test(const std::string &filename)
{
	std::ifstream file(filename.c_str());

	std::string input;
	std::string eval;
	std::string error;
	while (!file.eof() && !file.fail()) {
		std::string line;
		std::getline(file, line);

		int colon = line.find(':');
		if (colon != line.npos) {
			std::string cmd = line.substr(0, colon);
			std::string arg = line.substr(line.find_first_not_of(' ', colon + 1));
			if (cmd == "in") {
				input = arg;
			}
			else if (cmd == "eval") {
				eval = arg;
			}
			else if (cmd == "error") {
				error = arg;
			}

			if (!file.eof() && !file.fail()) {
				continue;
			}
		}

		Context context;
		std::stringstream ss(input);
		Object *object = IO::read(ss, &context);

		std::string gotEval;
		std::string gotError;
		try {
			object = context.eval(object);
			std::stringstream oss;
			oss << object;
			gotEval = oss.str();
		}
		catch (Error e) {
			gotError = e.message();
		}

		if (gotEval != eval || gotError != error) {
			std::cerr << "Test failed: " << input << std::endl;
			if (eval != "") {
				std::cerr << "Expected eval: " << eval << std::endl;
			}
			if (gotEval != "") {
				std::cerr << "Got eval: " << gotEval << std::endl;
			}
			if (error != "") {
				std::cerr << "Expected error: \"" << error << "\"" << std::endl;
			}
			if (gotError != "") {
				std::cerr << "Got error: \"" << gotError << "\"" << std::endl;
			}
			return 1;
		}
		input = "";
		eval = "";
		error = "";
	}

	std::cout << "All tests passed" << std::endl;

	return 0;
}

int repl()
{
	Object *obj;
	Context context;

	do {
		try {
			obj = IO::read(std::cin, &context);
			obj = context.eval(obj);
			std::cout << obj << std::endl;
		}
		catch (Error error) {
			std::cerr << "Error: " << error.message() << std::endl;
			return 1;
		}
	} while (obj != context.nil());

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 3 && !strcmp(argv[1], "--test")) {
		std::string filename(argv[2]);
		return test(filename);
	}

	return repl();
}