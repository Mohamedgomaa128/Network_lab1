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


char buffer[1024];


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

	//ss << "GET " << filePath << " HTTP/1.1"
		//	<< "\r\n\r\n";
	//cout << ss.str();

	ss << "GET" << filePath;
	string theMessage = ss.str();
	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);
	int readd = 100;
	string mainString;

	while (readd != 0){
		// reading the file from the server and writing it into new file with the same extension
		readd = read(sockfd, buffer, 1024);
		cout << readd << endl;
		// appending to main string which will be cleaned form unwanted data and written to the file
		mainString += buffer;

		cout << "mainSting now : " << mainString << endl;

		if (readd != 100){
			cout << "error in reading" << endl;
			exit(-100);
		}
	}

	size_t found = mainString.find("NOT FOUND");
	if (found != string::npos){
		perror("file not found");
		exit(-1);
	}
	else{
		string sub = "HTTP/1.1 200 OK GET\\r\\n \n";
		found = mainString.find(sub);
		mainString.erase(found, sub.length());
		writeFile(filePath, mainString);
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

	if (connection < 0){
		cout << "connection failed" << endl;
		exit(-100);
	}

	// reading needed file
	ostringstream oss;
	oss << "POST " << filePath << " ";//<< " HTTP/1.1" << "\r\n";

	//cout << oss.str();

	oss << readFile(filePath);
	//oss << "\r\n\r\n";
	cout << oss.str();

	//stringstream ss;
	//ss << "message";
	string theMessage = oss.str();
	//int readed = read(sockfd, buffer, 100);


	int ret = send(sockfd, theMessage.c_str(), theMessage.length(), 0);

	cout << ret;
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




	string line;

	ifstream commandFile;
	string command[argc - 1];
	//cout << commandFile.is_open()<< endl;
	commandFile.open("command.txt", ios::out);
	//cout << commandFile.is_open()<< endl;


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
				// cout << temp << endl;
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


	return 0;
}








