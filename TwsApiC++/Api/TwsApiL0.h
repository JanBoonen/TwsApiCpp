/*===========================================================================*\
TwsApiL0.h

Provide a portable and a platform/vendor independant C++ API based on the
POSIX C++ API provided by IB, that is backward compatible with version 9.60
and before.

In the default mode, a seperate thread reads the incoming data from TWS and
calls the EWrapper events automatically. Calling checkMessages() is not
needed and doing so would make no difference. 

Changing this default mode is possible, see constructor of EWrapperL0, and
in that case, checkMessages() MUST be called periodically.

EWrapperL0 extends EWrapper with a connectionOpened() event for reasons of 
symmetry with connectionClosed().  It is called just before eConnect() returns
if the connection was established successfuly.

Each EWrapperL0 event has a default empty implementation for convenience.

Internally, a layer protects the TwsApi from untrapped exceptions thrown by
the user implemented EWrapper methods. When such exception is trapped, this
internal layer calls the new method EWrapper::OnCatch().

EClientL0 extends EClient with clientVersion() that returns the client version
currently in use, this in symmetry with serverVersion().

\*===========================================================================*/
#ifndef _TwsApiL0_h
#define _TwsApiL0_h

//----------------------------------------------------------------------------
//  Add some C necessirites (for linux)
//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------
// IBString is extended for downward compatibility
// and to make the TwsApiDefs special operators work
//----------------------------------------------------------------------------
#define IB_USE_STD_STRING
/**/
#define IBString _IBString
#include "IBString.h"
#undef IBString

struct IBString: public _IBString
{
	IBString( void					) { reserve(32); }
	IBString( const char*		 s	) { this->assign(s); }
	IBString( const std::string& s	) { this->assign(s.data()); }

	operator const char*  (void) const{ return data(); }
};
/**/

// The other includes from shared
#include "EWrapper.h"
#include "EClient.h"
#include "Contract.h"
#include "Execution.h"
#include "OrderState.h"					// includes order.h
#include "ScannerSubscription.h"
#include "CommissionReport.h"

#include "TwsSocketClientErrors.h"

// An extra error to indicate a thread couldn't start during eConnect when asked for
static const CodeMsgPair COULD_NOT_START_THREAD( 700, "Couldn't start thread to read incomming messages." );

//----------------------------------------------------------------------------
// EClientL0
//----------------------------------------------------------------------------
class EClientL0: public EClient
{
public:

	// A static member that allocates an instance of this EClientL0
	static	EClientL0*	New( class EWrapperL0* Wrapper );

	// A static member that returns the api version
	static  const char* apiVersion( void ) { return (const char*)"9.71"; }
	
	// See all members declared in EClient

	// set the last parameter "const TagValueListSPtr&" to an empty as it is only for IB internal use!
	virtual void reqMktData( TickerId id, const Contract &contract, const IBString& genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions = TagValueListSPtr()) = 0;
	virtual void reqMktDepth( TickerId id, const Contract &contract, int numRows, const TagValueListSPtr& mktDepthOptions = TagValueListSPtr()) = 0;
	virtual void reqHistoricalData( TickerId id, const Contract &contract, const IBString &endDateTime, const IBString &durationStr, const IBString &barSizeSetting, const IBString &whatToShow, int useRTH, int formatDate, const TagValueListSPtr& chartOptions = TagValueListSPtr()) = 0;
	virtual void reqRealTimeBars( TickerId id, const Contract &contract, int barSize, const IBString &whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions = TagValueListSPtr()) = 0;
	virtual void reqScannerSubscription( int tickerId, const ScannerSubscription &subscription, const TagValueListSPtr& scannerSubscriptionOptions = TagValueListSPtr()) = 0;

	//---- Extra Methods -----------------------------------------------------

	// Access to the EWrapper
	virtual EWrapper*	GetEWrapper( void ) = 0;

	// Besides a Server version, there is a client version as well
	virtual int			clientVersion( void ) = 0;

	// Returns true when the Api is connected to TWS.
	virtual bool		IsConnected( void ) = 0;
};

//----------------------------------------------------------------------------
// EWrapperL0
// The posix implementation never calls winError.
//----------------------------------------------------------------------------
#define EWRAPPERL0_DEFAULT(id, name)
#if 1	// set to 0 to prevent this warning message in debug mode
#ifdef _DEBUG
#undef  EWRAPPERL0_DEFAULT
#define EWRAPPERL0_DEFAULT(id, name) printf( "#warning: empty default method EWrapperL0::%s(id=%ld) was called!\n", name, static_cast<long int>(id) );
#endif
#endif

class EWrapperL0: public EWrapper
{
public:
	// Default empty implementations for each method in the callback class
	virtual void tickPrice             ( TickerId tickerId, TickType field, double price, int canAutoExecute ) { EWRAPPERL0_DEFAULT( tickerId, "tickPrice" ) }
	virtual void tickSize              ( TickerId tickerId, TickType field, int size ) { EWRAPPERL0_DEFAULT( tickerId, "tickSize" ) }
	virtual void tickOptionComputation ( TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) { EWRAPPERL0_DEFAULT( tickerId, "tickOptionComputation" ) }
	virtual void tickGeneric           ( TickerId tickerId, TickType tickType, double value ) { EWRAPPERL0_DEFAULT( tickerId, "tickGeneric" ) }
	virtual void tickString            ( TickerId tickerId, TickType tickType, const IBString& value ) { EWRAPPERL0_DEFAULT( tickerId, "tickString" ) }
	virtual void tickEFP               ( TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints, double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry) { EWRAPPERL0_DEFAULT( tickerId, "tickEFP" ) }
	virtual void orderStatus           ( OrderId orderId, const IBString& status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString& whyHeld ) { EWRAPPERL0_DEFAULT( orderId, "orderStatus" ) }
	virtual void openOrder             ( OrderId orderId, const Contract& contract, const Order& order, const OrderState& orderState ) { EWRAPPERL0_DEFAULT( orderId, "openOrder" ) }
	virtual void openOrderEnd          () { EWRAPPERL0_DEFAULT( NO_VALID_ID, "openOrderEnd" ) }
	virtual void winError              ( const IBString& str, int lastError ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "winError" ) }
	virtual void connectionClosed      () { EWRAPPERL0_DEFAULT( NO_VALID_ID, "connectionClosed" ) }
	virtual void updateAccountValue    ( const IBString& key, const IBString& val, const IBString& currency, const IBString& accountName ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "updateAccountValue" ) }
	virtual void updatePortfolio       ( const Contract& contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString& accountName ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "updatePortfolio" ) }
	virtual void updateAccountTime     ( const IBString& timeStamp ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "updateAccountTime" ) }
	virtual void accountDownloadEnd    ( const IBString& accountName ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "accountDownloadEnd" ) }
	virtual void nextValidId           ( OrderId orderId ) { EWRAPPERL0_DEFAULT( orderId, "nextValidId" ) }
	virtual void contractDetails       ( int reqId, const ContractDetails& contractDetails ) { EWRAPPERL0_DEFAULT( reqId, "contractDetails" ) }
	virtual void bondContractDetails   ( int reqId, const ContractDetails& contractDetails ) { EWRAPPERL0_DEFAULT( reqId, "bondContractDetails" ) }
	virtual void contractDetailsEnd    ( int reqId ) { EWRAPPERL0_DEFAULT( reqId, "contractDetailsEnd" ) }
	virtual void execDetails           ( int reqId, const Contract& contract, const Execution& execution ) { EWRAPPERL0_DEFAULT( reqId, "execDetails" ) }
	virtual void execDetailsEnd        ( int reqId ) { EWRAPPERL0_DEFAULT( reqId, "execDetailsEnd" ) }
	virtual void error                 ( const int id, const int errorCode, const IBString errorString ) { EWRAPPERL0_DEFAULT( id, "error" ) }
	virtual void updateMktDepth        ( TickerId id, int position, int operation, int side, double price, int size ) { EWRAPPERL0_DEFAULT( id, "updateMktDepth" ) }
	virtual void updateMktDepthL2      ( TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size ) { EWRAPPERL0_DEFAULT( id, "updateMktDepthL2" ) }
	virtual void updateNewsBulletin    ( int msgId, int msgType, const IBString& newsMessage, const IBString& originExch ) { EWRAPPERL0_DEFAULT( msgId, "updateNewsBulletin" ) }
	virtual void managedAccounts       ( const IBString& accountsList ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "managedAccounts" ) }
	virtual void receiveFA             ( faDataType pFaDataType, const IBString& cxml ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "receiveFA" ) }
	virtual void historicalData        ( TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps ) { EWRAPPERL0_DEFAULT( reqId, "historicalData" ) }
	virtual void scannerParameters     ( const IBString& xml ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "scannerParameters" ) }
	virtual void scannerData           ( int reqId, int rank, const ContractDetails &contractDetails, const IBString &distance, const IBString &benchmark, const IBString &projection, const IBString &legsStr) { EWRAPPERL0_DEFAULT( reqId, "scannerData" ) }
	virtual void scannerDataEnd        ( int reqId) { EWRAPPERL0_DEFAULT( reqId, "scannerDataEnd" ) }
	virtual void realtimeBar           ( TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count) { EWRAPPERL0_DEFAULT( reqId, "realtimeBar" ) }
	virtual void currentTime           ( long time ) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "currentTime" ) }
	virtual void fundamentalData       ( TickerId reqId, const IBString& data ) { EWRAPPERL0_DEFAULT( reqId, "fundamentalData" ) }
	virtual void deltaNeutralValidation( int reqId, const UnderComp& underComp ) { EWRAPPERL0_DEFAULT( reqId, "deltaNeutralValidation") }
	virtual void tickSnapshotEnd       ( int reqId ) { EWRAPPERL0_DEFAULT( reqId, "tickSnapshotEnd" ) }
	virtual void marketDataType        ( TickerId reqId, int marketDataType) { EWRAPPERL0_DEFAULT( reqId, "marketDataType" ) }
	virtual void commissionReport      ( const CommissionReport &commissionReport) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "commissionReport" ) }
	virtual void position              ( const IBString& account, const Contract& contract, int position, double avgCost) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "position" ) }
	virtual void positionEnd           () { EWRAPPERL0_DEFAULT( NO_VALID_ID, "positionEnd" ) }
	virtual void accountSummary        ( int reqId, const IBString& account, const IBString& tag, const IBString& value, const IBString& curency) { EWRAPPERL0_DEFAULT( reqId, "accountSummary" ) }
	virtual void accountSummaryEnd     ( int reqId) { EWRAPPERL0_DEFAULT( reqId, "accountSummaryEnd" ) }
	virtual void verifyMessageAPI      ( const IBString& apiData) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "verifyMessageAPI" ) }
	virtual void verifyCompleted       ( bool isSuccessful, const IBString& errorText) { EWRAPPERL0_DEFAULT( NO_VALID_ID, "verifyCompleted" ) }
	virtual void displayGroupList      (  int reqId, const IBString& groups) { EWRAPPERL0_DEFAULT( reqId, "displayGroupList" ) }
	virtual void displayGroupUpdated   ( int reqId, const IBString& contractInfo) { EWRAPPERL0_DEFAULT( reqId, "displayGroupUpdated" ) }
	//---- Extra Methods -----------------------------------------------------

	// To keep symmetry with other requests that have an End method:
	// openOrderEnd, accountDownloadEnd, contractDetailsEnd, execDetailsEnd, scannerDataEnd, tickSnapshotEnd
//	virtual void historicalDataEnd     ( TickerId reqId ) {}

	// To keep symmetry with connectionClosed().
	// Is called just before the eConnect returns when connecting succeeded.
	virtual void connectionOpened( void ) {}

	static const char* Finished( void ) { return "finished"; }

	// The flow of historical data ends with a row holding a date field == 'finished'
	static bool IsEndOfHistoricalData( const IBString& Date )
	{
		return 0 == strncmp( (const char*)Date.data(), Finished(), 8 );
	}

	// The internal protection system reports via this method an unhandled exception
	// The Id is the tickerId, the orderId, or the reqId, or -1 when no id known
	virtual void OnCatch( const char* MethodName, const long Id )
	{
		fprintf( stderr, "*** Catch in EWrapper::%s( Id=%ld, ...) \n", MethodName, Id );
	}

	//---- To be Threaded or not to be Threaded-------------------------------
	// This allows the choice between a treaded or non threaded execution.
	// When non threaded, one must call checkMessages periodically.
	// The choice is made during creation only! Default it is set to true.
	// The users derived EWrapper must define a similar constructor in order to
	// set it to non-threaded, or just change the default to false.
	EWrapperL0( bool CalledFromThread = true ): m_CalledFromThread( CalledFromThread ) {}
	bool IsCalledFromThread( void ) { return m_CalledFromThread; }

protected:
	bool	m_CalledFromThread;
};

#undef EWRAPPERL0_DEFAULT


//----------------------------------------------------------------------------
#endif	// _TwsApiL0_h
