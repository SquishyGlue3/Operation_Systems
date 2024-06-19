#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
using namespace std;
void add2PB(const char* name, const char* number)
{
    int fd[2];
    pid_t pid;
    string nameAndNumber = string(name) + " " + string(number) + "\n";

    if (pipe(fd) < 0)
    {
        perror("Error creating pipe");
        return;
    }
    pid = fork();
    if (pid < 0)
    {
        perror("Error forking");
        return;
    }
    if (pid == 0)
    {
        close (fd[1]);
        dup2(fd[0], STDIN_FILENO);//redirecting stdin to read from pipe
        close(fd[0]);
        execlp("sh", "sh", "-c", "cat >> phonebook.txt", NULL);//executing the command (cat >> phonebook.txt, -c is used to run the command in a new shell)
        perror("execlp");//execlp only returns if an error occurs
        return;
    }
    else{
        close(fd[0]);
        write(fd[1], nameAndNumber.c_str(), nameAndNumber.length());
        close(fd[1]);

        waitpid(pid, NULL, 0);
    }
}
int main()
{
    add2PB("John", "123456789");
    add2PB("Jane", "987654321");
    add2PB("Alice", "123123123");
    return 0;
}