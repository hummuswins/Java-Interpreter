# Java Interpreter
This is a Java Interpreter project for school. I will be updating my codebase through Github for version control across multiple platform.
## Installation guide
Before doing anything, create a build directory folder in your system (from now on, I will be using BASH cli language)
```bash
mkdir build
cd build
```

Make sure CMake is installed before you continue on to this step using either your system package manger or downloading the source and build it (https://cmake.org/download/).
Now run the following cmake command
```bash
cmake ..
make
```
Now the executeable will either be "JavaIntepreter" or "JavaIntepreter.exe" depending on your operating system. Now run it with the following command
```bash
JavaInterpreter <test-class-file>
```
substitute test-class files with any of the test files in the test-cases and the program will start working. (Notes, I am only at the Parsing stage currently so the code doesn't do anything orther than parsing the class file).
## Implementation Update
Currently finishing up stage 1 of implementing a class Parser following the guideline of https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.6.
