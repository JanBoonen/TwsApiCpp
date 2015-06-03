//============================================================================
// TwsApi Test
//============================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
using namespace TwsApi;

#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
	#include <process.h>
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
	#include <pthread.h>
	#define CurrentThreadId pthread_self
#endif

#define PrintProcessId printf("%-8ld  ", CurrentThreadId() )

#include <time.h>

//----------------------------------------------------------------------------
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:

	MyEWrapper( bool CalledFromThread = true ): EWrapperL0( CalledFromThread ) {}

	virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute )
	{
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		PrintProcessId,printf
			( "TP: %4ld %02d:%02d:%02d %10s %5.3f\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, price
			);
	}

	virtual void winError( const IBString& str, int lastError )
	{
		PrintProcessId,printf( "WinError: %d = %s\n", lastError, (const char*)str );
	}

	virtual void connectionClosed()
	{
		PrintProcessId,printf( "Connection Closed\n");
	}

	virtual void error( const int id, const int errorCode, const IBString errorString )
	{
		PrintProcessId,printf( "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
	}

	virtual void connectionOpened( void )
	{
		PrintProcessId,printf( "Connection Opened\n");
	}

};

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main( void )
{
#define NR_CONNECTIONS 8	// 8 is maximum allowed by TWS

	Contract	C;
	C.symbol	= "MSFT";
	C.secType	= *SecType::STK;		//"STK"
	C.currency	= "USD";
	C.exchange	= *Exchange::IB_SMART;	//"SMART";



	//
	// messages checked in this thread
	//
	if( true ){
		MyEWrapper	MW(false);
		EClientL0*	EC[NR_CONNECTIONS];

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x] = EClientL0::New( &MW );

		bool connected = true;
		for(int x = 0; x < NR_CONNECTIONS; x++ ) connected = connected && EC[x]->eConnect( "", 7496, 100+x );

		PrintProcessId,printf( "ServerVersion = %d\n", EC[0]->serverVersion() );

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x]->reqMktData( x, C, "", false );

		time_t t1; time(&t1);
		time_t t2; time(&t2);
		while(time(&t2),(t2-t1) < 20)
		{
			for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x]->checkMessages();
		}

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x]->eDisconnect();
		
		for(int x = 0; x < NR_CONNECTIONS; x++ ) delete EC[x];
	}




	printf( "\n\n\n\n" );




	//
	// messages checked automatically in thread
	//
	if( true )
	{
		MyEWrapper	MW;
		EClientL0*	EC[NR_CONNECTIONS];

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x] = EClientL0::New( &MW );

		bool connected = true;
		for(int x = 0; x < NR_CONNECTIONS; x++ ) connected = connected && EC[x]->eConnect( "", 7496, 100+x );

		PrintProcessId,printf( "ServerVersion = %d\n", EC[0]->serverVersion() );

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x]->reqMktData( x, C, "", false );

		Sleep(20*1000);

		for(int x = 0; x < NR_CONNECTIONS; x++ ) EC[x]->eDisconnect();
		
		for(int x = 0; x < NR_CONNECTIONS; x++ ) delete EC[x];
	}

	printf("\n");


	PrintProcessId,printf( "Press return to end\n" ); char s[10]; gets(s);

	return 0;
}
