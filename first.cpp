#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <netdb.h>
#include <pthread.h>
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

	stringstream ss;
	//ss << "message";

	ss << "GET " << filePath << " HTTP/1.1"
			<< "\r\n\r\n";
	//cout << ss.str();

	string theMessage = ss.str();
	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);
	int readd = 100;
	string mainString;

	while (readd != 0){
		// reading the file from the server and writing it into new file with the same extension
		readd = read(sockfd, buffer, 100);
		cout << readd << endl;
		// appending to main string which will be cleaned form unwanted data and written to the file
		//mainString << buffer;
		cout << "mainSting now : " << mainString << endl;

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
	// first open connection
	int connection = inet_pton(AF_INET, hostName.c_str(), &serverAddress.sin_addr);

	char stt[100] ;
	inet_ntop(AF_INET, &serverAddress.sin_addr, stt, hostName.length());
	//cout <<  stt.()<< endl;
	cout << serverAddress.sin_port << endl;
	cout << serverAddress.sin_family << endl;
	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}

	// reading needed file
	ifstream fileToBeSent(filePath, ios::binary);
	ostringstream oss;
	oss << "POST " << filePath;//<< " HTTP/1.1" << "\r\n";

	cout << oss.str();

	oss << fileToBeSent.rdbuf();
	oss << "\r\n\r\n";
	cout << oss.str();

	//stringstream ss;
	//ss << "message";
	string theMessage = oss.str();
	int readed = 100;
	readed = read(sockfd, buffer, 100);


	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);


	//write to the file chunk by chunk >> may loop

	close(connection);
}



void* server(void * in) {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

		if (server_fd < 0){
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		int opt;
		if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		struct sockaddr_in clientAddress;
		clientAddress.sin_family = AF_INET;
		clientAddress.sin_port = htons(PORT_ORG);
		clientAddress.sin_addr.s_addr = INADDR_ANY;

		int bindRet = bind(server_fd, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
		if (bindRet < 0){
			//perror("bind failed");
			cout << "bind failed";
			exit(EXIT_FAILURE);
		}

		if (listen(server_fd, 3) < 0){
			perror("listen");
			exit(EXIT_FAILURE);
		}

		int new_socket = accept(server_fd, (struct sockaddr*) &clientAddress,
				(socklen_t*)(sizeof(clientAddress)));
		if (new_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		char buffer[1024];
		int valread = read(new_socket, buffer, 1024);
		printf("%s\n", buffer);
		send(new_socket, "okay", 4, 0);
		std::cout << "hello message sent\n";
		close(new_socket);
		shutdown(server_fd, SHUT_RDWR);
		return NULL;
}

void* client(void * in) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cout << "error in creating socket" << endl;
		exit(-100);
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(80);
	int connection = inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}

	stringstream ss;
	//ss << "message";

	ss << "FROM CLIENT";
	//cout << ss.str();
	int client_fd = connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (client_fd < 0)
	{
		perror("client:: connection failed");
		exit(1);
	}

	string theMessage = ss.str();
	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);
	std::cout << "message sent form client";
	char buffer[1024];
	int valread = read(sockfd, buffer, 1024);
	printf("%s\n", buffer);

	//write to the file chunk by chunk >> may loop
	close(connection);

	return NULL;
}

string readFile(string filePath) {
	ifstream fileToBeSent(filePath, ios::binary);
	ostringstream oss;

	oss << fileToBeSent.rdbuf();
	//cout << "string read from the file :: " + oss.str();

	return oss.str();
}

string getlastNameOfTheFile(string filePath){
	// gets name with extension
	string toRet = "";
	for (int i = filePath.length() - 1; i >= 0; i--){
		if (filePath[i] == '/')
			break;
		else
			toRet = filePath[i]+ toRet;

	}
	cout << toRet<<endl;
	return toRet;
}

void writeFile(string filePath, string data){
	ofstream file(getlastNameOfTheFile(filePath));
	file << data;
	file.close();

}
int main(int argc, char * argv[]) {


// argc by default = 1
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cout << "error in creating socket" << endl;
		exit(-100);
	}
	/*serverAddress.sin_family = AF_INET;
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





/*	string line;

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
				 cout << temp << endl;
				 temp = "";
			 }
		 }
		 if (temp != ""){
			 command[i] = temp;
			 cout << temp << endl;
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

	post("jksdflfjs/fskjl", "jslfkjsl.cjlkdjfsl.kljfsdl", 9);*/


//	cout << readFile("/home/ubuntu/Desktop/download.jpeg");
	//cout << getlastNameOfTheFile("/home/ubuntu/Desktop/download.jpeg");
	writeFile("/home/ubuntu/Desktop/download.jpeg",readFile("/home/ubuntu/Desktop/download.jpeg"));
	return 0;
}








