#include <Sender.h>

using namespace std;

Sender::Sender(ConnectionHandler conHand, mutex& mutex) :connectionHandler(conHand),_mutex(mutex) {}

void Sender::run() {
    while (true) {
            const short bufSize = 1024;
            char buf[bufSize];
            std::cout << "Send to Server: ";
            std::cin.getline(buf, bufSize);
            std::string line(buf);

            //int len = line.length();
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
            std::cout << "Sent \"" << line << "\" to server " << std::endl;

            /************************/
            // create boolean field that will be changed by reader
            if (line == "LOGOUT") {
                connectionHandler.close();
                break;
            }
        }
}