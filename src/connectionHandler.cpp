#include <connectionHandler.h>
 
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 using namespace std;
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
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


static void decode(string& msg) {
    string decodedMsg = "";
    char opCode[2] = { msg[0],msg[1] };
    msg = msg.substr(2);
    switch (bytesToShort(opCode)) {
        case 9:{  //if message is notification
            //add to decoded message noitifcation identifier
            decodedMsg += "NOTIFICATION ";
            //check if notification is pm or public and add PM or PUBLIC accordingly
            char pmPublic = msg[0];
            switch (pmPublic) {
                case '0':
                    decodedMsg += "PM ";
                    break;
                case '1':
                    decodedMsg += "PUBLIC ";
                    break;
            }
            msg = msg.substr(1);
            decodedMsg += "from ";
            // the rest of the message until the byte symbol \0 is the userName
            while (msg[0] != '\0') {
                decodedMsg += msg[0];
                msg = msg.substr(1);
            }
            //instead of 0 add a dot and space (NOTIFICATION PM/PUBLIS from USERNAME: )
            decodedMsg += ": ";
                // the rest of the message until the byte symbol \0 which if followed by ';' is the content
            while (msg[0] != '\0') {
                decodedMsg += msg[0];
                msg = msg.substr(1);
            }
            //now message is complete so change message to be decodedMsg
            msg = decodedMsg;
            break;
        }


        case 10: { //if message is ACK
            break;
        }


        case 11: { //if message is error
            break;
        }
    }

}

//static bool stringToOpcode (string &msg){
//    char ans[2];
//        if (msg == "REGISTER"){
//            shortToBytes(((short)1),ans);
//            msg = ans;
//            return true;
//        }
//        if (msg == "LOGIN"){
//            return true;
//
//        }
//        if (msg == "LOGOUT"){
//            shortToBytes(((short)3),ans);
//            while(true)
//                cout<<ans[2]<<endl;
//            msg = ans;
//            return true;
//        }
//        if (msg == "LOGOUT"){
//            return true;
//
//        }
//        if (msg == "FOLLOW"){
//            return true;
//
//        }
//        if (msg == "POST"){
//            return true;
//
//        }
//        if (msg == "PM"){
//            return true;
//
//        }
//        if (msg == "LOGSTAT"){
//            return true;
//
//        }
//        if (msg == "STAT"){
//            return true;
//
//        }
//        if (msg == "NOTIFICATION"){
//            return true;
//
//        }
//        if (msg == "ACK"){
//            return true;
//
//        }
//        if (msg == "ERROR"){
//            return true;
//
//        }
//        if (msg == "BLOCK"){
//            shortToBytes(((short)12),ans);
//            msg = ans;
//            return true;
//        }
//        return false;
//    }


static string encode(const std::string& frame) {
    // create msg from frame
    string msg(frame);
    // if msg is logout
    if (msg == "LOGOUT"){
        msg = "03";
        return msg;
    }
    // if msg is logstat
    if (msg == "LOGSTAT"){
        msg = "07";
        return msg;
    }
    // from https://www.delftstack.com/howto/cpp/cpp-split-string-by-space/
    /*
     * split by space_delimiter (" ")
     */
    string space_delimiter = " ";
    vector<string> splitMessage{};
    size_t pos = 0;
    while ((pos = msg.find(space_delimiter)) != string::npos ) {
        splitMessage.push_back(msg.substr(0, pos));
        msg.erase(0, pos + space_delimiter.length());
    }
    splitMessage.push_back(msg);
    msg.erase(0,msg.size());


    int wordNum = 0;
    if (splitMessage[wordNum] == "REGISTER"){
        // set msg = REGISTER
        msg = "01";
        wordNum++;

        // add userName as normal string
        msg += splitMessage[wordNum];
        wordNum++;
        // signify end of string with a '\0'
        msg += '\0';

        // add password as normal string
        msg += splitMessage[wordNum];
        wordNum++;
        // signify end of string with a '\0'
        msg += '\0';

        // add birthday as normal DD-MM-YYYY string
        msg += splitMessage[wordNum];
        wordNum++;
        // signify end of string with a '\0'
        msg += '\0';

        return msg;
    }


    return nullptr;
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

/*
 *              WHERE TO DECODE
 *                  WHERE
 *                          WHERE
 *               WHERE
 *                                          WHERE
 */
bool ConnectionHandler::getLine(std::string& line) {
    bool ans = getFrameAscii(line, ';');
    decode(line);
    return ans;
}


bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, ';');
}
 
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
		do{
			getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    cout<<"msg to send "<<frame<<endl;
    string msg = encode(frame);
    cout<<"sent msg is "<<msg<<endl;
    bool result=sendBytes(msg.c_str(), msg.size());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}