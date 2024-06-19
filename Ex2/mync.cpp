#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <netdb.h>
#include <errno.h>
#include <sys/un.h>

using namespace std;

static struct sockaddr_in client_addr; // UDP client address we need those to send data to the client because we do not have direct connection to the client
static struct sockaddr_in serv_addr;   // UDP server address we need those to send data to the server because we do not have direct connection to the server(if we send from server to server)

// Function to start a TCP server and listen for input
int startTcpServer(int port)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    cout << "Creating socket..." << endl;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // create socket
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    cout << "Binding socket to port " << port << "..." << endl;
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // bind socket to port
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Listening on port " << port << "..." << endl;
    if (listen(server_fd, 3) < 0) // listen for connections on socket
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Accepting connection..." << endl;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) // accept connection on socket
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    cout << "Connection accepted." << endl;
    return new_socket;
}

// Function to start a TCP client and connect for output
int startTcpClient(const string &host, int port)
{
    struct sockaddr_in serv_addr;
    int sock = 0;
    struct hostent *he;

    cout << "Creating socket..." << endl;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    cout << "Resolving hostname " << host << "..." << endl;
    if ((he = gethostbyname(host.c_str())) == NULL) // get host info from hostname and store in he
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = *((struct in_addr *)he->h_addr); // get ip address from host info and store in serv_addr

    cout << "Connecting to " << host << " on port " << port << "..." << endl;
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect to server using socket
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connected to " << host << " on port " << port << "." << endl;
    return sock;
}

int startUdpServer(int port, int timeout)
{
    int server_fd;
    int addrlen = sizeof(serv_addr);

    cout << "Creating socket..." << endl;
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) // create socket
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    cout << "Binding socket to port " << port << "..." << endl;
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // bind socket to port
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Listening on port " << port << "..." << endl;
    alarm(timeout);
    return server_fd;
}

int startUdpClient(const string &host, int port)
{
    int sock = 0;
    struct hostent *he; // host entry structure to store host info

    cout << "Creating socket..." << endl;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) // create socket
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    cout << "Resolving hostname " << host << "..." << endl;
    if ((he = gethostbyname(host.c_str())) == NULL) // get host info from hostname and store in he
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr = *((struct in_addr *)he->h_addr); // get ip address from host info and store in client_addr
    return sock;
}

// Function to execute the command and redirect its output
void executeCommand(const string &program, const vector<string> &args, int output_fd)
{
    int pipeOut[2];
    pid_t pid;

    cout << "Creating pipes..." << endl;
    if (pipe(pipeOut) == -1)
    {
        perror("pipeOut");
        exit(EXIT_FAILURE);
    }

    cout << "Forking process..." << endl;
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // Child process
        cout << "In child process. Redirecting stdout..." << endl;
        close(pipeOut[0]);               // Close read end of pipeOut
        dup2(pipeOut[1], STDOUT_FILENO); // Redirect stdout to pipeOut write end

        // Prepare arguments for execv
        vector<char *> exec_args;
        exec_args.push_back(strdup(program.c_str()));
        for (const auto &arg : args)
        {
            exec_args.push_back(strdup(arg.c_str()));
        }
        exec_args.push_back(NULL);

        cout << "Executing program: " << program << " with arguments: ";
        for (const auto &arg : exec_args)
        {
            if (arg)
                cout << arg << " ";
        }
        cout << endl;

        execv(exec_args[0], exec_args.data());
        perror("execv");
        exit(EXIT_FAILURE);
    }
    else
    {                      // Parent process
        close(pipeOut[1]); // Close write end of pipeOut

        fd_set readfds;
        char buffer[1024];

        cout << "In parent process. Entering select loop..." << endl;
        while (true)
        {
            int valread = read(pipeOut[0], buffer, sizeof(buffer)); // read from the pipeOut and store the data in buffer
            if (valread == 0)                                       // if the read value is 0, that means the server and the client are done sending data
            {
                break; // EOF
            }
            cout << "Read " << valread << " bytes from pipeOut. Writing to output_fd..." << endl;
            write(output_fd, buffer, valread); // write the data to the output_fd to send it to the client
        }

        close(pipeOut[0]);
    }
}

// start UDS server datagram on recieved path
int startDatagramServer(const string &path)
{
    int server_fd;
    struct sockaddr_un serv_addr;
    unlink(path.c_str()); // unlink the path to avoid error if it is already exist
    cout << "Creating UDS datagram socket..." << endl;
    if ((server_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    { // create socket AF_UNIX as the type of the socket, sock_DGRAM as the type of the data, 0 as the protocol
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created." << endl;

    serv_addr.sun_family = AF_UNIX;                                            // set the family of the socket to AF_UNIX
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1); // copy the path to the socket address using the recieved path

    cout << "Binding socket to path " << path << "..." << endl;

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int startDatagramClient(const string &path)
{
    int sock;
    struct sockaddr_un client_addr;

    cout << "Creating UDS datagram socket..." << endl;
    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, path.c_str(), sizeof(client_addr.sun_path) - 1);

    return sock;
}

int startStreamServer(const string &path)
{
    int server_fd;
    struct sockaddr_un serv_addr;
    int addrlen = sizeof(serv_addr);

    cout << "Creating UDS stream socket..." << endl;
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    cout << "Binding socket to path " << path << "..." << endl;
    unlink(serv_addr.sun_path);
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    cout << "Listening on path " << path << "..." << endl;
    if (listen(server_fd, 3) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int new_socket;
    cout << "Accepting connection..." << endl;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&serv_addr, (socklen_t *)&addrlen)) == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cout << "Connection accepted." << endl;
    return new_socket;
}
int startStreamClient(const string &path)
{
    int sock;
    struct sockaddr_un serv_addr;

    cout << "Creating UDS stream socket..." << endl;
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, path.c_str(), sizeof(serv_addr.sun_path) - 1);

    cout << "Connecting to " << path << "..." << endl;
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    cout << "Connected to " << path << "." << endl;
    return sock;
}

int main(int argc, char *argv[])
{
    bool inputFlag = false;
    bool outputFlag = false;
    bool executeFlag = false;
    bool timeoutFlag = false;
    int timeout;
    string inputMethod, outputMethod, command, programName, programAddition;
    vector<string> commandArgs; // Vector to store the arguments of the command to be executed


    cout << "Parsing arguments..." << endl;
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "-e" && i + 1 < argc)
        {
            executeFlag = true;
            command = argv[++i];
            size_t pos = command.find(' ');
            if (pos != string::npos)
            { // If there is a space in the command, split it into program name and arguments
                programName = command.substr(0, pos);
                programAddition = command.substr(pos + 1);
                commandArgs.push_back(programAddition); // Add the arguments to the commandArgs vector
            }
            else
            {
                programName = command; // If there is no space, the whole command is the program name
            }
        }
        else if (arg == "-t" && i + 1 < argc)
        {
            timeoutFlag = true;
            timeout = stoi(argv[++i]);
            if (timeout < 0)
            {
                cout << "Invalid timeout value. Exiting." << endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (arg == "-i" && i + 1 < argc)
        {
            inputFlag = true; // there is an input flag
            inputMethod = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFlag = true; // there is an output flag
            outputMethod = argv[++i];
        }
    }

    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    // Set up the input connection
    if (inputFlag)
    { // if there is an input flag
        if (inputMethod.rfind("TCPS", 0) == 0)
        { // if the input method is TCP server
            int port = stoi(inputMethod.substr(4));
            cout << "Starting TCP server on port " << port << endl;
            input_fd = startTcpServer(port);
            cout << "TCP server started and client connected." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        else if (inputMethod.rfind("TCPC", 0) == 0)
        { // if the input method is TCP client
            string host = inputMethod.substr(4, inputMethod.find(',') - 4);
            int port = stoi(inputMethod.substr(inputMethod.find(',') + 1));
            cout << "Connecting to TCP server at " << host << " on port " << port << endl;
            input_fd = startTcpClient(host, port);
            cout << "Connected to TCP server." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the client socket
        }
        else if (inputMethod.rfind("UDPS", 0) == 0)
        { // if the input method is UDP server
            int port = stoi(inputMethod.substr(4));
            cout << "Starting UDP server on port " << port << endl;
            if (timeoutFlag)
            { // receiving timeout value to kill the server after the timeout
                input_fd = startUdpServer(port, timeout);
            }
            else
            {
                input_fd = startUdpServer(port, 0);
            }
            cout << "UDP server started and client connected." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        else if (inputMethod.rfind("UDSSD", 0) == 0)
        { // Unix domain sockets - server - datagram
            string path = inputMethod.substr(5);
            cout << "Starting UDS datagram server on path " << path << endl;
            input_fd = startDatagramServer(path);
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        else if (inputMethod.rfind("UDSSS", 0) == 0)
        { // Unix domain sockets - server - stream
            string path = inputMethod.substr(5);
            cout << "Starting UDS stream server on path " << path << endl;
            input_fd = startStreamServer(path);
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        else if (inputMethod.rfind("UDSCS", 0) == 0)
        { // Unix domain sockets - client - stream
            string path = inputMethod.substr(5);
            cout << "Connecting to UDS stream server at " << path << endl;
            input_fd = startStreamClient(path);
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the client socket
        }
    }

    if (outputFlag)
    {
        // Set up the output connection
        if (outputMethod.rfind("TCPS", 0) == 0)
        { // if the output method is TCP server
            int port = stoi(outputMethod.substr(4));
            cout << "Starting TCP server on port " << port << endl;
            output_fd = startTcpServer(port);
            cout << "TCP server started and client connected." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the server socket
        }
        else if (outputMethod.rfind("TCPC", 0) == 0)
        { // if the output method is TCP client
            string host = outputMethod.substr(4, outputMethod.find(',') - 4);
            int port = stoi(outputMethod.substr(outputMethod.find(',') + 1));
            cout << "Connecting to TCP server at " << host << " on port " << port << endl;
            output_fd = startTcpClient(host, port);
            cout << "Connected to TCP server." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
        else if (outputMethod.rfind("UDPC", 0) == 0)
        {                                                                     // if the output method is UDP client
            string host = outputMethod.substr(4, outputMethod.find(',') - 4); // get host name from the output method by taking the substring from 4 to the first comma minus 4
            int port = stoi(outputMethod.substr(outputMethod.find(',') + 1)); // get port number from the output method by taking the substring from the first comma plus 1 to the end of the string
            cout << "Connecting to UDP server at " << host << " on port " << port << endl;
            output_fd = startUdpClient(host, port);
            cout << "Connected to UDP server." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
        else if (outputMethod.rfind("UDSSS", 0) == 0)
        { // Unix domain sockets - server - stream
            string path = outputMethod.substr(5);
            cout << "Starting UDS stream server on path " << path << endl;
            output_fd = startStreamServer(path);
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the server socket
        }
        else if (outputMethod.rfind("UDSCD", 0) == 0)
        { // Unix domain sockets - cleint - datagram
            string path = outputMethod.substr(5);
            cout << "Connecting to UDS datagram server at " << path << endl;
            output_fd = startDatagramClient(path);
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
        else if (outputMethod.rfind("UDSCS", 0) == 0)
        { // Unix domain sockets - client - stream
            string path = outputMethod.substr(5);
            cout << "Connecting to UDS stream server at " << path << endl;
            output_fd = startStreamClient(path);
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
    }

    if (executeFlag)
    {
        executeCommand(programName, commandArgs, output_fd); // using different function(in our case, ttt program) to execute the command and redirect its output
    }
    else if (inputFlag || outputFlag)
    {
        // Transfer data from input_fd to output_fd without executing a program
        char buffer[1024];
        while (true)
        {
            ssize_t bytes_read = read(input_fd, buffer, sizeof(buffer));
            if (bytes_read <= 0)
            {
                break;
            }
            write(output_fd, buffer, bytes_read);
        }
    }
    else
    {
        cout << "Invalid or insufficient parameters provided. Exiting." << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}