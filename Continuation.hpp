#ifndef CONTINUATION_HPP
#define CONTINUATION_HPP

#include "Datum.hpp"

#include <vector>

class Continuation
{
public:
	struct Frame {
		Datum *next;
		Datum *current;
	};

	std::vector<Frame> &frames();

	void addFrame(const Frame &frame);
	void popFrame();

private:
	std::vector<Frame> mFrames;
};
#endif