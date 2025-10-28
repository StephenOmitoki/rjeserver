/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
// rje_server.cpp
#define _XOPEN_SOURCE
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unordered_map>

using namespace std;
using COMMAND = vector<string>;

const char *myfifo = "/tmp/rje_fifo";
const char *passwd_file = "passwd.txt";

unordered_map<string, string> users;

// Load user database from file
bool load_user_db() {
	ifstream file(passwd_file);
	if (!file.is_open()) {
		cerr << "Error: Cannot open password file: " << passwd_file << endl;
		return false;
	}

	string line;
	while (getline(file, line)) {
		size_t pos = line.find(':');
		if (pos != string::npos) {
			string username = line.substr(0, pos);
			string hash = line.substr(pos + 1);
			users[username] = hash;
		}
	}
	file.close();
	return true;
}

// Parse command from FIFO
COMMAND parseCmd() {
	int fd1;
	char str1[512];
	COMMAND request;

	fd1 = open(myfifo, O_RDONLY);
	if (fd1 == -1) {
		perror("open read");
		return request;
	}

	int nbytes = read(fd1, str1, sizeof(str1)-1);
	if (nbytes > 0) {
		str1[nbytes] = '\0';
		string s(str1), token;
		stringstream sstr(s);
		while (getline(sstr, token, ' ')) {
			request.push_back(token);
		}
	}
	close(fd1);
	return request;
}

// Send response through FIFO
void Respond(const string& response) {
	int fd1 = open(myfifo, O_WRONLY);
	if (fd1 == -1) {
		perror("open write");
		return;
	}
	write(fd1, response.c_str(), response.length() + 1);
	close(fd1);
}

// Authenticate user
bool authenticate_user(const string& username, const string& password_hash) {
	auto it = users.find(username);
	if (it == users.end()) {
		return false;
	}

	// Verify password using crypt
	char *verify_hash = crypt(password_hash.c_str(), it->second.c_str());
	if (!verify_hash) {
		return false;
	}

	return string(verify_hash) == it->second;
}

// Execute command
bool execute_command(const COMMAND& cmd, pid_t& pid) {
	if (cmd.size() < 2 || cmd[0] != "EXEC") {
		return false;
	}

	pid = fork();
	if (pid == 0) {
		// Child process
		vector<char*> args;
		for (size_t i = 1; i < cmd.size(); i++) {
			args.push_back(const_cast<char*>(cmd[i].c_str()));
		}
		args.push_back(nullptr);

		execvp(args[0], args.data());

		// If execvp returns, there was an error
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		// Parent process - don't wait for child to complete
		return true;
	} else {
		// Fork failed
		return false;
	}
}

int main() {
	cout << "RJE Server starting..." << endl;

	if (!load_user_db()) {
		cerr << "Failed to load user database. Exiting." << endl;
		return 1;
	}

	cout << "User database loaded with " << users.size() << " users." << endl;

	while (true) {
		cout << "Waiting for client connection..." << endl;

		// Create FIFO
		if (mkfifo(myfifo, 0666) == -1) {
			if (errno != EEXIST) {
				perror("mkfifo");
				return 1;
			}
		}

		// Parse client command
		COMMAND request = parseCmd();

		if (request.empty()) {
			unlink(myfifo);
			continue;
		}

		// Handle USER command
		if (request[0] == "USER" && request.size() >= 3) {
			string username = request[1];
			string password_hash = request[2];

			if (authenticate_user(username, password_hash)) {
				Respond("+ACCOUNT VALID");
				cout << "User " << username << " authenticated successfully." << endl;

				// Wait for EXEC command
				COMMAND exec_cmd = parseCmd();

				if (!exec_cmd.empty() && exec_cmd[0] == "EXEC") {
					pid_t pid;
					if (execute_command(exec_cmd, pid)) {
						string response = "+EXEC SUCCESSFUL PID " + to_string(pid);
						Respond(response);
						cout << "Executed command with PID: " << pid << endl;
					} else {
						Respond("-EXEC FAILED");
						cout << "Failed to execute command." << endl;
					}
				} else {
					Respond("-INVALID COMMAND");
				}
			} else {
				Respond("-INVALID ACCOUNT.");
				Respond("+GOODBYE");
				cout << "Authentication failed for user: " << username << endl;
			}
		} else {
			Respond("-INVALID REQUEST");
		}

		// Clean up FIFO
		unlink(myfifo);
	}

	return 0;
}