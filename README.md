Stephen Omitoki
Overview:

This program demonstrates a simple Remote Job Entry (RJE) system using inter-process communication (IPC) with a named pipe (FIFO).
The system consists of three parts:

A server that authenticates users and executes commands.

A client that sends login credentials and job requests to the server.

A password generator that creates the user database file.

The client sends a username, password, and command to the server.
The server verifies the credentials using the passwd.txt file and, if valid, executes the requested program.

How to run:
Step 1: Compile all programs

g++ create_passwd.cpp -o create_passwd -lcrypt
g++ rje_server.cpp -o rje_server
g++ rje_client.cpp -o rje_client -lcrypt

Step 2: Create the password file

./create_passwd

This will create a file containing:
alice:password123
bob:secret456
charlie:hello789

Step 3: Start the server

In one terminal, run:

./rje_server

You should see:

RJE Server starting...
User database loaded with 3 users.
Waiting for client connection...

Step 4: Run the client with the worker program

In another terminal, connect to the server using:
./rje_client alice password123 ./worker 10 50 or
./rje_client bob secret456 ./worker 20 60 or
./rje_client charlie hello789 ./worker 5 30

You should see:
Server: +ACCOUNT VALID
Server: +EXEC SUCCESSFUL PID 1234 (what the pid is, different pids for the three users)

and in the server, you should see:

User alice autheticated successfully.
Executed command with PID : 1234
Waiting for client connection...
Primes: 11 13 17 19 23 29 31 37 41 43 47

If the login fails, you'll see:
Server: -INVALID ACCOUNT.
Server: +GOODBYE

Files:
rje_server.cpp
rje_client.cpp
worker.cpp
create_passwd.cpp
passwd.txt
README.txt


Requirements: Ubuntu 24.04.2 LTS
