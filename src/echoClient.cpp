#include <stdlib.h>
#include <connectionHandler.h>
#include <thread>
#include <Sender.h>
#include <Reader.h>
/*
static bool shouldTerminate = false;

void send (ConnectionHandler connectionHandler){
    const short bufSize = 1024;
    char buf[bufSize];
    while (std::cin.getline(buf, bufSize) && !shouldTerminate){
         const short bufSize = 1024;
        char buf[bufSize];
        std::cout << "Send to Server: ";
        std::cin.getline(buf, bufSize);
       std::string line(buf);

		int len=line.length();
        if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
		// connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
        std::cout << "Sent \"" << line << "\" to server " << std::endl;

    }
}

void read (ConnectionHandler connectionHandler){
    while (!shouldTerminate){
           // We can use one of three options to read data from the server:
        // 1. Read a fixed number of characters
        // 2. Read a line (up to the newline character using the getline() buffered reader
        // 3. Read up to the null character
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

		// A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
        std::cout << "Server reply: \"" << answer << "\"."<< std::endl;
        if (answer == "ACK LOGOUT") {
            std::cout << "Exiting...\n" << std::endl;
            shouldTerminate = true;
            connectionHandler.close();
            break;
        }
    }

}

static short bytesToShort(char* bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}


static void shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);

}

*/


/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
    //*************************************************//
    // create mutex == std::mutex mutex;
    // Sender sender(connectionHandler,mutex);
    // Reader reader(connectionHandler,mutex);
    std::mutex mutex;
    Sender sender(connectionHandler,mutex);
    Reader reader(connectionHandler,mutex);
    
    std::thread senderThread(&Sender::run, &sender);
    senderThread.join();
    std::thread readerThread(&Reader::run, &reader);
    readerThread.join();
    //reader.join();

    return 0;
}
