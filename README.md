# TwsApiCpp

TwsApiC++ is availble freely since 2008 in [Yahoo's Interactive Brokers TWS APIDiscussion Groups](https://groups.yahoo.com/neo/groups/TWSAPI/files/C%2B%2B%20Code/TwsApiC%2B%2B%20Directory/). Versions 9.71 and up will be available only via this Github.

The goal of TwsApiC++ is to hide all the specific 'IB POSIX' api stuff for the application programmer while
* no limitation on the use of all features offered in the IB api
* adding safety, robustness and easy of use 

See the [wiki history](https://github.com/JanBoonen/TwsApiCpp/wiki/Home---History) page for a more lengthy history and why it was created.

## Compiling and execution
This project includes all necessary IB POSIX C++ source files, no need to install the IB api first.

TwsApiC++ has been installed and compiled on many different platforms. Unix/linux make files and Visual Studio project files are included.
* directory TwsApiC++ contains the specific non IB sources code for this library
* directory source/PosixClient contains the copy of the IB POSIX C++ sources.
* sub-directories _cygwin, _linux and _win32 contain the make/project files respectivey
It should normaly compile 'out of the box' and the compiled libraries are written into TwsApiCpp/TwsApiC++/Api/ and can be linked from there.

The footprint of the library is small and fast. 

## Examples programs
There are some examples programs included in the [TwsApiC++/Test/Src](https://github.com/JanBoonen/TwsApiCpp/tree/master/TwsApiC%2B%2B/Test/Src) directory.


## Features - what makes the use of TwsApiC++ worthwile

###Ease of use
* It reinstates the **EClient** and **EWrapper** in combination with the other classes (Contract, Order, Execution, etc) in the **Shared** directory as the interface to all the functionality offered by IB


  - No need to have knowledge about the POSIX specific classes in source/PosixClient/src and the socket low level programming introduced in version 9.62. See the [wiki history](https://github.com/JanBoonen/TwsApiCpp/wiki/Home---History) page for a more lengthy explanation.

  - **Resolves all POSIX socket connection related issues left to the programmer**, i.e., it ensures no data is stuck in the internal buffer of the IB api when the first attempt to send it to the TWS fails for some reason. Otherwise, it would sit there until the next EClient call is executed.

* Including just one file, TwsApiL0.h,  is enough to have access to the full functionality

* It implements an default method for every EWrapper method. During debugging, these methods print a warning message when called to warn you that maybe an implementation is needed in the trading system! You can switch it off.

* It provides a **non-blocking EClient::checkMessages()** call which is safe to call in an endless loop without cpu usage penalty when idle: cpu usage less then 1% when no incoming data

  See the example [Clients.cpp](https://github.com/JanBoonen/TwsApiCpp/blob/master/TwsApiC++/Test/Src/Clients.cpp?ts=4) that makes 8 connections simultaneously and calls each of the 8 checkMessages() in a loop.
  
* It implements the **EReader** functionality as found in the Java api and in the MS Windows based version of the C++ api

  - By default this EReader runs in a separate thread and sits waiting for incoming data and processes it without any delay.

  - **Can be switched off** simply by passing a parameter when instantiating the EWrapper class. Of course, the user must call the non-blocking EClient::checkMessages() to check for incoming events (data) send by the TWS. See [Clients.cpp](https://github.com/JanBoonen/TwsApiCpp/blob/master/TwsApiC++/Test/Src/Clients.cpp?ts=4)

* It provides a way to check the correct spelling of the many textual or numeric parameters at compile time instead of runtime. This can reduce the test effort considerably, or even bring up hidden errors only discovered until a rare situation occurs. See [TwsApiDefs.h](https://github.com/JanBoonen/TwsApiCpp/blob/master/TwsApiC++/Api/TwsApiDefs.h?ts=4).

  A quick example:
```C++
  // Just imagine you wrote
  if( key == "LookaheadAvailableFunds" ) { ... } // hmm, will never be executed
  // => Correct spelling is
  if( key == "LookAheadAvailableFunds" ) { ... }
  
  // you can write 
  if( UpdateAccountValueKey(key) == UpdateAccountValueKey::LookAheadAvailableFunds ) { ... }
  //  or
  switch( UpdateAccountValueKey(key) ) {
    case UpdateAccountValueKey::LookAheadAvailableFunds:
    break;
    ...
    case UpdateAccountValueKey::_INVALID_:  // key was not recognised
    break;
    
    default:
    break;
  }

  // and the following would raise a compiling error
  if( UpdateAccountValueKey(key) == UpdateAccountValueKey::LookaheadAvailableFunds ) { ... }
```

###Safety, Robustness and stability
* It **protects** the inner workings of **the IB library against exceptions** thrown inadvertently from within the user code in the derived EWrapper methods. And as such, you don't need to put the code in a try/catch block yourself.

  EWrapper is extended with a method onCatch() which the library calls when such exceptions occurs with information where it was triggered.

* It **protects** the inner workings of **the IB library against concurrent use of its internal data**, which could lead to loss of requests send to the TWS.

  This can happen when the EClient is called from different threads, i.e. the automatic ‘EReader’ thread is used and both the main thread and the EWrapper call methods of EClient.

* **Resolves all POSIX socket connection related issues left to the programmer**, i.e., it ensures no data is stuck in the internal buffer of the IB api when the first attempt to send it to the TWS fails for some reason. Otherwise, it would sit there until the next EClient call is executed.

* It is **by design a ‘closed’ library**, meaning you cannot derive from its EClient class directly and overwrites its methods.

  This will ensure the integrity of the whole safety measures build into this library.

* It garantees the EWRapper::connectionClosed() is called when the connection is closed for any reason, on purpose or not.

  It sends every second a ‘heartbeat’ to check the connection is still alive. This ensures the closing of the connection is detected in case something went wrong leaving the connection in an undefined status

  Note: There is no automatic reconnection
  
