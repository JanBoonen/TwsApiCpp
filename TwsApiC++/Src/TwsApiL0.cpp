/*===========================================================================*\
TwsApiL0.cpp
\*===========================================================================*/
#include "TwsApiL0.h"

#if 0
//Old version
#define TwsApiDefsImpl		// Allocate the string values
#include "TwsApiDefs.h"
#else
//New version
#include "TwsApiDefs.h"
#undef   _TwsApiDefs_h
#define ENUMImplementation
#include "TwsApiDefs.h"
#endif


#ifndef WIN32
	#include <sys/socket.h>	//
#endif
// (4 march 2013) JBOO #define replaced by friend class EClientL0Impl
// #define private public				// to get access to send and receive
#include "EPosixClientSocket.cpp"
#include "EClientSocketBase.cpp"	//	CLIENT_VERSION
#undef private

#ifdef WIN32
#ifndef CorrectAfxWinIncluded
#error Not the correct afxwin.h included.
#endif
#endif


#ifdef WIN32
	#include <windows.h>	// for mutex
	#include <process.h>	// for Sleep
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <pthread.h>	// for mutex
	#define Sleep( m ) usleep( m*1000 )
	#define CurrentThreadId pthread_self
#endif


#define _TRY_    try{

#ifdef _DEBUG
#define _CATCH_  }catch(...){ fprintf( stderr, "*** CATCH: in file %s, at %d ***\n", __FILE__, __LINE__ ); }
#define ENTERCRITICALSECTION( CSobject ) { bool b=true; try{ CSobject EnterCriticalSection(); } catch(...) { printf("***CS*** %s %d\n",__FILE__,__LINE__); b = false; } if ( b ) {
#define LEAVECRITICALSECTION( CSobject ) CSobject LeaveCriticalSection(); } }

#else
#define _CATCH_  }catch(...){}
#define ENTERCRITICALSECTION(CSobject) CSobject EnterCriticalSection();
#define LEAVECRITICALSECTION(CSobject) CSobject LeaveCriticalSection();

#endif


//----------------------------------------------------------------------------
// CriticalSection
// Less expensive than Mutex, but sufficient as interprocess isn't needed.
//----------------------------------------------------------------------------
struct CriticalSection
{
#ifdef WIN32
public:
	CRITICAL_SECTION	m_CriticalSection;

	CriticalSection		( void ) { ::InitializeCriticalSection	(&m_CriticalSection); }
	~CriticalSection	( void ) { ::DeleteCriticalSection		(&m_CriticalSection); }

#ifdef _DEBUG
	// Put in a seperate method to allow breakpoint in debugger
	// While debugging, shouldn't it always have a breakpoint set
	// The throw(0) on purpose for the ENTERCRITICALSECTION() macro
	void DeadlockEncountered( void ) { throw(0); }
	void EnterCriticalSection   ( void ) { try{ ::EnterCriticalSection(&m_CriticalSection); } catch(...) { DeadlockEncountered(); } }
#else
	void EnterCriticalSection   ( void ) { ::EnterCriticalSection(&m_CriticalSection); }
#endif

	void LeaveCriticalSection   ( void ) { ::LeaveCriticalSection(&m_CriticalSection); }

	bool TryEnterCriticalSection( void ) { return ::TryEnterCriticalSection(&m_CriticalSection) != 0; }
#else
	pthread_mutex_t	m_mutex;

	CriticalSection		( void ) :m_mutex (PTHREAD_MUTEX_INITIALIZER) {}

	~CriticalSection	( void ) { ::pthread_mutex_destroy( &m_mutex ); }

	bool EnterCriticalSection   ( void ) { return ::pthread_mutex_lock  ( &m_mutex ) == 0; }
	void LeaveCriticalSection   ( void ) { ::pthread_mutex_unlock( &m_mutex ); }

	bool TryEnterCriticalSection( void ) { return ::pthread_mutex_trylock( &m_mutex ) == 0; }
#endif
};

//----------------------------------------------------------------------------
// EWrapperL0Impl
// * Protects TwsApi from unhandled exceptions in the users EWrapper
// * Allows the EClientL0Impl to disables the calls just before eDisconnect()
//----------------------------------------------------------------------------
#define EW_TRYCATCH( N, I, F ) \
	try { if( m_EW && m_Enabled ) m_EW->F; } catch( ... ) { _TRY_ m_EW->OnCatch( #N, I ); _CATCH_ }

class EWrapperL0Impl: public EWrapperL0
{
protected:
	EWrapperL0*		m_EW;
	bool			m_Enabled;		// When false, calls are not passed on
	EClient*		m_Client;		// Access to the logger

public:
	EWrapperL0Impl( EWrapperL0* EW )
	: m_EW			( EW	)
	, m_Enabled		( true	)
	, m_Client		( 0		)
	{}

	void		EnableCalls		( void ) { m_Enabled = true;	}
	void		DisableCalls	( void ) { m_Enabled = false;	}
	bool		IsEnableCalls	( void ) { return m_Enabled;	}

	void		SetClient		( EClient* Client ) { m_Client = Client; }


	// Wrap each call in a try/catch to protect the API client (==EClientSocket)
	virtual void tickPrice             ( TickerId tickerId, TickType field, double price, int canAutoExecute )
	{EW_TRYCATCH(tickPrice             , tickerId, tickPrice( tickerId, field, price, canAutoExecute ); ) }

	virtual void tickSize              ( TickerId tickerId, TickType field, int size )
	{EW_TRYCATCH(tickSize              , tickerId, tickSize( tickerId, field, size ); ) }

	virtual void tickOptionComputation ( TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice)
	{EW_TRYCATCH(tickOptionComputation , tickerId, tickOptionComputation ( tickerId, tickType, impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice ); ) }


	virtual void tickGeneric           ( TickerId tickerId, TickType tickType, double value )
	{EW_TRYCATCH(tickGeneric           , tickerId, tickGeneric( tickerId, tickType, value ); ) }

	virtual void tickString            ( TickerId tickerId, TickType tickType, const IBString& value )
	{EW_TRYCATCH(tickString            , tickerId, tickString( tickerId, tickType, value ); ) }

	virtual void tickEFP               ( TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints, double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry)
	{EW_TRYCATCH(tickEFP               , tickerId, tickEFP( tickerId, tickType, basisPoints, formattedBasisPoints, totalDividends, holdDays, futureExpiry, dividendImpact, dividendsToExpiry); ) }

	virtual void orderStatus           ( OrderId orderId, const IBString& status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString& whyHeld )
	{EW_TRYCATCH(orderStatus           , orderId, orderStatus( orderId, status, filled, remaining, avgFillPrice, permId, parentId, lastFillPrice, clientId, whyHeld ); ) }

	virtual void openOrder             ( OrderId orderId, const Contract& contract, const Order& order, const OrderState& orderState )
	{EW_TRYCATCH(openOrder             , orderId, openOrder( orderId, contract, order, orderState ); ) }

	virtual void openOrderEnd          ()
	{EW_TRYCATCH(openOrderEnd          , NO_VALID_ID, openOrderEnd(); ) }

	virtual void winError              ( const IBString& str, int lastError )
	{EW_TRYCATCH(winError              , NO_VALID_ID, winError( str, lastError ); ) }

	virtual void connectionClosed      ()
	{EW_TRYCATCH(connectionClosed      , NO_VALID_ID, connectionClosed(); ) }

	virtual void updateAccountValue    ( const IBString& key, const IBString& val, const IBString& currency, const IBString& accountName )
	{EW_TRYCATCH(updateAccountValue    , NO_VALID_ID, updateAccountValue( key, val, currency, accountName ); ) }

	virtual void updatePortfolio       ( const Contract& contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString& accountName )
	{EW_TRYCATCH(updatePortfolio       , NO_VALID_ID, updatePortfolio( contract, position, marketPrice, marketValue, averageCost, unrealizedPNL, realizedPNL, accountName ); ) }

	virtual void updateAccountTime     ( const IBString& timeStamp )
	{EW_TRYCATCH(updateAccountTime     , NO_VALID_ID, updateAccountTime( timeStamp ); ) }

	virtual void accountDownloadEnd    ( const IBString& accountName )
	{EW_TRYCATCH(accountDownloadEnd    , NO_VALID_ID, accountDownloadEnd( accountName ); ) }

	virtual void nextValidId           ( OrderId orderId )
	{EW_TRYCATCH(nextValidId           , orderId, nextValidId( orderId ); ) }

	virtual void contractDetails       ( int reqId, const ContractDetails& contractDetails )
	{EW_TRYCATCH(contractDetails       , reqId, contractDetails( reqId, contractDetails ); ) }

	virtual void bondContractDetails   ( int reqId, const ContractDetails& contractDetails )
	{EW_TRYCATCH(bondContractDetails   , reqId, bondContractDetails( reqId, contractDetails ); ) }

	virtual void contractDetailsEnd    ( int reqId )
	{EW_TRYCATCH(contractDetailsEnd    , reqId, contractDetailsEnd( reqId ); ) }

	virtual void execDetails           ( int reqId, const Contract& contract, const Execution& execution )
	{EW_TRYCATCH(execDetails           , reqId, execDetails( reqId, contract, execution ); ) }

	virtual void execDetailsEnd        ( int reqId )
	{EW_TRYCATCH(execDetailsEnd        , reqId, execDetailsEnd( reqId ); ) }

	virtual void error                 ( const int id, const int errorCode, const IBString errorString )
	{EW_TRYCATCH(error                 , id, error( id, errorCode, errorString ); ) }

	virtual void updateMktDepth        ( TickerId id, int position, int operation, int side, double price, int size )
	{EW_TRYCATCH(updateMktDepth        , id, updateMktDepth( id, position, operation, side, price, size ); ) }

	virtual void updateMktDepthL2      ( TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size )
	{EW_TRYCATCH(updateMktDepthL2      , id, updateMktDepthL2( id, position, marketMaker, operation, side, price, size ); ) }

	virtual void updateNewsBulletin    ( int msgId, int msgType, const IBString& newsMessage, const IBString& originExch )
	{EW_TRYCATCH(updateNewsBulletin    , msgId, updateNewsBulletin( msgId, msgType, newsMessage, originExch ); ) }

	virtual void managedAccounts       ( const IBString& accountsList )
	{EW_TRYCATCH(managedAccounts       , NO_VALID_ID, managedAccounts( accountsList ); ) }

	virtual void receiveFA             ( faDataType pFaDataType, const IBString& cxml )
	{EW_TRYCATCH(receiveFA             , NO_VALID_ID, receiveFA( pFaDataType, cxml ); ) }

	virtual void historicalData        ( TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps )
	{EW_TRYCATCH(historicalData        , reqId, historicalData( reqId, date, open, high, low, close, volume, barCount, WAP, hasGaps ); ) }

	virtual void scannerParameters     ( const IBString& xml )
	{EW_TRYCATCH(scannerParameters     , NO_VALID_ID, scannerParameters( xml ); ) }

	virtual void scannerData           ( int reqId, int rank, const ContractDetails &contractDetails, const IBString &distance, const IBString &benchmark, const IBString &projection, const IBString &legsStr)
	{EW_TRYCATCH(scannerData           , reqId, scannerData( reqId, rank, contractDetails, distance, benchmark, projection, legsStr); ) }

	virtual void scannerDataEnd        ( int reqId)
	{EW_TRYCATCH(scannerDataEnd        , reqId, scannerDataEnd( reqId); ) }

	virtual void realtimeBar           ( TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count)
	{EW_TRYCATCH(realtimeBar           , reqId, realtimeBar( reqId, time, open, high, low, close, volume, wap, count); ) }

	virtual void currentTime           ( long time )
	{EW_TRYCATCH(currentTime           , NO_VALID_ID, currentTime( time ); ) }

	virtual void fundamentalData       ( TickerId reqId, const IBString& data )
	{EW_TRYCATCH(fundamentalData       , reqId, fundamentalData( reqId, data ); ) }

	virtual void deltaNeutralValidation( int reqId, const UnderComp& underComp )
	{EW_TRYCATCH(deltaNeutralValidation, reqId, deltaNeutralValidation( reqId, underComp ); ) }

	virtual void tickSnapshotEnd       ( int reqId )
	{EW_TRYCATCH(tickSnapshotEnd       , reqId, tickSnapshotEnd( reqId ); ) }

	virtual void marketDataType        ( TickerId reqId, int marketDataType)
	{EW_TRYCATCH(marketDataType        , reqId, marketDataType( reqId, marketDataType ); ) }

	virtual void commissionReport      ( const CommissionReport &commissionReport)
	{EW_TRYCATCH(commissionReport      , NO_VALID_ID, commissionReport( commissionReport ); ) }

	virtual void position              ( const IBString& account, const Contract& contract, int position, double avgCost)
	{ EW_TRYCATCH(position             , NO_VALID_ID, position( account, contract, position, avgCost); ) }

	virtual void positionEnd           ()
	{ EW_TRYCATCH(positionEnd          , NO_VALID_ID, positionEnd(); ) }

	virtual void accountSummary        ( int reqId, const IBString& account, const IBString& tag, const IBString& value, const IBString& curency)
	{ EW_TRYCATCH(accountSummary       , reqId, accountSummary( reqId, account, tag, value, curency); ) }

	virtual void accountSummaryEnd     ( int reqId)
	{ EW_TRYCATCH(accountSummaryEnd    , reqId, accountSummaryEnd( reqId); ) }

	virtual void verifyMessageAPI      ( const IBString& apiData)
	{ EW_TRYCATCH(verifyMessageAPI     , NO_VALID_ID, verifyMessageAPI( apiData); ) }

	virtual void verifyCompleted       ( bool isSuccessful, const IBString& errorText)
	{ EW_TRYCATCH(verifyCompleted      , NO_VALID_ID, verifyCompleted( isSuccessful, errorText); ) }

	virtual void displayGroupList      (  int reqId, const IBString& groups)
	{ EW_TRYCATCH(displayGroupList     , reqId, displayGroupList(  reqId, groups); ) }

	virtual void displayGroupUpdated   ( int reqId, const IBString& contractInfo)
	{ EW_TRYCATCH(displayGroupUpdated  , reqId, displayGroupUpdated( reqId, contractInfo); ) }

	//---- Extra Methods -----------------------------------------------------
	virtual void connectionOpened( void )
	{EW_TRYCATCH(connectionOpened      , NO_VALID_ID, connectionOpened( ); ) }

};
#undef EW_TRYCATCH


//----------------------------------------------------------------------------
// ThreadMain
//----------------------------------------------------------------------------
void ThreadMain( void* pEClient );	// implementation at end of file


//----------------------------------------------------------------------------
// EClientL0Impl
//
//----------------------------------------------------------------------------
struct EClientL0Impl: public EPosixClientSocket
{
	EWrapperL0Impl	m_EWrapperL0Impl;	// the wrapper called by EPosixClientSocket
	EWrapperL0*		m_EWrapperL0;		// Just to have pointer to user specified EWrapper
	int				m_ClientId;
	bool			m_ThreadIsRunning;
	time_t			m_NextHeartbeat;

	EClientL0Impl( EWrapperL0* EW )
	: EPosixClientSocket				( &m_EWrapperL0Impl )
	, m_EWrapperL0Impl					( EW	)
	, m_EWrapperL0						( EW	)

	, m_ClientId						( NO_VALID_ID	)

	, m_ThreadIsRunning					( false	)
	, m_NextHeartbeat					( 0		)
	{
		m_EWrapperL0Impl.SetClient( this );
	}

	~EClientL0Impl( void )
	{
		eDisconnect();
	}

	bool eConnect( const char *host, unsigned int port, int clientId=0, bool extraAuth=0 )
	{
		m_EWrapperL0Impl.EnableCalls();	// just to ensure it is enabled

		// If connected, just let it follow the flow in EClientSocket
		if( isConnected() )
			return EPosixClientSocket::eConnect(host, port, clientId, extraAuth);

		if( !EPosixClientSocket::eConnect(host, port, clientId, extraAuth) )
		{
			EPosixClientSocket::eDisconnect();	// otherwise, the m_fd > 0
			return false;
		}

		m_EWrapperL0Impl.connectionOpened();

		if( m_EWrapperL0->IsCalledFromThread() )
		{	// Then start the reading thread
#ifdef WIN32
			int err = _beginthread( ThreadMain, 2*8192, this );
			if( err < 0 )
				// errno == EAGAIN // there are too many threads
				// errno == EINVAL // the argument is invalid or the stack size is incorrect
				// errno == EACCES // insufficient resources (such as memory)
			{
				m_EWrapperL0->error( errno, COULD_NOT_START_THREAD.code(), COULD_NOT_START_THREAD.msg());
#else
			pthread_t thr;
			int err = pthread_create(&thr, 0, (void *(*)(void *)) ThreadMain, this);
			if( err != 0 )
				// err == EAGAIN // Insufficient resources to create another thread
				// err == EINVAL // Invalid settings in attr.
				// err == EPERM  // No permission to set the scheduling policy
			{	// if err
				m_EWrapperL0->error( err, COULD_NOT_START_THREAD.code(), COULD_NOT_START_THREAD.msg());
#endif
				eDisconnect();
				return false;
			}
		}

		return true;
	}



	void eDisconnect()
	{
		if( !isConnected() && !m_ThreadIsRunning )
			return;

		_TRY_
		// if threaded, try to stop the thread gracefully ...
		// * EWrapper calls are disabled to spend no time in users code while
		//   checkMessages parses the current buffer
		// * The receive method prevents new data will be presented to
		//   checkMessages()
		// * Execution is passed to another thread for max 100 times, hoping
		//   in the meantime the current running checkMessages() returns and
		//   checkMessagesLoop() can end and so can the thread.
		if( m_ThreadIsRunning )
		{
			m_EWrapperL0Impl.DisableCalls();	// will stop the thread loop
			_TRY_
			for( int i = 100; i > 0 && m_ThreadIsRunning; i-- ) { Sleep(1); }

			if( m_ThreadIsRunning )
			{
				/* troubles ahead!? */
			}
			_CATCH_
			m_EWrapperL0Impl.EnableCalls();
		}

		EPosixClientSocket::eDisconnect();
		_CATCH_

		m_EWrapperL0Impl.connectionClosed();
	}


    virtual int receive(char* buf, size_t sz)
	{
		// no point to process new data when system is disconnecting
		if( !m_EWrapperL0Impl.IsEnableCalls() )
			return 0;

//		return EPosixClientSocket::receive( buf, sz );
		if( sz <= 0)
			return 0;

		int nResult = ::recv( fd(), buf, sz, 0);

		// Seems the only way to discover TWS was shut down?  See comment on ErrorSet.
		if( nResult < 0 )
			eDisconnect();

		return nResult;
	}


    virtual int send(const char* buf, size_t sz)
	{
		return EPosixClientSocket::send( buf, sz );
	}


	virtual bool isSocketOK() const { return EPosixClientSocket::isSocketOK(); }


	//---- Extra Methods -----------------------------------------------------
    virtual EWrapper*	GetEWrapper		() { return m_EWrapperL0;		}
    virtual int			clientVersion	() { return CLIENT_VERSION;		}
    virtual bool		IsConnected		() { return isConnected();		}



	// Block the checkMessages() call from the EClient API when threaded
	// Note: checkMessages() is called in EPosixClientSocket::eConnect
	// before isConnected() == true!
	bool checkMessages()
	{
		if( IsConnected() && time(0) > m_NextHeartbeat )
		{
			bufferedSend("\0",1);			// Sending a 0 byte helps the socket detect its closing by TWS
			m_NextHeartbeat = time(0)+1;	// Wait +/- 1 second for next heartbeat
		}

		if( !isSocketOK() )
			return false;

		if( m_EWrapperL0->IsCalledFromThread() && isConnected() )
			return false;

		return checkMessagesOnce(1);
	}

	bool checkMessagesOnce( long waitfor = 0 )
	{
		_TRY_
	//	fprintf( stderr, "ThreadId = %d    \r", CurrentThreadId() );

		int	SH = fd();	// Socket Handle

		fd_set ReadSet ; FD_ZERO( &ReadSet  ); FD_SET ( SH, &ReadSet  );
		fd_set WriteSet; FD_ZERO( &WriteSet ); if( !isOutBufferEmpty() ) FD_SET ( SH, &WriteSet );
		fd_set ErrorSet; FD_ZERO( &ErrorSet ); FD_SET ( SH, &ErrorSet );

		struct timeval timeout = {0,waitfor};	// wait for some milli/micro seconds
		int s = select( SH+1, &ReadSet, &WriteSet, &ErrorSet, &timeout );
				s = s; // turn of -Wall warning


		// The ErrorSet doesn't seem to do a lot on windows?
		// Even if TWS is shut down, ErrorSet is cleared as nothing is wrong?
		if( FD_ISSET(SH, &ErrorSet) )	// error on socket
			return handleSocketError();

		if( FD_ISSET(SH, &WriteSet) && isSocketOK() )	// socket is ready for writing
			onSend();

		if( FD_ISSET(SH, &ReadSet ) && isSocketOK() )	// socket is ready for reading
			return EPosixClientSocket::checkMessages();

		_CATCH_	// EPosixClientSocket::checkMessages() can throw an exception!

		// Nothing done, but nothing wrong either
		return true;
	}

	void checkMessagesLoop ( void )
	{
		m_ThreadIsRunning = true;

		_TRY_
		while( isSocketOK() && isConnected() && m_EWrapperL0Impl.IsEnableCalls() )
		{
			checkMessagesOnce(1);	// the minimal time possible to wait is enough
		}							// to prevent this loop using 100% of the cpu
		_CATCH_

		m_ThreadIsRunning = false;
	}

};	// EClientL0Impl


//----------------------------------------------------------------------------
// ThreadMain
//----------------------------------------------------------------------------
void ThreadMain( void* pEClient )
{
	_TRY_ ((EClientL0Impl*)pEClient)->checkMessagesLoop(); _CATCH_
}



//----------------------------------------------------------------------------
// Interface to EClientL0
// Implements the interface as a call through to EClientL0Impl
// The critical section is needed because the EWrapper running in another
// thread can call EClient too, and the EClientSocketBase::bufferedSend() is
// not thread safe as it sets EClientSocketBase members.
//----------------------------------------------------------------------------
#define ENTER_CLIENT( x, i ) ENTERCRITICALSECTION(this->)
#define LEAVE_CLIENT( x    ) LEAVECRITICALSECTION(this->)


struct EClientL0Interface: public EClientL0, CriticalSection
{
	EClientL0Impl	EC;

	EClientL0Interface( EWrapperL0* EW ) :EC( EW ) {}


	bool eConnect                               ( const char *host, unsigned int port, int clientId=0, bool extraAuth=0)
    { bool r;
	  ENTER_CLIENT( eConnect                    , NO_VALID_ID )
      r =        EC.eConnect                    ( host, port, clientId);
      LEAVE_CLIENT( eConnect                    )
	  return r    ;                             }

    void eDisconnect                            ()
    { ENTER_CLIENT( eDisconnect                 , NO_VALID_ID )
                 EC.eDisconnect                 ();
      LEAVE_CLIENT( eDisconnect                 ) }

    int serverVersion                           ()
    { int r;
	  ENTER_CLIENT( serverVersion               , NO_VALID_ID )
      r =        EC.serverVersion               ();
      LEAVE_CLIENT( serverVersion               )
      return r;                                 }

    IBString TwsConnectionTime                  ()
    { IBString s;
	  ENTER_CLIENT( TwsConnectionTime           , NO_VALID_ID )
      s =        EC.TwsConnectionTime           ();
      LEAVE_CLIENT( TwsConnectionTime           )
      return s;                                 }

    void reqMktData                             ( TickerId id, const Contract &contract, const IBString& genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions)
    { ENTER_CLIENT( reqMktData                  , id )
                 EC.reqMktData                  ( id, contract, genericTicks, snapshot, mktDataOptions);
      LEAVE_CLIENT( reqMktData                  ) }

    void cancelMktData                          ( TickerId id)
    { ENTER_CLIENT( cancelMktData               , id )
                 EC.cancelMktData               ( id);
      LEAVE_CLIENT( cancelMktData               ) }

    void placeOrder                             ( OrderId id, const Contract &contract, const Order &order)
    { ENTER_CLIENT( placeOrder                  , id )
                 EC.placeOrder                  ( id, contract, order);
      LEAVE_CLIENT( placeOrder                  ) }

    void cancelOrder                            ( OrderId id)
    { ENTER_CLIENT( cancelOrder                 , id )
                 EC.cancelOrder                 ( id);
      LEAVE_CLIENT( cancelOrder                 ) }

    void reqOpenOrders                          ()
    { ENTER_CLIENT( reqOpenOrders               , NO_VALID_ID )
                 EC.reqOpenOrders               ();
      LEAVE_CLIENT( reqOpenOrders               ) }

    void reqAccountUpdates                      (bool subscribe, const IBString& acctCode)
    { ENTER_CLIENT( reqAccountUpdates           , NO_VALID_ID )
                 EC.reqAccountUpdates           (subscribe, acctCode);
      LEAVE_CLIENT( reqAccountUpdates           ) }

    void reqExecutions                          (int reqId, const ExecutionFilter& filter)
    { ENTER_CLIENT( reqExecutions               , reqId )
                 EC.reqExecutions               ( reqId, filter);
      LEAVE_CLIENT( reqExecutions               ) }

    void reqIds                                 ( int numIds)
    { ENTER_CLIENT( reqIds                      , NO_VALID_ID )
                 EC.reqIds                      ( numIds);
      LEAVE_CLIENT( reqIds                      ) }

    bool checkMessages                          ()
    { bool r;
	  ENTERCRITICALSECTION(this->);	// without logging
      r =        EC.checkMessages               ();
      LEAVECRITICALSECTION(this->);	// without logging
      return r;                                 }

    void reqContractDetails                     ( int reqId, const Contract &contract)
    { ENTER_CLIENT( reqContractDetails          , reqId )
                 EC.reqContractDetails          ( reqId, contract);
      LEAVE_CLIENT( reqContractDetails          ) }

    void reqMktDepth                            ( TickerId id, const Contract &contract, int numRows, const TagValueListSPtr& mktDepthOptions)
    { ENTER_CLIENT( reqMktDepth                 , id )
                 EC.reqMktDepth                 ( id, contract, numRows, mktDepthOptions);
      LEAVE_CLIENT( reqMktDepth                 ) }

    void cancelMktDepth                         ( TickerId id)
    { ENTER_CLIENT( cancelMktDepth              , id )
                 EC.cancelMktDepth              ( id);
      LEAVE_CLIENT( cancelMktDepth              ) }

    void reqNewsBulletins                       ( bool allMsgs)
    { ENTER_CLIENT( reqNewsBulletins            , NO_VALID_ID )
                 EC.reqNewsBulletins            ( allMsgs);
      LEAVE_CLIENT( reqNewsBulletins            ) }

    void cancelNewsBulletins                    ()
    { ENTER_CLIENT( cancelNewsBulletins         , NO_VALID_ID )
                 EC.cancelNewsBulletins         ();
      LEAVE_CLIENT( cancelNewsBulletins         ) }

    void setServerLogLevel                      (int level)
    { ENTER_CLIENT( setServerLogLevel           , NO_VALID_ID )
                 EC.setServerLogLevel           (level);
      LEAVE_CLIENT( setServerLogLevel           ) }

    void reqAutoOpenOrders                      (bool bAutoBind)
    { ENTER_CLIENT( reqAutoOpenOrders           , NO_VALID_ID )
                 EC.reqAutoOpenOrders           (bAutoBind);
      LEAVE_CLIENT( reqAutoOpenOrders           ) }

    void reqAllOpenOrders                       ()
    { ENTER_CLIENT( reqAllOpenOrders            , NO_VALID_ID )
                 EC.reqAllOpenOrders            ();
      LEAVE_CLIENT( reqAllOpenOrders            ) }

    void reqManagedAccts                        ()
    { ENTER_CLIENT( reqManagedAccts             , NO_VALID_ID )
                 EC.reqManagedAccts             ();
      LEAVE_CLIENT( reqManagedAccts             ) }

    void requestFA                              (faDataType pFaDataType)
    { ENTER_CLIENT( requestFA                   , NO_VALID_ID )
                 EC.requestFA                   (pFaDataType);
      LEAVE_CLIENT( requestFA                   ) }

    void replaceFA                              (faDataType pFaDataType, const IBString& cxml)
    { ENTER_CLIENT( replaceFA                   , NO_VALID_ID )
                 EC.replaceFA                   (pFaDataType, cxml);
      LEAVE_CLIENT( replaceFA                   ) }

    void reqHistoricalData                      ( TickerId id, const Contract &contract, const IBString &endDateTime, const IBString &durationStr, const IBString &barSizeSetting, const IBString &whatToShow, int useRTH, int formatDate, const TagValueListSPtr& chartOptions)
    { ENTER_CLIENT( reqHistoricalData           , id )
                 EC.reqHistoricalData           ( id, contract, endDateTime, durationStr, barSizeSetting, whatToShow, useRTH, formatDate, chartOptions);
      LEAVE_CLIENT( reqHistoricalData           ) }

    void exerciseOptions                        ( TickerId id, const Contract &contract, int exerciseAction, int exerciseQuantity, const IBString &account, int override)
    { ENTER_CLIENT( exerciseOptions             , id )
                 EC.exerciseOptions             ( id, contract, exerciseAction, exerciseQuantity, account, override);
      LEAVE_CLIENT( exerciseOptions             ) }

    void cancelHistoricalData                   ( TickerId tickerId )
    { ENTER_CLIENT( cancelHistoricalData        , tickerId )
                 EC.cancelHistoricalData        ( tickerId );
      LEAVE_CLIENT( cancelHistoricalData        ) }

    void reqRealTimeBars                        ( TickerId id, const Contract &contract, int barSize, const IBString &whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions)
    { ENTER_CLIENT( reqRealTimeBars             , id )
                 EC.reqRealTimeBars             ( id, contract, barSize, whatToShow, useRTH, realTimeBarsOptions);
      LEAVE_CLIENT( reqRealTimeBars             ) }

    void cancelRealTimeBars                     ( TickerId tickerId)
    { ENTER_CLIENT( cancelRealTimeBars          , tickerId )
                 EC.cancelRealTimeBars          ( tickerId);
      LEAVE_CLIENT( cancelRealTimeBars          ) }

    void cancelScannerSubscription              ( int tickerId)
    { ENTER_CLIENT( cancelScannerSubscription   , tickerId )
                 EC.cancelScannerSubscription   ( tickerId);
      LEAVE_CLIENT( cancelScannerSubscription   ) }

    void reqScannerParameters                   ()
    { ENTER_CLIENT( reqScannerParameters        , NO_VALID_ID )
                 EC.reqScannerParameters        ();
      LEAVE_CLIENT( reqScannerParameters        ) }

    void reqScannerSubscription                 ( int tickerId, const ScannerSubscription &subscription, const TagValueListSPtr& scannerSubscriptionOptions)
    { ENTER_CLIENT( reqScannerSubscription      , tickerId )
                 EC.reqScannerSubscription      ( tickerId, subscription, scannerSubscriptionOptions);
      LEAVE_CLIENT( reqScannerSubscription      ) }

    void reqCurrentTime                         ()
    { ENTER_CLIENT( reqCurrentTime              , NO_VALID_ID )
                 EC.reqCurrentTime              ();
      LEAVE_CLIENT( reqCurrentTime              ) }

    void reqFundamentalData                     ( TickerId reqId, const Contract& contract, const IBString& reportType)
    { ENTER_CLIENT( reqFundamentalData          , reqId )
                 EC.reqFundamentalData          ( reqId, contract, reportType);
      LEAVE_CLIENT( reqFundamentalData          ) }

    void cancelFundamentalData                  ( TickerId reqId)
    { ENTER_CLIENT( cancelFundamentalData       , reqId )
                 EC.cancelFundamentalData       ( reqId);
      LEAVE_CLIENT( cancelFundamentalData       ) }

	virtual void calculateImpliedVolatility     ( TickerId reqId, const Contract &contract, double optionPrice, double underPrice)
    { ENTER_CLIENT( calculateImpliedVolatility  , reqId )
                 EC.calculateImpliedVolatility  ( reqId, contract, optionPrice, underPrice);
      LEAVE_CLIENT( calculateImpliedVolatility  ) }

	virtual void calculateOptionPrice           ( TickerId reqId, const Contract &contract, double volatility, double underPrice)
    { ENTER_CLIENT( calculateOptionPrice        , reqId )
                 EC.calculateOptionPrice        ( reqId, contract, volatility, underPrice);
      LEAVE_CLIENT( calculateOptionPrice        ) }

	virtual void cancelCalculateImpliedVolatility( TickerId reqId)
    { ENTER_CLIENT( cancelCalculateImpliedVolatility, reqId )
                 EC.cancelCalculateImpliedVolatility( reqId);
      LEAVE_CLIENT( cancelCalculateImpliedVolatility) }

	virtual void cancelCalculateOptionPrice     ( TickerId reqId)
    { ENTER_CLIENT( cancelCalculateOptionPrice  , reqId )
                 EC.cancelCalculateOptionPrice  ( reqId);
      LEAVE_CLIENT( cancelCalculateOptionPrice  ) }

    void reqGlobalCancel                        ()
    { ENTER_CLIENT( reqGlobalCancel             , NO_VALID_ID )
                 EC.reqGlobalCancel             ();
      LEAVE_CLIENT( reqGlobalCancel             ) }

   void reqMarketDataType                       ( int marketDataType)
    { ENTER_CLIENT( reqMarketDataType           , NO_VALID_ID )
                 EC.reqMarketDataType           ( marketDataType);
      LEAVE_CLIENT( reqMarketDataType           ) }

   void reqPositions                            ()
    { ENTER_CLIENT( reqPositions                , NO_VALID_ID )
                 EC.reqPositions                ();
      LEAVE_CLIENT( reqPositions                ) }

   void cancelPositions                         ()
    { ENTER_CLIENT( cancelPositions             , NO_VALID_ID )
                 EC.cancelPositions             ();
      LEAVE_CLIENT( cancelPositions             ) }

   void reqAccountSummary                       ( int reqId, const IBString& groupName, const IBString& tags)
    { ENTER_CLIENT( reqAccountSummary           , reqId )
                 EC.reqAccountSummary           ( reqId, groupName, tags);
      LEAVE_CLIENT( reqAccountSummary           ) }

   void cancelAccountSummary                    ( int reqId)
    { ENTER_CLIENT( cancelAccountSummary        , reqId )
                 EC.cancelAccountSummary        ( reqId);
      LEAVE_CLIENT( cancelAccountSummary        ) }

   void verifyRequest                           ( const IBString& apiName, const IBString& apiVersion)
    { ENTER_CLIENT( verifyRequest               , NO_VALID_ID )
                 EC.verifyRequest               ( apiName, apiVersion);
      LEAVE_CLIENT( verifyRequest               ) }

   void verifyMessage                           ( const IBString& apiData)
    { ENTER_CLIENT( verifyMessage               , NO_VALID_ID )
                 EC.verifyMessage               ( apiData);
      LEAVE_CLIENT( verifyMessage               ) }

   void queryDisplayGroups                      ( int reqId)
    { ENTER_CLIENT( queryDisplayGroups          , reqId )
                 EC.queryDisplayGroups          ( reqId);
      LEAVE_CLIENT( queryDisplayGroups          ) }

   void subscribeToGroupEvents                  ( int reqId, int groupId)
    { ENTER_CLIENT( subscribeToGroupEvents      , reqId )
                 EC.subscribeToGroupEvents      ( reqId, groupId);
      LEAVE_CLIENT( subscribeToGroupEvents      ) }

   void updateDisplayGroup                      ( int reqId, const IBString& contractInfo)
    { ENTER_CLIENT( updateDisplayGroup          , reqId )
                 EC.updateDisplayGroup          ( reqId, contractInfo);
      LEAVE_CLIENT( updateDisplayGroup          ) }

   void unsubscribeFromGroupEvents              ( int reqId)
    { ENTER_CLIENT( unsubscribeFromGroupEvents  , reqId )
                 EC.unsubscribeFromGroupEvents  ( reqId);
      LEAVE_CLIENT( unsubscribeFromGroupEvents  ) }

	//---- Extra Methods -----------------------------------------------------
    virtual EWrapper*	GetEWrapper			( void ) { return EC.GetEWrapper		(); }
    virtual int			clientVersion		( void ) { return EC.clientVersion		(); }
    virtual bool		IsConnected			( void ) { return EC.IsConnected		(); }
};


//----------------------------------------------------------------------------
// EClientL0::New()
//----------------------------------------------------------------------------
EClientL0* EClientL0::New( EWrapperL0* EW )
{
	return (EClientL0*)new EClientL0Interface( EW );
}

