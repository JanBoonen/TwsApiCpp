# TwsApiCpp

TwsApiC++ is a library build on top of the IB POSIX C++ library. It's goal is to make programming with this api more easy without hiding any functionality while at the same time it protects the programmer from hard to discover pitfalls hidden in the IB code.

TwsApiC++ is availble freely since 2008 in [Yahoo's Interactive Brokers TWS APIDiscussion Groups](https://groups.yahoo.com/neo/groups/TWSAPI/files/C%2B%2B%20Code/TwsApiC%2B%2B%20Directory/). Versions 9.71 and up will be available only via this Github.

See the [wiki history](https://github.com/JanBoonen/TwsApiCpp/wiki/History) page for a more lengthy history and why it was created.

TwsApiC++ is a ‘closed’ library. You cannot derive from its EClient class directly and overwrites its methods. This is on purpose to ensure it's integrity (and it's build in safety measures) as a whole. And why would you? Your trading system **is not** an EClient but **uses** the EClient to achieve it's goal. It could use in fact any other broker api besides IB.


## Compiling and execution
This project includes all necessary IB POSIX C++ source files, no need to install the IB api first.

TwsApiC++ has been installed and compiled on many different platforms. Unix/linux make files and Visual Studio project files are included.
* directory TwsApiC++ contains the specific non IB sources code for this library
* directory source/PosixClient contains the copy of the IB POSIX C++ sources.
* sub-directories _cygwin, _linux and _win32 contain the make/project files respectivey
It should normaly compile 'out of the box' and the compiled libraries are written into TwsApiCpp/TwsApiC++/Api/ and can be linked from there.

The footprint of the library is small and fast. 

## Examples programs
There are some examples programs included in the delivery in the [TwsApiC++/Test/Src](https://github.com/JanBoonen/TwsApiCpp/tree/master/TwsApiC%2B%2B/Test/Src) directory.

An simple example that demonstrates some strengths of the library is [Retrieve History](https://github.com/JanBoonen/TwsApiCpp/wiki/Example---Retrieve-History) in the wiki pages.

## What makes the use of TwsApiC++ worthwile

* **Easy access to ful functionality**

  Including **TwsApiL0.h** is all it takes to have access to the full functionality.

* **No knowledge of sockets programming required**

  It reinstates the **EClient** and **EWrapper** in combination with the other classes (Contract, Order, Execution, etc) in the **[Shared](https://github.com/JanBoonen/TwsApiCpp/tree/master/source/PosixClient/Shared)** directory as the interface to all the functionality offered by IB
  
  To achieve that, TwsApiC++ takes care of the specific POSIX classes in source/PosixClient/src and these socket low level programming features one should not bothered with. These were inroduced in version 9.62. See the [wiki history](https://github.com/JanBoonen/TwsApiCpp/wiki/Home---History) page for a more lengthy explanation.
  
  
* **Resolves socket connection related issues left to the programmer in the IB POSIX implementation**

  TwsApiC++ ensures no data is stuck in the internal buffer of the IB api when the first attempt to send it to the TWS fails for some reason. Otherwise, it would sit there until the next EClient call is executed.


* **Provides default empty method for each EWrapper method**

  This means that you don't have to implement each EWrapper methods for yourself. TwsApiC++ does that for you.


* **Notifies when EWrapper methods without implementation are called**

  To prevent certain events are passed by unnoticed, these default methods print a warning message (see #define EWRAPPERL0_DEFAULT) when called. This helps when 'discovering' how the IB api works and what calls are involved, as that is will not always be clear from the documentation.
  
  Only in debug mode.


* **Provides 'EReader'**

  TwsApiC++ implements EReader functionality as found in the Java api and in the MS Windows based version of the C++ api. It runs in a separate thread and sits waiting for incoming data to process it without any delay (calling your EWrapper).

  Can be switched off simply by passing a parameter when instantiating the EWrapper class. In that case repetitive calling the non-blocking EClient::checkMessages() (see below) is necessarily to check for incoming events (data) send by the TWS.
  
  See [Example: With or without the 'EReader'](https://github.com/JanBoonen/TwsApiCpp/wiki/Example:-With-or-without-the-'EReader')


* **Provides non-blocking EClient::checkMessages()**

  The IB's implementation of EClient::checkMessage() call halts the programs until data is send to the client. TwsApiC++ overloads this method as a non-blocking call that waits for maximum 1 millisecond and which is safe to call in an endless loop without cpu usage penalty: when no data is received.
  
  See the example [Clients.cpp](https://github.com/JanBoonen/TwsApiCpp/blob/master/TwsApiC++/Test/Src/Clients.cpp?ts=4). It makes 8 connections simultaneously (maximum possible per client) and calls each of the 8 checkMessages() in a loop, or uses the EReader for each connection.

  
* **Provides help with the many textual and numeric parameter values**

  As a programmer, you need to pass a lot of textual parameters into the EClient methods parameters and you have to compare incomings strings parameters in the EWrapper to interprete them. This is a big issue because these values are not always very well documented and making a typo can be hard to detect at runtime.

  TwsApiC++ has predefined these values:
  -- allows them to use as named parameters what enables the compiler to check
  -- definitions are grouped per parameter, can be used in if statements and switch statementt
  -- all numeric values have a string value what makes reading code a lot easier, and can be printed out in the program as such
  -- unknown parameter values received in EWrapper can be recognised and and can be printed i.e.
  
  A quick example:
```C++
  // Just imagine you wrote
  if( key == "LookaheadAvailableFunds" ) { ... } // hmm, will never be executed
  // => because correct spelling is
  if( key == "LookAheadAvailableFunds" ) { ... }
  
  // instead you can write 
  if( UpdateAccountValueKey(key) == UpdateAccountValueKey::LookAheadAvailableFunds ) { ... }
  //  or
  switch( UpdateAccountValueKey(key) ) {
    case UpdateAccountValueKey::LookAheadAvailableFunds:
    break;
    ...
    case UpdateAccountValueKey::_INVALID_:  // key was not recognised - missing - report it!!
    break;
    
    default:
    break;
  }

  // and the following would raise a compiling error
  if( UpdateAccountValueKey(key) == UpdateAccountValueKey::LookaheadAvailableFunds ) { ... }
```

  See [TwsApiDefs.h](https://github.com/JanBoonen/TwsApiCpp/blob/master/TwsApiC++/Api/TwsApiDefs.h?ts=4).


* **Protects the original IB code from uncatched exceptions thrown from the users EWrapper code**

  TwsApiC++ protects the inner workings of the IB library against exceptions thrown inadvertently from within the user code statements in the derived EWrapper methods. Otherwise, these woud raise error *509, "Exception caught while reading socket - "* and parsing the incoming data would be halted.

  TwsApiC++ extends EWrapper with the method **onCatch( const char* MethodName, const long Id )** and calls it when it intercepts such exception.


* **Protects the user against unintentionally concurrent use of its internal data**

  Calling the same instance of EClient simultaniously could lead to the loss of requests send to the TWS, i.e. when the automatic ‘EReader’ thread is used and both the main thread and the EWrapper call methods of EClient.
  
  TwsApiC++ uses critical sections to prevents this situation.


* **‘closed’ library by design**

  You cannot derive from its EClient class directly and overwrites its methods. This will ensure the integrity of the whole safety measures build into this library.


* **EWRapper::connectionClosed()**

  TwsApiC++ garantees the EWRapper::connectionClosed() is called when the connection is closed for any reason, on purpose (eDisconnect()) or not. Every second a ‘heartbeat’ is send over the socket to check the status of the connection. This ensures the proper closing of the connection in case something went wrong.

  Note: There is no automatic reconnection
