//============================================================================
// TwsApi Test
//============================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
using namespace TwsApi;

// to use the Sleep function
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

#define PrintProcessId printf("%ld  ", CurrentThreadId() )

#include <time.h>


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
struct OHLC
{
	double O, H, L, C;

	OHLC() { Reset(); }
	void Reset() { O = 0, C = 0, L = 0, C = 0; }
};

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
		switch( field )
		{
		case TickTypes::Open:
		case TickTypes::High:
		case TickTypes::Low:
		case TickTypes::Close:
		case TickTypes::Last:
		printf
			( "TP: %4ld %02d:%02d:%02d %15s %5.3f\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, price
			);
		default:{}
		}
	}

	virtual void tickSize( TickerId tickerId, TickType field, int size )
	{
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		switch( field )
		{
		case TickTypes::LastSize:
			printf
				( "TP: %4ld %02d:%02d:%02d %15s %5d\n"
				, tickerId
				, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
				, *(TickTypes::ENUMS)field, size
				);
		default:{}
		}
	}

	virtual void tickString( TickerId tickerId, TickType tickType, const IBString& value )
	{
		time_t		_t; time(&_t);
		struct tm	_tm	= *localtime( &_t );
		time_t		_t1 = atol((const char*)value);
		struct tm	_tm1= *localtime( &_t1 );
		
		switch( tickType )
		{
		case TickTypes::LastTimestamp:
			printf
				( "TP: %4ld %02d:%02d:%02d %15s %02d:%02d:%02d\n"
				, tickerId
				, _tm.tm_hour, _tm.tm_min, _tm.tm_sec
				, *(TickTypes::ENUMS)tickType
				, _tm1.tm_hour, _tm1.tm_min, _tm1.tm_sec
				);
		default:{}
		}
	}

	virtual void winError( const IBString& str, int lastError )
	{
		printf( "WinError: %d = %s\n", lastError, (const char*)str );
	}

	virtual void error( const int id, const int errorCode, const IBString errorString )
	{
		printf( "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
	}

};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main( void )
{
//	TestEnums();

	Contract			C;
	C.symbol			= "C";
	C.secType			= *SecType::STK;		//"STK"
	C.currency			= "USD";
	C.exchange			= *Exchange::IB_SMART;	//"SMART";
//	C.primaryExchange	= *Exchange::AMEX;

	MyEWrapper	MW( false );	// no thread
	EClientL0*	EC = EClientL0::New( &MW );

	printf( "ClientVersion = %d\n", EC->clientVersion() );

	if( EC->eConnect( "", 7496, 100 ) )
	{

		EC->reqMktData( 100, C, "", false );

		time_t t1; time(&t1);
		time_t t2; time(&t2);
		while(time(&t2),(t2-t1) < 200)
		{
			EC->checkMessages();
		}

	}


	EC->eDisconnect();

	delete EC;

	{ PrintProcessId,printf( "Press return to end\n" ); char s[10]; gets(s); }
	return 0;
}
