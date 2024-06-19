#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>

using namespace std;

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <program> <strategy>" << endl;
        return 1;
    }

    int pipeIn[2];
    int pipeOut[2];
    pid_t pid;

    string programName;
    string strategy;
    int strlength = strlen(argv[2]);//we need to copy the string to a new char array to use strtok
    char *temp = new char[strlength];
    strcpy(temp, argv[2]);//copy the string we recived in the argv to a new char array
    programName = strtok(temp, " ");//get the first word in the string
    cout << programName << endl;
    strategy = strtok(NULL, " ");//get the second word in the string, using NULL to continue from the last strtok
    cout << strategy << endl;

    if (pipe(pipeIn) == -1) {
        perror("pipeIn");
        return 1;
    }
    if (pipe(pipeOut) == -1) {
        perror("pipeOut");
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        close(pipeIn[1]); // Close write end of pipeIn
        close(pipeOut[0]); // Close read end of pipeOut

        dup2(pipeIn[0], STDIN_FILENO); // Redirect stdin to pipeIn read end
        dup2(STDOUT_FILENO, pipeOut[1]); // Redirect stdout to pipeOut write end

        char *args[] = {strdup(programName.c_str()), strdup(strategy.c_str()), NULL};
        execv(args[0], args);
        perror("execv");
        return 1;
    } else { // Parent process
        close(pipeIn[0]); // Close read end of pipeIn
        close(pipeOut[1]); // Close write end of pipeOut

        string input;
        char buffer[256];

        while (true) {
            cin >> input;
            input += '\n'; // Add newline for the game input
            write(pipeIn[1], input.c_str(), input.length()); // Write input to pipeIn

            ssize_t bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1); // Read from pipeOut
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; // Null-terminate the string
                cout << buffer; // Output the game's response
            }
        }
    }
    return 0;
}
