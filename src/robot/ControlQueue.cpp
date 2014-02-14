#include "ControlQueue.h"

using namespace std;

thread* ControlQueue::startQueueThread() {
	thread* thr = new std::thread(&ControlQueue::run, this);
	while(!this->isInitialized());
	return thr;
}

ControlQueue::ControlQueue(int degOfFreedom) {
	this->degOfFreedom = degOfFreedom;
}

int ControlQueue::getMovementDegreesOfFreedom() {
	return degOfFreedom;
}