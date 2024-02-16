#include <iostream>
#include <unordered_set>
#include <dirent.h>
#include <wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    const char* workingDirectory{argv[1]};
    chdir(workingDirectory);
    char currentHash[65]{};
    std::unordered_set<std::string> hashes;
    DIR* directoryStream{opendir(workingDirectory)};
    dirent* currentFile{readdir(directoryStream)};
    while (currentFile != NULL)
    {
        if (currentFile->d_type != DT_DIR)
        {
            int pipeOuter[2]{};
            pipe(pipeOuter);
            int pid{fork()};
            if (pid > 0)
            {
                int status;
                wait(&status);
                read(pipeOuter[0], &currentHash, 64);
                if (!hashes.contains(currentHash))
                {
                    std::string hash;
                    for (int i{0}; i < 64; ++i)
                    {
                        hash.push_back(currentHash[i]);
                    }
                    hashes.emplace(hash);
                }
                else
                {
                    int pid2{fork()};
                    if (pid2 > 0)
                    {
                        int status2;
                        wait(&status2);
                    }
                    else if (pid2 == 0)
                    {
                        execlp("rm", "rm", currentFile->d_name, NULL);
                    }
                    else
                    {
                        perror("Couldn't fork!");
                        exit(-1);
                    }
                }

            }
            else if (pid == 0)
            {
                int pipeInner[2];
                pipe(pipeInner);
                int pid2{fork()};
                if (pid2 > 0)
                {
                    close(pipeInner[1]);
                    close(pipeOuter[0]);
                    dup2(pipeInner[0], 0);
                    dup2(pipeOuter[1], 1);
                    int status2;
                    wait(&status2);
                    execlp("cut", "cut", "-d", " ", "-f", "1", NULL);
                }
                else if (pid2 == 0)
                {
                    close(pipeInner[0]);
                    dup2(pipeInner[1], 1);
                    execlp("sha256sum", "sha256sum", currentFile->d_name, NULL);
                }
                else
                {
                    perror("Couldn't fork!");
                    exit(-1);
                }
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
