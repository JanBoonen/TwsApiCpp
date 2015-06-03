# Test Debug Version

CC      = gcc
INCLUDE = -I../../Api -I../../../source/PosixClient/Shared
CFLAGS  = -mthreads -mconsole -D_REENTRANT -D_DEBUG
LIBS    = -L../../Api -lTwsApiL0D -lstdc++

Test: ../Src/Test.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/Test.cpp    $(LIBS) -o ./debug/Test.exe

History: ../Src/History.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/History.cpp $(LIBS) -o ./debug/History.exe

Bars: ../Src/Bars.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/Bars.cpp    $(LIBS) -o ./debug/Bars.exe

Clients: ../Src/Clients.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/Clients.cpp $(LIBS) -o ./debug/Clients.exe

all: Test History Bars Clients

clean:
	rm -f ./debug/*