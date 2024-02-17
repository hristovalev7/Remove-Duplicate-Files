#include <iostream>
#include <unordered_set>
#include <dirent.h>
#include <wait.h>
#include <unistd.h>

#define parent pid > 0
#define child pid == 0

void argumentCheck(int argc)
{
    if (argc != 2)
    {
        perror("Invalid number of arguments!");
        exit(-1);
    }
}

void removeFile(const char* fileName)
{
    int pid{fork()};
    if (parent)
    {
        int status;
        wait(&status);
    }
    else if (child)
    {
        execlp("rm", "rm", fileName, NULL);
    }
    else
    {
        perror("Couldn't fork!");
        exit(-1);
    }
}

void putHashInPipe(int pipeInner[2], int pipeOuter[2], const char* fileName)
{
    int pid{fork()};
    if (parent)
    {
        close(pipeInner[1]);
        close(pipeOuter[0]);
        dup2(pipeInner[0], 0);
        dup2(pipeOuter[1], 1);
        int status;
        wait(&status);
        execlp("cut", "cut", "-d", " ", "-f", "1", NULL);
    }
    else if (child)
    {
        close(pipeInner[0]);
        dup2(pipeInner[1], 1);
        execlp("sha256sum", "sha256sum", fileName, NULL);
    }
    else
    {
        perror("Couldn't fork!");
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    argumentCheck(argc);
    const char* workingDirectory{argv[1]};
    chdir(workingDirectory);
    char currentHash[65]{};
    std::unordered_set<std::string> hashes;
    DIR* directoryStream{opendir(workingDirectory)};
    dirent* currentFile{readdir(directoryStream)};
    while (currentFile != nullptr)
    {
        if (currentFile->d_type != DT_DIR)
        {
            int pipeOuter[2]{};
            pipe(pipeOuter);
            int pid{fork()};
            if (parent)
            {
                int status;
                wait(&status);
                read(pipeOuter[0], &currentHash, 64);
                if (hashes.contains(currentHash))
                {
                    removeFile(currentFile->d_name);
                }
                else
                {
                    hashes.emplace(currentHash);
                }
            }
            else if (child)
            {
                int pipeInner[2];
                pipe(pipeInner);
                putHashInPipe(pipeInner, pipeOuter, currentFile->d_name);
            }
            else
            {
                perror("Couldn't fork!");
                exit(-1);
            }
        }
        currentFile = readdir(directoryStream);
    }
    return 0;
}
