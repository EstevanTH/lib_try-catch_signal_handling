# Library: `try` ... `catch` signal handling
Convert OS-level signals (or CPU exceptions) into catchable C++ software exceptions

**:dart: Beta stage:** This library is almost untested. Try it hard, fix it, tell me your experiments!

## Introduction:
This is my first library in C++. As a beginner I would like to meet other C++ developers and fix bugs.

By default, exceptions coming from CPU are not catch-able. This includes *memory access violation* (`SIGSEGV`), *illegal mathematical operation* (`SIGFPE`) and *illegal instruction* (`SIGILL`). They are signaled by the operating system and are asynchronous. When they happen, your program crashes.

The C++ language provides the function `std::signal()` which allows to change the behavior when such signals are triggered. By using it, you can customize what happens. You can choose between **calling a handling function** (similar to interrupt), **ignoring the signal** (worthless if the signal is a CPU exception) or **going back to the default behavior** (simple crash). If you choose to call a function but do nothing special with it, the program will crash.

*Well, I combined all of this to make a small library with a lightweight syntax, based on the well-known `try`/`catch` structure.*

With this library, you can make safer multithreaded applications (safely closing the thread instead of crashing application). You can save some data before closing (and avoid lost work). You can even continue the execution if you really know what you are doing!

## Usage:
#### 1- Include the header of the library:
```cpp
#include "TryCatchSignalHandler.h"
```
#### 2- Add this line at the top of the `main()` function in `main.cpp`
```cpp
TryCatchSignalHandler::catchSignalsCpu();
```
If you want to protect your program for all default signals instead of just CPU exceptions, use instead
```cpp
TryCatchSignalHandler::catchSignalsAll();
```
#### 3- Every time you want to protect a piece of code, replace your usual
```cpp
try{
	int* ptr = 0;
	std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access
}
catch( std::exception& e ){
	std::cerr << "Exception " << e.what() << " in " << __FILE__ << ':' << __LINE__ << std::endl
}
```
with the new structure
```cpp
TRY( identifier ){
	int* ptr = 0;
	std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access
}END_TRY( identifier )
catch( std::exception& e ){
	std::cerr << "Exception " << e.what() << " in " << __FILE__ << ':' << __LINE__ << std::endl
}
FINISH_TRY( identifier );
```
where `identifier` can contain digits, letters and underscores. The `identifier` must be unique in a function, as enforced by the C++ parser.

## Compatibility:
- I tested my library successfully with **MinGW 5.3**.
- **GCC** compilers will probably work because **MinGW** does.
- I tried **MSVC2013 (Visual C++ 12.0)**. `SIGSEGV` signals seem to be handled properly while `SIGFPE` signals crash the program without calling the handling function.

I could not test real `SIGILL` signals because they require to assembly-edit the binary program to replace an instruction with a non-existing one.

## Warnings:
- As a beginner in C++, I cannot say if this code is reliable, but I wish to help any of you.
- Execution continues in the catch block after being interrupted by an exception (possibly in external libraries or so). This is different than usual *try/catch* because exceptions are not thrown by C++ mechanism: this can lead to memory leaks and unpredictable behavior!
- *Reminder:* Usage of the usual *try/catch* syntax requires you to be aware that objects & data manually instantiated in the `TRY` sequence (using `new`) must be freed even when an exception / a signal happened before the matching `delete` within that sequence.

## Notes:
- Simulated signals (eg. triggered with `std::raise()`) do not require a jump in order to continue. On the contrary when real signals happen, the instruction pointer seems to retry the same instruction forever if the signal is ignored.
