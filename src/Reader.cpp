#include <Reader.h>

using namespace std;

Reader::Reader(ConnectionHandler conHand,mutex &mutex) :connectionHandler(conHand) ,_mutex(mutex) {}

void Reader::run() {
   
    while (true) {
        std::lock_guard<std::mutex> lock(_mutex);
        //**************************
        //std::unique_lock<std::mutex> lock(mutex);
        //****************************

        // We can use one of three options to read data from the server:
        // 1. Read a fixed number of characters
        // 2. Read a line (up to the newline character using the getline() buffered reader
        // 3. Read up to the null character
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            /**************************/
            // if there's a way to change a boolean field in sender to make sender thread stop
            break;
        }

        // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
        std::cout << "Server reply: \"" << answer << "\"." << std::endl;
        if (answer == "ACK LOGOUT") {
            std::cout << "Exiting...\n" << std::endl;
            connectionHandler.close();
            break;
        }
    }
}
