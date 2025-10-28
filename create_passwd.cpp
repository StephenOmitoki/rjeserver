// create_passwd.cpp
#define _XOPEN_SOURCE
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

int main() {
    ofstream file("passwd.txt");
    if (!file.is_open()) {
        cerr << "Error: Cannot create passwd.txt" << endl;
        return 1;
    }
    
    // Create some sample users
    const char *salt = "$6$myS4ltStr1ng$";
    
    // User 1: alice/password123
    char *hash1 = crypt("password123", salt);
    file << "alice:" << hash1 << endl;
    
    // User 2: bob/secret456
    char *hash2 = crypt("secret456", salt);
    file << "bob:" << hash2 << endl;
    
    // User 3: charlie/hello789
    char *hash3 = crypt("hello789", salt);
    file << "charlie:" << hash3 << endl;
    
    file.close();
    cout << "Password file created with 3 sample users:" << endl;
    cout << "alice:password123" << endl;
    cout << "bob:secret456" << endl;
    cout << "charlie:hello789" << endl;
    
    return 0;
}