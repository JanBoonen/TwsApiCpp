=====================
= TwsApi C++ 9.71   =
=====================
Released as is and free to use, but when used in a commercial environment, documentation should include a reference to the use of this software.

By
Jan Boonen
Hofstade
Belgium


Purpose
-------
Provide a portable and a platform/vendor independant C++ API based on the POSIX C++ API provided by IB, that is backward compatible with version 9.60 and before.

Starting from version 9.62, IB supports a C++ API with POSIX socket implementation.  This is a platform and vendor independant implementation.  The only drawback of the POSIX API is its complexity and its non-compatibility with versions previous to 9.62.  Indeed, the new API is the combination of the classes EClient/EWrapper, the class EPosixClientSocket AND the POSIX sockets API. Reading the example code in directory TestPosixSocketClient makes it clear: you will find a call to the select() method, the use of the low level macro's FD_ZERO, FD_SET, FD_CLR and FD_ISSET and tests on the file descriptor whether it is still valid or not, plus calls to onReceive(), onSend(), and onError(), introduced in new class EPosixClientSocket in the POSIX API. This all makes the use of the TWS C++ API far more complicated then it was before and not just plug & play with previous versions.

This TwsApiC++ ensures all that is hidden and restores the backwards 'plug & play' compatibility, except for the xstring wich is replaced by newly introduced IBString by IB in version 9.62.


9.71: Differences with TwsApiC++ versions 9.67_4
--------------------------------------------------
* IB changed its file structure drastically, but the way to use this lib remains unchanged. The zip file contains all is needed to build the c++ POSIX library. 

* ECLient:
** method added that returns the api version
   static  char* apiVersion( void ) { return "9.71"; }
** Some existing methods got an extra parameter of type 'const TagValueListSPtr&' only for internal usage by IB. For these, an extra definition is added in EClientL0 where this parameter get a default value to ensure backwards compatibility.
  The methods involved are
    reqMktData
    reqMktDepth
    reqHistoricalData
    reqRealTimeBars
    reqScannerSubscription

* EWRAPPERL0
** The default empty methods in EWrapperL0 print a message in debug mode to ensure their call is exposed.
    #define EWRAPPERL0_DEFAULT(id, name)
    #if 1	// set to 0 to prevent this warning message in debug mode
    #ifdef _DEBUG
    #undef  EWRAPPERL0_DEFAULT
    #define EWRAPPERL0_DEFAULT(id, name) printf( "#warning: empty default method EWrapperL0::%s(id=%d) was called!\n", name, id );
    #endif
    #endif



9.67_4: Differences with TwsApiC++ versions 9.67_3
--------------------------------------------------
* the _linux/makefile(s) used the wrong -gdb switch, is now -ggdb (for the dgb debugger)

9.67_3: Differences with TwsApiC++ versions 9.67.2
--------------------------------------------------
* the Test/_linux/makefile was a wrong copy (my mistake), it is now the orrect one

* the Test/_cygwin/makefile and the Test/_linux/makefile both have now targets for each program and an 'all' target to makes each of them in one go

* Test/_linux/compile script now call the make command by its full path '/usr/bin/make'

* for linux, the -gdb compiler flag is added, so that gdb can be used to step through the program while debugging and show the source code line by line. (thanks Randall)


9.67: Differences with TwsApiC++ versions 9.65
----------------------------------------------
This release is an upstep to version 9.67, but also a merger of the specific linux version contributed by Randall L. Rathbun. It is available as TwsApiC++.9.65.v2.tar.gz in the same directory.

His special note:
SPECIAL NOTES:

I built a linux section for Ubuntu V12.04 LTS (Precise Pangolin edition) which uses the gcc compiler v4.6.3 (Ubuntu/Linaro 4.6.3-1ubuntu5)  and tested it with both the IB TWS and the IB Gateway Client.

I also fixed an incomplete switch statement in EClientSocketBaseImpl.h and unused variable warnings for TwsApiL0.cpp in TwsApic++.9.65/TwsApiC++/Src and Main.cpp in TwsApiC++.9.65/TwsApiC++/Test/Src

The make command with the -Wall option on the compile now passes both the library creation and the testing section.

Author: Randall L. Rathbun
Date: 2012-August-31

Many thank Randall!

Note: the adjustement on EClientSocketBaseImpl.h is not included, because the polici is not to touch the source of IB.


* The new introduced methods added to EClient and EWrapper
** EClient: reqMarketDataType
** EWrapper: marketDataType and commissionReport

* Linux adjustements, thanks to Randall
** see the _linux for the make files
** some includes stdio.h and string.h for linux
** printf format specifiers, %d -> %ld where needed (also in main.cpp)

* TwsApiDefs: updated with new definitions/values, and 1 bugfixed
** SecType updated: 3 new values: WAR, FUND and EFP
** ClearIntent bugfixed: it must be a string instead of en numeric value
*** Default (""), IB ("IB"), Away ("Away"), PTA ("PTA") // (post trade allocation).
** HedgeType added : Delta ("D"), Beta ("B"), FX ("F"), Pair ("P")
** HedgeParam added: BetaHedge ("beta=X"), PairHedge ("ratio=Y")
** SmartComboRouting added: (indeed, long names)
*** Priority ("LeginPrio")
*** DiscretionaryAmount ("MaxSegSize")
*** MarketIfTouchedTimeoutAfterFirstFill ("ChangeToMktTime1")
*** MarketIfTouchedTimeoutAfterLastFill ("ChangeToMktTime2")
*** MarketIfTouchedStopLoss ("ChangeToMktOffset")
*** MaximumLegInSize ("MaxSegSize")
*** DiscretionaryPercentage ("DiscretionaryPct")
** MarketDataType added: Realtime and Frozen
** BarSizeSetting: values updated (10 secs, 10 mins and 4 hours added)

* Test: main_mini.cpp is replaced by two other examples:
** Main-history.cpp: connects with TWS and executes a reqHistoricalData().
Default parameters, or with some on the command line i.e.
MSFT  20130220  1 M  1 day  TRADES
It also demonstrates the use of the TwsApiDefs.h, just try wrong a parameter
** Main-8clients.cpp: demonstrates the low overhead of the library by running 8 connection simultaniously, first with checkMessages() called in the main thread for each connection, and secondly with a separate thread per connection, and just a Sleep(20seconds) to let the data come in automatically.  Take a close look at the printed process id's at the beginning of the lines.


9.65: Differences with TwsApiC++ versions 9.64
----------------------------------------------
* Support for new method EClient::reqGlobalCancel()

* TickTypes extended with new values
** TRADE_COUNT    = 54
** TRADE_RATE     = 55
** VOLUME_RATE    = 56
** LAST_RTH_TRADE = 57

* .\Test\Src\Main_mini.cpp added
** it is an updated version of Main_minimal.cpp included in previous versions

Remark: in the Shared directory delivered by IB, two old files are included that are not needed anymore: EClientSocket.h and MySocket.h.  They are not included in this delivery of TwsApiC++ 9.65.

9.64: Differences with TwsApiC++ versions 9.63
----------------------------------------------
* TwsApiDefs en Enumerations.h
** The way to define the enumerations is extracted from TwsApiDefs.h and defining maintaining is now easier
** But although the purpose is the same, the new enumerations are each enclosed in a class, instead of giving each name a different prefix.
An example
 TT_Bid        is now  TickTypes::Bid
 OS_Submitted  is now  OrderStatus::Submitted
** Advantage/Disadvantage:
** + more readable WS_BID => WhatToShow::BID
** + conversion id/string and iteration possible even when the values are not consecutive, see i.e. TriggerMethod
** + enumerations are easier to maintain
** + you can use the system to add your own enumerations for other parts in the program
** - more text to write in the code
** - current code needs update

But it is possible to keep using the old system:
i) at the beginning of source TwsApiL0.cpp, change:
#if 0
//Old version

into
#if 1
//Old version

ii) and replace the new TwsApiDefs.h with the old one.


Differences with TwsApiC++ versions 9.60 and before
---------------------------------------------------
* xstring is replaced by IBString:
IB had to replace its MFC specific CString with something based on the C++ standard.  They've choosen for stl::string and extended it with a few extra methods and called it IBString.  TwsApiC++ extends it with some methods to keep its compatibility with its xstring.  So the impact of that change should be minimal.

* TwsApiDefs.h
is used the same way as before, but has been rewritten:
** including it multiple times in one project will not lead to linking problems.
** strings and enumeration values are both defined in one statement what reduces changes for errors and improves its maintenanability
Note: TwsApiDefs.h is in fact a very convenient system to support conversions between enumerations and their string representations. So it could be used on its own in other projects as well.

* class EWrapperL0Protected
is no longer part of the TwsApiL0, as it is integrated in the library.  Just rename EWrapperL0Protected back to EWrapperL0 in case you were using it.

* OnCatch event in EWrapperL0
When an exception is catched in the intergrated EWrapperL0Protected, it calls the OnCatch event in the EWrapperL0:
virtual void OnCatch( const char* MethodName, const long Id );

* Threaded checkMessages()
In previous versions prior to 9.63 of TwsApiC++, a seperate thread reads the incomming events from the socket and calls the appropriate EWrapper methods. Starting from version 9.63, TwsApiC++ offers the choice to not run this automatic thread.  In that case, the user program must call checkMessages().  More info in the following chapter.

* An extra error COULD_NOT_START_THREAD is defined in TwsApiL0.h:
static const CodeMsgPair COULD_NOT_START_THREAD( 700, "Couldn't start thread to read incomming messages." );
This error is send from eConnect() in case the extra thread for automatic execution of checkMessages() failed to start.  After sending this message, the connection is closed and eConnect returns with false.  But because the connection was established first, connectionOpened() and connectionClosed() are called!

* The zip files contains more then TwsApiC++ only
It contains also a copy of all needed sources from the directories  shared  and  PosixSocketClient, just for convenience.



Use and Implementation notes
----------------------------
By default, the MS behaviour of class CAsyncSocket is simulated by executing checkMessages() in a seperate thread. The EWrapper members are called from this thread the moment the events arrives in the socket. In this automatic mode, calling checkMessages() in the application is unnecessary, but doing so does no harm and would make no difference.

From version 9.63 on, the choice whether or not to start a thread is a property of EWrapperL0 set durings its creation: EWrapper EW( false );  When set to false, no thread is started and the user program must call checkMessages() periodically in order to handle the incoming events on the socket.  See the main.cpp for an example.

It is garanteed that connectionClosed() is called and the automatic thread, if any, stops running (if any) whenever the connection with TWS gets broken for any reason.  There is no need to call eDisconnect() when receiving the connectionClosed() event without a call to eDisconnect().  Because sockets have no method to detect that the other end died without closing the socket,  about every second, a '\0' byte is send to TWS as a check.

For convenience, a class EWrapperL0 derived from EWrapper provides an empty body for each method and adds a few:
* connectionOpened() is called when the connection with TWS is established successfully (symmetry with connectionClosed());
* IsEndOfHistoricalData() can be used to check for the end in the historicalData() stream.
Class EClient is extended into EClientL0 and provides some extra methods, i.e. clientVersion() and IsConnected().
* OnCatch() is called whenever an exception is raised in the users EWrapper code.  The tickerId/orderId/reqId and the method name in wich the exception was raised are available.

The result is very lightweight and fast. The library in release mode is < 1MB and the small test program compiles into less then 300KB and needs less then 2MB of memory to run.

One user, Mark Casson mentioned to have some issues when integrating an older version of this library with Qt and qmake. He solved it by just including TwsApiL0.cpp in its program instead of compiling it separately.  So thanks for sharing this knowledge.


Platforms/compilers tested so far
---------------------------------
For the moment, it is tested only on a Windows platform, but both the WIN32 and the non-win32 (==unix assumed) path are compiled and tested successfully.

The Visual C++ Express 2008 files sln and vcproj files are included and it should compile out of the box. Use TwsApiC++\Win32\TwsApi.sln for it.

Also available are the makefiles for the Linux/unix make utility.

NON-WIN32 == UNIX:
I downloaded and installed the cygwin environment via the setup at http://www.cygwin.com/ and added some development support via the same setup.  There are two makefiles in the project (see below in the file list). One compiles the library, the other compiles the test program.  Compiling/linking and running the result went well.  Only a debug version is implemented in the makefiles for now.
For convenience, the ./compile shell in TwsApiC++\Test\_cygwin\. compiles both the library and the test program, while the ./run script compiles and runs the test.

LINUX (by Randall L. Rathbun)
A linux section has been added and tested under Ubuntu LTS 12.04.  There is a makefile for compiling the library and also a makefile for compiling the test program.

As in the Cygwin, compile compiles both the library and test program, while ./run compiles and runs the test.  Be sure to set the listener to port 7496 on the IB Gateway Client in order for it to detect the socket requests.  You can recompile this port to a different one (say 4001) if you wish by changing the source files.

Installation
------------
For a quick start under Windows, expand the zipfile into the Jts directory. The Visual C++ Express 2008 files .sln and .vcproj files are included, and the projects should compile out of the box.  Of course, the directories Jts\Shared, and Jts\PosixSocketClient must be available.

When you consider to make a new makefile, keep following in mind:
A) the include directories to build the library should be in following order:
  1) TwsApiC++\Api
  2) TwsApiC++\Src
  3) Jts\Shared
  4) Jts\PosixSocketClient\src
B) only TwsApiC++\Src\TwsApiL0.cpp should be compiled to build the library. All other cpp files are incuded to get access to some #defines and private members.

The compiler option to allow multithreading must be switched on.

Including TwsApiC++\Api\TwsApiL0.h is sufficient to use the library. TwsApiDefs.h is optional but must be included after TwsApiL0.h.

To build the cygwin version, the current directory must be on _cygwin when executing the make command.  First build the api, then the test program, or execute the compile or run script in TwsApiC++\Test\_cygwin


Overview of the zip file (TwsApiC++ only)
-----------------------------------------
TwsApiC++\Readme.txt		: this file

TwsApiC++\Api			: the directory to be included in the include path when using this library
TwsApiC++\Api\Enumerations.h	: Included when TwsApiDefs is used.
TwsApiC++\Api\TwsApiDefs.h	: An optional include file.  See the comments for its purpose.  Is used in the test program.
TwsApiC++\Api\TwsApiL0.h	: The only mandatory include file for TwsApi. It includes all other necessary include files.

TwsApiC++\Src\afxwin.h		: To #undef _MSC_VER
TwsApiC++\Src\TwsApiL0.cpp	: The source code of the library (~ 700 lines)

TwsApiC++\_cygwin\makefile	: makefile for the library
TwsApiC++\_cygwin\debug		: output directory for the build step

TwsApiC++\_linux\makefile	: makefile for the library
TwsApiC++\_linux\debug		: output directory for the build step

TwsApiC++\_win32\TwsApi.sln	: Visual C++ Express 2008 Solution file (= main project file)
TwsApiC++\_win32\TwsApi.vcproj	: Visual C++ Express 2008 project file for the TwsApi library

TwsApiC++\Test\Src\Test.cpp	: The source of the small test/example program
TwsApiC++\Test\Src\Bars.cpp	: Example of a reqMktData, no command line paramaters can be specified yet
TwsApiC++\Test\Src\History.cpp	: Example of reqHistoricalData, accepts command line parameters
TwsApiC++\Test\Src\Clients.cpp	: makes 8 connections and demontrates both ways to check for incomming data

TwsApiC++\Test\_cygwin\makefile	: makefile for the example programs.
TwsApiC++\Test\_cygwin\debug	: output directory for the build step
TwsApiC++\Test\_cygwin\compile	: shell script that compiles the library irst and then  the test programm.
TwsApiC++\Test\_cygwin\run	: shell script that compiles the library first and then the test programm and runs the test program

TwsApiC++\Test\_linux\makefile	: makefile for the example programs.
TwsApiC++\Test\_linux\debug	: output directory for the build step
TwsApiC++\Test\_linux\compile	: shell script that compiles the library first and then the test programm.
TwsApiC++\Test\_linux\run	: shell script that compiles the library first and then the test programm and runs the test program

TwsApiC++\Test\_win32\Test.vcproj:Visual C++ Express 2008 project file for Test
