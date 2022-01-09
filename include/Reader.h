#pragma once
#include <stdlib.h>
#include <connectionHandler.h>
#include <thread>
#include <mutex>



class Reader {
private:
	mutex& _mutex;
	ConnectionHandler connectionHandler;
public:
	Reader(ConnectionHandler conHand, mutex& _mutex);
	void run();
};