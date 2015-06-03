//============================================================================
// TwsApi Test
//============================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
using namespace TwsApi;

bool EndOfHistoricalData	= false;
bool ErrorForRequest		= false;

//----------------------------------------------------------------------------
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:

	MyEWrapper( bool CalledFromThread = true ) : EWrapperL0( CalledFromThread ) {}

	virtual void winError( const IBString& str, int lastError )
	{
		fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
		ErrorForRequest = true;
	}

	virtual void error( const int id, const int errorCode, const IBString errorString )
	{
		fprintf( stderr, "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
		ErrorForRequest = (id > 0);
		// id == -1 are 'system' messages, not for user requests
		// as a test, set year to 2010 in the reqHistoricalData
	}

	virtual void historicalData( TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps )
	{
		if( IsEndOfHistoricalData(date) )
		{
			EndOfHistoricalData = true;
			return;
		}

		fprintf( stdout, "%10s, %5.3f, %5.3f, %5.3f, %5.3f, %7d\n", (const char*)date, open, high, low, close, volume );
	}

};


//----------------------------------------------------------------------------
// DUMP_DEFS
// Dumps the list of valid values for enum E
//----------------------------------------------------------------------------
// The iterator has a similar interface as the of the std::map
#define DUMP_DEFS( E ) \
	for( E::iterator i = E::begin(); i != E::end(); ++i ) { \
		printf( "\'%s\'\n", i->second ); }

//----------------------------------------------------------------------------
// ARGV
// Checks whether V is valid value for enum E
// Whn invalid, prints list of the valid values via DUMP_DEFS
//----------------------------------------------------------------------------
#define CHECK_VAR( E, V ) {\
	E::ENUMS e; \
	if( !(e *= V) ) { \
		printf("Error in argument '%s': value '%s' is not recognised.\nFollowing are valid values:\n", #E, V); \
		DUMP_DEFS( E ) \
		return 1; \
	} }


//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main( int argc, const char* argv[] )
{
	Contract			C;
	C.symbol			= "MSFT";
	C.secType			= *SecType::STK;		//"STK"
	C.currency			= "USD";
	C.exchange			= *Exchange::IB_SMART;	//"SMART";
	C.primaryExchange	= *Exchange::AMEX;


	int EDTY			= 2014;
	int EDTM			= 8;
	int EDTD			= 4;
	IBString DS			= DurationStr(1, *DurationHorizon::Months);
	IBString BSS		= *BarSizeSetting::_1_day;
	IBString WTS		= *WhatToShow::TRADES;

	if( argc > 1 )
	{
		if( argc < 8 )
		{
			printf	( "args are: symbol  end-date  duration  barsize  what-to-show.\n"
					  "     i.e. MSFT    20140804  10 D      1 day    TRADES\n"
					);
			return 1;
		}

		C.symbol		= argv[1];
		IBString EDT	= argv[2];
		if( EDT.size() != 8 )
		{
			printf	( "end-date should be 8 characters: YYYYMMDD\n"
					  "     i.e. 20140804\n"
					);
			return 1;
		}
		EDTY = atoi( EDT.substr(0,4).data() );
		EDTM = atoi( EDT.substr(4,2).data() );
		EDTD = atoi( EDT.substr(6,2).data() );

		CHECK_VAR( DurationHorizon, argv[4] ) // DurationHorizon
			DS		= DurationStr(atoi(argv[3]), argv[4] );

		IBString bss = argv[5]; bss = bss + " " + argv[6];
		CHECK_VAR( BarSizeSetting, (const char*)bss )	// BarSizeSetting
			BSS		= bss;

		CHECK_VAR( WhatToShow, argv[7] )	// WhatToShow
			WTS		= argv[7];
	}


	MyEWrapper	MW( false );
	EClientL0*	EC = EClientL0::New( &MW );

	if( EC->eConnect( "", 7496, 100 ) )
	{
		EC->reqHistoricalData
			( 20
			, C
			, EndDateTime(EDTY,EDTM,EDTD)	// Attention: for IB, this means last day will be 2013,02,19
			, DS							// DurationStr(1, *DurationHorizon::Months)
			, BSS							// *BarSizeSetting::_1_day
			, WTS							// *WhatToShow::TRADES
			, UseRTH::AllTradingData		// or use OnlyRegularTradingData
			, FormatDate::AsDate
//			, TagValueListSPtr()			// solved by default parameter in EClientL0
			);

		while( !EndOfHistoricalData && !ErrorForRequest )
			EC->checkMessages();
	}

	EC->eDisconnect();
	delete EC;

	return ErrorForRequest;
}
