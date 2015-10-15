# ThreadPool

ThreadPool is an implementation of a thread pool in C++. It includes a thread pool class as well as a example usage class that times the execution between using the thread pool and not using one. 

This project files are for Visual Studio 2013 and the project requires C++ 11 because of Lambda function usage. Project files were built targeting Windows 64-bit computers. 32-bit and 64-bit executables of the project are included in zip files.

Note: this code is not intended to be the most optimized version of a thread pool implementation, but rather an implementation without dependencies on other major systems. One optimization that would improve performance would be utilizing a memory manager to execute threads on already allocated memory rather than allocating on the fly.
