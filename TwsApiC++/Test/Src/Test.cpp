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
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:
	MyEWrapper( bool CalledFromThread = true ): EWrapperL0( CalledFromThread ) {}

	virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute )
	{
	///	int id = 10, y = 0; id = id/y; // Divide by zero to test EWrapperL0Protected
	///	int *id = 0; *id = 100;

	///	throw(0);

	//	time_t		_t	= GetEventTime();		// !! from L1
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		PrintProcessId,printf
			( "TP: %4ld %02d:%02d:%02d %10s %5.3f\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, price
			);
	}

	virtual void tickSize( TickerId tickerId, TickType field, int size )
	{
	//	time_t		_t	= GetEventTime();		// !! from L1
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		PrintProcessId,printf
			( "TS: %4ld %02d:%02d:%02d %10s %5d\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, size
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

	virtual void updateAccountValue( const IBString& key, const IBString& val, const IBString& currency, const IBString& accountName )
	{
		UpdateAccountValueKey::ENUMS UAVK;
		if( UAVK *= key )
			PrintProcessId,printf( "AC: %s %5s   %-30s %s\n", (const char*)accountName, (const char*)currency, *UAVK,            (const char*)val );
		else
			PrintProcessId,printf( "AC: %s %5s ? %-30s %s\n", (const char*)accountName, (const char*)currency, (const char*)key, (const char*)val );
	}

	virtual void accountDownloadEnd( const IBString& accountName )
	{
		PrintProcessId,printf( "AC: %s end\n", (const char*)accountName );
	}

	virtual void nextValidId( OrderId orderId )
	{
		PrintProcessId,printf( "nextValidId = %ld\n", orderId );
	}

	virtual void contractDetails( const ContractDetails& contractDetails )
	{
		const Contract& C = contractDetails.summary;

		PrintProcessId,printf	( "CD: %10s %5s %8s, %5.2f\n", (const char*)C.localSymbol, (const char*)C.secType, (const char*)C.expiry, C.strike );
	}

	virtual void error( const int id, const int errorCode, const IBString errorString )
	{
		PrintProcessId,printf( "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
	}

	virtual void historicalData( TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps )
	{
		if( IsEndOfHistoricalData(date) )
		{
			PrintProcessId,printf( "HD: %s\n", Finished() );
			return;
		}

		PrintProcessId,printf( "HD: %4ld %10s %5.3f %5.3f %5.3f %5.3f %7d %7d\n", reqId, (const char*)date, open, high, low, close, volume, barCount );
	}

   virtual void updateMktDepth(TickerId id, int position, int operation, int side, double price, int size )
   {
	   PrintProcessId,printf
			( "MD: %4ld %2d %10s %5s %7.2f %5d\n"
			, id
			, position
			, *(MktDepthOperation::ENUMS) operation
			, *(MktDeptSide::ENUMS)side
			, price
			, size
			);
   }

   virtual void updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size )
   {
   }


	virtual void connectionOpened( void )
	{
		PrintProcessId,printf( "Connection Opened\n");
	}

	virtual void checkMessagesStarted( void )
	{
		PrintProcessId,printf( ">>> checkMessagesStarted\n");
	}

	virtual void checkMessagesStopped( void )
	{
		PrintProcessId,printf( "<<< checkMessagesStopped\n");
	}
};

//----------------------------------------------------------------------------
// TestEnums
//----------------------------------------------------------------------------
void TestEnums( void )
{
	IBString			id = *TickTypes::Bid;
	id = *OrderStatus::PendingCancel;

	IBString			y;
	y = *TriggerMethod::LastPrice;

	OrderStatus::ENUMS	e;
	if( e *= "PendingCancel" )
	{ printf("OK\t"); }
	else
	{ printf("NOK\t"); }
	printf( "%4d %s\n", e, *e );


	if( e *= "not a status" )
	{ printf("OK\t"); }
	else
	{ printf("NOK\t"); }
	printf( "%4d %s\n", e, *e );

	switch( OrderStatus("PendingCancelxxxx") )
	{
	case OrderStatus::PendingCancel:
		{ printf("OK\n"); } break;
	case OrderStatus::_INVALID_:
		{ printf("NOK\n"); } break;
	default:
		{ printf("??\n"); } break;
	}

	// The iterator has a similar interface as the of the std::map
	for( UpdateAccountValueKey::iterator ac = UpdateAccountValueKey::begin(); ac != UpdateAccountValueKey::end(); ++ac )
		printf( "%4d %s\n", ac->first, ac->second );

}



//----------------------------------------------------------------------------
// Marco TEST
// Prints out the statement before executing it.
// Used just to demonstrate the api: you see the statement, and then the result
// i.e.: TEST(id, EC->reqMktData( id, C, "", false ) );
// Without TEST: EC->reqMktData( id, C, "", false );
//----------------------------------------------------------------------------
#define TEST( T, X ) ( printf( "T%7d %s\n", T, #X ), X )

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------


struct Contract_ : public Contract
{
	Contract_( IBString sb, IBString st, IBString cr, IBString ex )
	: Contract()
	{
		symbol			= sb;
		secType			= st;		//"STK"
		currency		= cr;
		exchange		= ex;	//"SMART";
	}
};

Contract_			C( "MSFT", *SecType::STK, "USD", *Exchange::IB_SMART );

int main( void )
{
	printf( "APIVersion    = %s\n", EClientL0::apiVersion() );

//	TestEnums();

	Contract_			C( "MSFT", *SecType::STK, "USD", *Exchange::IB_SMART );

/*
	Contract			C;
	C.symbol			= "MSFT";
	C.secType			= *SecType::STK;		//"STK"
	C.currency			= "USD";
	C.exchange			= *Exchange::IB_SMART;	//"SMART";
//	C.primaryExchange	= *Exchange::AMEX;
*/
	// from version 9.63 on, the protected ewrapper is active by default
	MyEWrapper	MW;
	EClientL0*	EC = EClientL0::New( &MW );

	printf( "ClientVersion = %d\n", EC->clientVersion() );

	TEST(0, EC->eDisconnect() );	// only for test purposes

	if( TEST(0, EC->eConnect( "", 7496, 100 )) )
	{
		PrintProcessId,printf( "ServerVersion = %d\n", EC->serverVersion() );

//		EC->reqNewsBulletins( true );
//		EC->reqNewsBulletins( true );
/*
		TEST( 0, EC->reqAccountUpdates( true, "" ) );
*/

//		for( int i = 0; i < 60; i++ )
		TEST( 100, EC->reqMktData( 100, C, "", false ) );
/*
		EC->reqMktDepth( 11, C, 3 );
*/
		EC->reqHistoricalData
			( 20
			, C
			, EndDateTime(2014,8,4)
			, DurationStr(1, *DurationHorizon::Days)
			, *BarSizeSetting::_1_hour
			, *WhatToShow::TRADES
			, true
			, FormatDate::AsDate
			);
/*
//		EC->reqMktData( 20, C, false );
		EC->reqHistoricalData
		( 20//EC->GetNextValidId()
		, C
		, EndDateTime(2006,10,03), DurationStr(1, DH_Days), *BarSizeSetting::_1_secs
		, *WS_TRADES
		, true
		, FD_AsDate
		);
*/
		{
		Contract C;
		C.symbol	= "C";
		C.secType	= *SecType::OPT;		//"STK"
		C.currency	= "USD";
		C.exchange	= *Exchange::IB_SMART;	//"SMART";
		EC->reqContractDetails( 25, C );


//		EC->reqContractDetails( C );
		}
/*  QQQQ
		C.symbol	= "DNEX";
		EC->reqContractDetails( C );

		C.symbol	= "MSFT";
		EC->reqContractDetails( C );
*/
	}

	int id = 1;
	while( id++ < 1000 )
	{
	//	if( !MW.IsCalledFromThread() ) EC->checkMessages();
		Sleep( 100 );

	/* for 'stress' testing */
		if( 0 == id%50)
			TEST(id, EC->reqMktData( id, C, "", false ) );
	/**/
		if( 30 == id)
			TEST(id, EC->eDisconnect() );
	/**/
		if( 40 == id)
			TEST(id, EC->eDisconnect() );


		if( 60 == id || 70 == id )
			TEST(id, EC->reqMktData( id, C, "", false ) );	// will fail of course


		if( 130 == id)
			TEST(id, EC->eConnect( "", 7496, 10 ) );

		if( 150 == id)
			TEST(id, EC->eConnect( "", 7496, 10 ) );		// raises already connected error

		if( 190 == id )
			TEST(id, 
			EC->reqHistoricalData
				( id
				, C
				, EndDateTime(2013,02,20), DurationStr(1, *DurationHorizon::Days), *BarSizeSetting::_1_hour
				, *WhatToShow::TRADES
				, true
				, FormatDate::AsDate
				)
			);

		if( 200 == id )
			TEST(id, EC->reqMktData( id, C, "", false ) );

	/**/
		if( 800 == id)
			TEST(id, EC->cancelMktData( 200 ) );
	/**/
	}


	TEST( 0, EC->eDisconnect() );

	delete EC;

	{ PrintProcessId,printf( "Press return to end\n" ); char s[10]; gets(s); }
	return 0;
}
