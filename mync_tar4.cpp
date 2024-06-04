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
#include <fcntl.h>

using namespace std;

static struct sockaddr_in client_addr; // UDP client address we need those to send data to the client because we do not have direct connection to the client
static struct sockaddr_in serv_addr;   // UDP server address we need those to send data to the server because we do not have direct connection to the server(if we send from server to server)

// Function to start a TCP server and listen for input
int startTcpServer(int port)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    cout << "Creating socket..." << endl;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // create socket
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    cout << "Setting socket options..." << endl;
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

    cout << "Setting socket options..." << endl;
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

        cout << "Executing program: " << program << endl;
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

            int valread = read(pipeOut[0], buffer, sizeof(buffer));
            if (valread == 0)
            {
                break; // EOF
            }
            cout << "Read " << valread << " bytes from pipeOut. Writing to output_fd..." << endl;
            write(output_fd, buffer, valread);
        }

        close(pipeOut[0]);
    }
}

int main(int argc, char *argv[])
{
    bool inputFlag = false;
    bool outputFlag = false;
    bool executeFlag = false;
    bool timeoutFlag = false;
    // int serverType = -1, clientType = -1;
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
            if (pos != string::npos) // If there is a space in the command, split it into program name and arguments
            {
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
    if (inputFlag) // if there is an input flag
    {
        if (inputMethod.rfind("TCPS", 0) == 0) // if the input method is TCP server
        {
            int port = stoi(inputMethod.substr(4));
            cout << "Starting TCP server on port " << port << endl;
            input_fd = startTcpServer(port);
            cout << "TCP server started and client connected." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        else if (inputMethod.rfind("TCPC", 0) == 0) // if the input method is TCP client
        {
            string host = inputMethod.substr(4, inputMethod.find(',') - 4);
            int port = stoi(inputMethod.substr(inputMethod.find(',') + 1));
            cout << "Connecting to TCP server at " << host << " on port " << port << endl;
            input_fd = startTcpClient(host, port);
            cout << "Connected to TCP server." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the client socket
        }
        else if (inputMethod.rfind("UDPS", 0) == 0) // if the input method is UDP server
        {
            int port = stoi(inputMethod.substr(4));
            cout << "Starting UDP server on port " << port << endl;
            if (timeoutFlag) // receiveing timeout value to kill the server after the timeout
            {
                input_fd = startUdpServer(port, timeout);
            }
            else
            {
                input_fd = startUdpServer(port, 0);
            }
            cout << "UDP server started and client connected." << endl;
            dup2(input_fd, STDIN_FILENO); // Redirect standard input to the server socket
        }
        // there is no need to support UDP client as an input

        // else if (inputMethod.rfind("UDPC", 0) == 0)
        // {
        //     string host = inputMethod.substr(4, inputMethod.find(',') - 4);
        //     int port = stoi(inputMethod.substr(inputMethod.find(',') + 1));
        //     cout << "Connecting to UDP server at " << host << " on port " << port << endl;
        //     input_fd = startUdpClient(host, port);
        //     cout << "Connected to UDP server." << endl;
        //     dup2(input_fd, STDIN_FILENO); // Redirect standard input to the client socket
        // }
    }
    if (outputFlag)
    {
        // Set up the output connection
        if (outputMethod.rfind("TCPS", 0) == 0) // if the output method is TCP server
        {
            int port = stoi(outputMethod.substr(4));
            cout << "Starting TCP server on port " << port << endl;
            output_fd = startTcpServer(port);
            cout << "TCP server started and client connected." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the server socket
        }
        else if (outputMethod.rfind("TCPC", 0) == 0) // if the output method is TCP client
        {
            string host = outputMethod.substr(4, outputMethod.find(',') - 4);
            int port = stoi(outputMethod.substr(outputMethod.find(',') + 1));
            cout << "Connecting to TCP server at " << host << " on port " << port << endl;
            output_fd = startTcpClient(host, port);
            cout << "Connected to TCP server." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
        // there is no need to support UDP server as an output

        // else if (outputMethod.rfind("UDPS", 0) == 0)// if the output method is UDP server
        // {
        //     int port = stoi(outputMethod.substr(4));
        //     cout << "Starting UDP server on port " << port << endl;
        //     output_fd = startUdpServer(port);
        //     cout << "UDP server started and client connected." << endl;
        //     dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the server socket
        // }
        else if (outputMethod.rfind("UDPC", 0) == 0) // if the output method is UDP client
        {
            string host = outputMethod.substr(4, outputMethod.find(',') - 4); // get host name from the output method by taking the substring from 4 to the first comma minus 4
            int port = stoi(outputMethod.substr(outputMethod.find(',') + 1)); // get port number from the output method by taking the substring from the first comma plus 1 to the end of the string
            cout << "Connecting to UDP server at " << host << " on port " << port << endl;
            output_fd = startUdpClient(host, port);
            cout << "Connected to UDP server." << endl;
            dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the client socket
        }
    }

    if (executeFlag)
    {
        executeCommand(programName, commandArgs, output_fd);
    }
    else if (inputFlag || outputFlag)
    {
        // Transfer data from input_fd to output_fd
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
