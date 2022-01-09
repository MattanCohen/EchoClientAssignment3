#pragma once
#include <stdlib.h>
#include <connectionHandler.h>
#include <thread>
#include <mutex>



class Sender {
private:
	mutex& _mutex;
	ConnectionHandler connectionHandler;
public:
	Sender(ConnectionHandler conHand, mutex& _mutex);
	void run();
};