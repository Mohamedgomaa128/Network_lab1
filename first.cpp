#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <netdb.h>

using namespace std;
#define PORT_ORG 80
struct sockaddr_in serverAddress;
int sockfd;


char buffer[100];
// file path on server struct sockaddr_in serverAddress;
void get(string filePath, string hostName, int port){
	//reading from the file

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	int connection = inet_pton(AF_INET, hostName.c_str(), &serverAddress.sin_addr);

	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}

	// TO BE MODIFIED
	stringstream ss;
	//ss << "message";

	ss << "GET " << filePath << " HTTP/1.1"
			<< "\r\n\r\n";
	//cout << ss.str();

	string theMessage = ss.str();
	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);
	int readd = 100;

	while (readd != 0){

		readd = read(sockfd, buffer, 100);
		cout << readd << endl;

		if (readd != 100){
			cout << "error in reading" << endl;
			exit(-100);
		}
	}
	//write to the file chunk by chunk >> may loop
	close(connection);
}




void post(string filePath, string hostName, int port){ // will already passed
// upload file
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	int connection = inet_pton(AF_INET, hostName.c_str(), &serverAddress.sin_addr);

	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}

	ifstream myfile(filePath);
	string s;

	if (myfile.is_open()){
		myfile >> s;
		cout << s;
	}

	ifstream fileToBeSent(filePath, ios::binary);
	ostringstream oss;
	oss << "POST " << filePath << " HTTP/1.1"
				<< "\r\n\r\n";
	cout << oss.str();

	oss << fileToBeSent.rdbuf();
	cout << oss.str();

	// TO BE MODIFIED
	//stringstream ss;
	//ss << "message";
	string theMessage = oss.str();

	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);
	int readed = 100;

	while (readed != 0){
		readed = read(sockfd, buffer, 100);
		cout << readed << endl;

		if (readed != 100){
			cout << "error in reading" << endl;
			exit(-100);
		}
	}
	//write to the file chunk by chunk >> may loop

	close(connection);
}


int main(int argc, char * argv[]) {


// argc by default = 1
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cout << "error in creating socket" << endl;
		exit(-100);
	}
/*	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = hton(PORT_ORG);
	int ret = inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	if (ret < 0){
		cout << "invalid address / address not supported" << endl;
		exit(-100);
	}GetAdaptersInfoGetAdaptersInfo

	int connection = connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}
	*/





	string line;

	ifstream commandFile;
	string command[argc - 1];
	//cout << commandFile.is_open()<< endl;
	commandFile.open("command.txt", ios::out);
	//cout << commandFile.is_open()<< endl;

	argc = 4;

	cout << line;

	while (getline(commandFile, line)){
		// read commands from the file
		 cout << line << endl;
		 string delimiter = " ";
		 int i = 0;



		 // parse command
		 string temp = "";
		 for (auto ch : line){
			 if (ch != ' ')
				 temp += ch;
			 else {
				 command[i++] = temp;
				 //cout << temp << endl;
				 temp = "";
			 }
		 }
		 if (temp != ""){
			 command[i] = temp;
			 //cout << temp << endl;
		 }


		 const char * name = command[2].c_str();
		 struct hostent* host = gethostbyname(name);
		 // if get go to get
		 if (command[0].compare("client_get") == 0)
			 get(command[1], host->h_addr, (argc == 4 ? stoi(command[3]) : PORT_ORG));
		 else
			 post(command[1], host->h_addr, (argc == 4 ? stoi(command[3]) : PORT_ORG));

	}

	commandFile.close();

	//post("jksdflfjs/fskjl", "jslfkjsl.cjlkdjfsl.kljfsdl", 9);
	return 0;
}





