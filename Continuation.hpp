#ifndef CONTINUATION_HPP
#define CONTINUATION_HPP

#include <vector>

class Continuation
{
public:
	struct Frame {

	};

	std::vector<Frame> &frames();

	void addFrame(const Frame &frame);
	void popFrame();

private:
	std::vector<Frame> mFrames;
};
#endif