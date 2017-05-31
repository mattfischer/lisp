#include "Continuation.hpp"

std::vector<Continuation::Frame> &Continuation::frames()
{
	return mFrames;
}

void Continuation::addFrame(const Frame &frame)
{
	mFrames.push_back(frame);
}

void Continuation::popFrame()
{
	mFrames.pop_back();
}