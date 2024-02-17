# Remove-Duplicate-Files
A simple program that removes duplicate files in a given directory by calculating the SHA-256 sum of every file in the directory.

# How to use?
```
./removeDuplicates <absolute path to a directory>
```

# Prerequisites
1. **Linux** (The program works only on Linux and maybe some BSD systems as it uses the d_type field of the dirent structre defined in <dirent.h> as only the d_name and d_ino fields are mandated by POSIX)
2. **C++20** (The program needs to be compiled with a compiler that supports C++ 20 as it uses the contains method of std::unordered_set)
3. **/bin/sha256sum** (Your linux distro needs to have the sha256sum binary as the program uses execlp(3) to execute /bin/sha256sum)

# Compiling
## Method 1 (requires CMake)
Navigate to the directory where CMakeLists.txt and main.cpp are and exectue:
```
cmake .
```
```
make
```
## Method 2 (Doesn't require CMake)
Navigate to the directory where main.cpp is and exectue:
```
g++ -std=c++20 main.cpp -o removeDuplicates
```

# What if I'm using Windows?
You can install wsl and run the program in wsl.

