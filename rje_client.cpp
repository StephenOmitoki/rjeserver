// rje_client.cpp
#define _XOPEN_SOURCE
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

const char *myfifo = "/tmp/rje_fifo";

string read_response(int fd) {
    string response;
    char buffer[512];
    int bytes_read;
    
    do {
        bytes_read = read(fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            response += string(buffer);
        }
    } while (bytes_read == sizeof(buffer)-1);
    
    return response;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <username> <password> <command> [args...]" << endl;
        cerr << "Example: " << argv[0] << " alice password123 /bin/ls -l" << endl;
        return 1;
    }
    
    string username = argv[1];
    string password = argv[2];
    
    // Create the command string for execution
    string exec_cmd = "EXEC";
    for (int i = 3; i < argc; i++) {
        exec_cmd += " " + string(argv[i]);
    }
    
    // Hash the password (using the same salt format as in create_passwd)
    const char *salt = "$6$myS4ltStr1ng$";
    char *hash = crypt(password.c_str(), salt);
    if (!hash) {
        perror("crypt");
        return 1;
    }
    
    string user_cmd = "USER " + username + " " + string(hash);
    
    // Send USER command
    int fd = open(myfifo, O_WRONLY);
    if (fd == -1) {
        perror("open write");
        return 1;
    }
    write(fd, user_cmd.c_str(), user_cmd.length() + 1);
    close(fd);
    
    // Read authentication response
    fd = open(myfifo, O_RDONLY);
    if (fd == -1) {
        perror("open read");
        return 1;
    }
    
    string response = read_response(fd);
    close(fd);
    
    cout << "Server: " << response << endl;
    
    if (response == "+ACCOUNT VALID") {
        // Send EXEC command
        fd = open(myfifo, O_WRONLY);
        if (fd == -1) {
            perror("open write");
            return 1;
        }
        write(fd, exec_cmd.c_str(), exec_cmd.length() + 1);
        close(fd);
        
        // Read execution response
        fd = open(myfifo, O_RDONLY);
        if (fd == -1) {
            perror("open read");
            return 1;
        }
        
        response = read_response(fd);
        close(fd);
        cout << "Server: " << response << endl;
    }
    
    return 0;
}