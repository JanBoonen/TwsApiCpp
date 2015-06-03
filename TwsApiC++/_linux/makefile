# TwsApiC++ Debug Version

CC      = gcc
INCLUDE = -I../Api -I../Src -I../../source/PosixClient/Shared -I../../source/PosixClient/src
CFLAGS  = -D_REENTRANT -D_DEBUG -ggdb -std=gnu++0x -Wall

../Api/libTwsApiL0D.a: ../Api/TwsApiL0.h ../Api/TwsApiDefs.h ../Api/Enumerations.h ../Src/TwsApiL0.cpp ../../source/PosixClient/src/EClientSocketBase.cpp ../../source/PosixClient/src/EPosixClientSocket.cpp ../../source/PosixClient/src/EPosixClientSocket.h ../../source/PosixClient/src/EPosixClientSocketPlatform.h
	$(CC) $(INCLUDE) $(CFLAGS) -c ../Src/TwsApiL0.cpp   -o ./debug/TwsApiL0.o
	ar rcs ../Api/libTwsApiL0D.a ./debug/TwsApiL0.o

clean:
	rm -f ../Api/libTwsApiL0D.a ./debug/TwsApiL0.o

