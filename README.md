# TwsApiCpp
(same project as TwsApiC++ in Yahoo TWSAPI)

Makes programming with the IB POSIX C++ api significant easier, safer, and more robust.

It keeps the **focus** of the programmer **on the trading system**, instead of how to do things right with a socket connection

## Compiling and execution
This project includes all the IB POSIX C++ source files needed to compile this library, along with the 'unix alike' make files and the Microsoft specific Visual Studio project files. It should compile 'out of the box'.

* dir TwsApiC++ contains the sources code for this library
* dir source/PosixClient contains the needed copy of the IB POSIX C++ sources.

## Examples programs
There are some examples programs included in the Test directory. The most usefull downloads hystorical data.

## What does TwsApiCpp offers you

Well, nothing and a lot! TwsApiCpp does not introduce new features (nor does it hides any of the features), but it offers a **back to basics** layer around the complex IB POSIX C++ api, and makes programming with it a lot easier, safer and more robust, and that's a lot! It is **backward compatible with the non posix versions** and with older versions of the TWS. That might be important for the upcomming version 9.72.

The following explains it in more depth.

###Ease of use
* Including "TwsApiL0.h" is enough to have access to the full functionality

* It Reinstates the **EClient** and **Ewrapper** in combination with the other classes in the **Shared** directory such as Contract, Order, Execution, etc. , as the interface to all the functionality offered by IB
  No need to have knowledge about the POSIX specific classes in source/PosixClient/src and the socket low level programming. See the [wiki history](to do) page for a more lengthy explanation.

* It implements an empty method for every EWrapper method defined, no need to do it yourself.
  In debug mode, it prints a message to warn the programer that maybe an implementation is needed!

* It provides a **non-blocking EClient::checkMessages()** call with following properties:
  - calling it in an endless loop is possible and safe.
  - it offers the fastest response without any hardcoded delay;
  - does consume less then 1% of the cpu in idle state (no incoming data);

  See the example clients.cpp includes the Test directory
  
* It implements the **EReader** functionality as found in the Java api and in the MS Windows based version of the C++ api

  By default this EReader is started and it runs in a separate thread. This garantees the incoming data is processed without any delay.

  **Can be switched off** simply by passing a parameter when instantiating the EWrapper class. Of course, the user must call the non-blocking EClient::checkMessages() to check for incoming events (data) send by the TWS. [See Clients.cpp](blob/master/TwsApiC++/Test/Src/Clients.cpp)

* Optionally, it provides a way to check the correct spelling of the many textual or numeric parameters at compile time instead of runtime. This can reduce the test effort considerably, or even bring up hidden errors only discovered until a rare situation occurs.

  See [TwsApiDefs.h](TwsApiCpp/TwsApiC++/Api/TwsApiDefs.h)
  

###Safety, Robustness and stability
* It **protects** the inner workings of **the IB library against exceptions** thrown inadvertently from within the user code in the derived EWrapper methods.

  EWrapper is extended with a method onCatch() that is called in when an exceptions occurs 

* It **protects** the inner workings of **the IB library against concurrent use of its internal data**, which could lead to loss of requests send to the TWS.

  This can happen when the EClient is called from different threads, i.e. the automatic ‘EReader’ thread and the main thread. This can happen when handling the incoming data leads into a call of an EClient methods (i.e. buy or sell), and concurrently the main loop also calls EClient methods for some other reason.

* It is **by design a ‘closed’ library**, meaning you cannot derive from its EClient class directly and overwrites its methods.

  This will ensure the integrity of the whole safety measures build into this library.

* The footprint of the library is small and its code is fast. 


###Hides all the posix sockets related api issues introduced by IB since version 9.62.
* Reinstates the **EClient** and **Ewrapper** in combination with the other classes in the **Shared** directory such as Contract, Order, Execution, etc. , as the interface to all the functionality offered by IB

* **Resolves all POSIX socket connection related issues left to the programmer**

  It ensures the data is not stuck in the internal buffer of the IB api when the first attempt sending it to the TWS fails for some reason. Otherwise, it would sit there until the next EClient call is executed.

* It garantees the EWRapper::connectionClosed() is called when the connection is closed for any reason, on purpose or not.

    - It sends every second a ‘heartbeat’ to check the connection is still alive. This ensures the closing of the connection is detected in case something went wrong leaving the connection in an undefined status

  Note: There is no automatic reconnection




