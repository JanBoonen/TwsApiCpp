//============================================================================
// Using TwsApiDefs.h is optional, but include it after TwsApiLx.h!!
// Its goal is to discover coding errors due to misspelling during compilation
// instead of during th execution of the program.  Using ames instead of
// strings or numberic values enables this.  It improves readability as well.
//
// Use the enumeration when the type of the numeric (or bool) value is needed.
// Use the * operator in front of the enumeration if the string is needed.
// I.e calling 
//   reqRealTimeBars( TickerId id, const Contract &contract, int barSize,
//	   const IBString &whatToShow, bool useRTH)
// can be done as follows
//  reqRealTimeBars( id, contract, BarSizeSetting::_5_secs
//     , *WhatToShow::TRADES, UseRTH::OnlyRegularTradingData );
// Instead of
//  reqRealTimeBars( id, contract, "5 secs", "Trades", true );
//
//============================================================================

#ifndef _TwsApiDefs_h
#define _TwsApiDefs_h

#include "Enumerations.h"


namespace TwsApi
{
	//------------------------------------------------------------------------
	// Exchange list
	// Copied from the list of exchanges where IB specifies how certain orders
	// are supported, native or simulated.
	// Seems to be NOT ALL the possible exchanges (i.e. NYCE )
	// Note: as the case doesn't matter, all are converted to uppercase.
	//------------------------------------------------------------------------
	ENUMValues( Exchange )
		ENUM_S( IB_SMART     , "SMART"        )
		ENUM_N( AMEX                          )
		ENUM_N( ARCA                          )
		ENUM_N( BELFOX                        )
		ENUM_N( BOX                           )
		ENUM_N( BRUT                          )
		ENUM_N( BTRADE                        )
		ENUM_N( CBOE                          )
		ENUM_N( CBOT                          )
		ENUM_N( CFE                           )
		ENUM_N( CME                           )
		ENUM_N( DTB                           )
		ENUM_S( E_CBOT       , "E-CBOT"       )
		ENUM_S( EUREX_US     , "EUREX US"     )
		ENUM_S( FOREX        , "FOREX"        )
		ENUM_N( FTA                           )
		ENUM_N( GLOBEX                        )
		ENUM_N( HKFE                          )
		ENUM_N( IBIS                          )
		ENUM_N( IDEM                          )
		ENUM_N( IDEALPRO                      )
		ENUM_N( ISE                           )
		ENUM_N( ISLAND                        )
		ENUM_N( LIFFE                         )
		ENUM_N( LSE                           )
		ENUM_N( MATIF                         )
		ENUM_N( ME                            )
		ENUM_N( MEFFRV                        )
		ENUM_N( MONEP                         )
		ENUM_N( NYMEX                         )
		ENUM_N( NYSE                          )
		ENUM_N( ONE                           )
		ENUM_S( OSE_JPN_PHLX , "OSE.JPN PHLX" )
		ENUM_N( PSE                           )
		ENUM_N( SNFE                          )
		ENUM_N( SOFFEX                        )
		ENUM_N( SUPERMONTAGE                  )
		ENUM_N( SWX                           )
		ENUM_N( TSE                           )
		ENUM_S( TSE_JPN      , "TSE.JPN"      )
		ENUM_N( TSX                           )
		ENUM_N( VIRTX                         )
		ENUM_N( XETRA                         )
	ENUMFunctions( Exchange )

	//------------------------------------------------------------------------
	// EWrapper & EClient method support
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// reqMktData( id, contract, genericTicks, snapshot );
	//------------------------------------------------------------------------

	// To fill up the genericTicks field only
	ENUMValues( GenericTicks )                   // resulting tick values, see TickType
		ENUM_V( OptionVolume             , 100 ) // (currently for stocks)	, 29, 30
		ENUM_V( OptionOpenInterest       , 101 ) // (currently for stocks)	, 27, 28
		ENUM_V( HistoricalVolatility     , 104 ) // (currently for stocks)	, 23
		ENUM_V( CloseOptVolume           , 105 ) // ?via error msg?
		ENUM_V( OptionImpliedVolatility  , 106 ) // (currently for stocks)	, 24
		ENUM_V( CloseVolatility	         , 107 ) // ?via error msg?
		ENUM_V( IndexFuturePremium       , 162 ) //	, 31
		ENUM_V( MiscellaneousStats       , 165 ) //	, 15, 16, 17, 18, 19, 20, 21
		ENUM_V( MarkPrice                , 221 ) // (used in TWS P&L computations)	, 37
		ENUM_V( Auctionvalues            , 225 ) // (volume, price and imbalance)	, 34, 35, 36
	//	ENUM_V( MarkPrice                , 232 ) // ?via error msg?
		ENUM_V( RTVolume                 , 233 ) //	, 48
		ENUM_V( Shortable                , 236 ) //	, 46
		ENUM_V( Inventory                , 256 ) //	,  
		ENUM_V( FundamentalRatios        , 258 ) //	, 47
	ENUMFunctions( GenericTicks )

	// To help with the assembly of the genericTicks string, the struct GenerciTickString will be usefull as follows:
	//	GenericTickString GT = GenerciTickString(GenericTicks::OptionVolume) + GenericTicks::OptionOpenInterest;	// "100,101"
	//	GT += GenericTicks::HistoricalVolatility;	// "100,101,104"
	//	GT = GT + GenericTicks::Auctionvalues;		// "100,101,104,225"
	//	reqMktData( id, contract, GenericTickString(GenericTicks::OptionVolume) + GenericTicks::OptionOpenInterest, snapshot );
	#ifndef ENUMImplementation
	struct GenericTickString
	{
		GenericTickString             ( void ) {}
		GenericTickString             ( GenericTicks::ENUMS gt ) { char s[32]; sprintf( s, "%d", gt ); m_gt  = s; }
		GenericTickString& operator = ( GenericTicks::ENUMS gt ) { char s[32]; sprintf( s, "%d", gt ); m_gt  = s; return *this; }
		GenericTickString& operator + ( GenericTicks::ENUMS gt ) { char s[32]; sprintf( s, "%d", gt ); m_gt += IBString(m_gt.length() ?"," : "") + s; return *this; }
		GenericTickString& operator +=( GenericTicks::ENUMS gt ) { char s[32]; sprintf( s, "%d", gt ); m_gt += IBString(m_gt.length() ?"," : "") + s; return *this; }
		operator const IBString& ( void ) { return m_gt; }
		operator const char*     ( void ) { return m_gt; }
		private: IBString m_gt;
	};
	#endif

	// The values in the market data EWrapper events
	ENUMValues( TickTypes )                 // Used in these EWrapper calls , Value of genericTicks
		ENUM_N( BidSize                )	//  0 tickSize()				,
		ENUM_N( Bid                    )	//  1 tickPrice()				,
		ENUM_N( Ask                    )	//  2 tickPrice()				,
		ENUM_N( AskSize                )	//  3 tickSize()				,
		ENUM_N( Last                   )	//  4 tickPrice() 				,
		ENUM_N( LastSize               )	//  5 tickSize()				,
		ENUM_N( High                   )	//  6 tickPrice() 				,
		ENUM_N( Low                    )	//  7 tickPrice() 				,
		ENUM_N( Volume                 )	//  8 tickSize()				,
		ENUM_N( Close                  )	//  9 tickPrice()				,
		ENUM_N( BidOptionComputation   )	// 10 tickOptionComputation()	,
		ENUM_N( AskOptionComputation   )	// 11 tickOptionComputation()	,
		ENUM_N( LastOptionComputation  )	// 12 tickOptionComputation()	,
		ENUM_N( ModelOptionComputation )	// 13 tickOptionComputation()	,
		ENUM_N( Open                   )	// 14 tickPrice()				,
		ENUM_N( Low13Week              )	// 15 tickPrice()				, MiscellaneousStats
		ENUM_N( High13Week             )	// 16 tickPrice()				, MiscellaneousStats
		ENUM_N( Low26Week              )	// 17 tickPrice()				, MiscellaneousStats
		ENUM_N( High26Week             )	// 18 tickPrice()				, MiscellaneousStats
		ENUM_N( Low52Week              )	// 19 tickPrice()				, MiscellaneousStats
		ENUM_N( High52Week             )	// 20 tickPrice()				, MiscellaneousStats
		ENUM_N( AvgVolume              )	// 21 tickSize()				, MiscellaneousStats
		ENUM_N( OpenInterest           )	// 22 tickSize()				,
		ENUM_N( OptionHistoricalVol    )	// 23 tickGeneric()				, HistoricalVolatility
		ENUM_N( OptionImpliedVol       )	// 24 tickGeneric()				, OptionImpliedVolatility
		ENUM_N( OptionBidExch          )	// 25 NOT USED					,
		ENUM_N( OptionAskExch          )	// 26 NOT USED					,
		ENUM_N( OptionCallOpenInterest )	// 27 tickSize()				, OptionOpenInterest
		ENUM_N( OptionPutOpenInterest  )	// 28 tickSize()				, OptionOpenInterest
		ENUM_N( OptionCallVolume       )	// 29 tickSize()				, OptionVolume
		ENUM_N( OptionPutVolume        )	// 30 tickSize()				, OptionVolume
		ENUM_N( IndexFuturePremium     )	// 31 tickGeneric()				, IndexFuturePremium
		ENUM_N( BidExch                )	// 32 tickString()				,
		ENUM_N( AskExch                )	// 33 tickString()				,
		ENUM_N( AuctionVolume          )	// 34 NOT USED					, Auctionvalues
		ENUM_N( AuctionPrice           )	// 35 NOT USED					, Auctionvalues
		ENUM_N( AuctionImbalance       )	// 36 NOT USED					, Auctionvalues
		ENUM_N( MarkPrice              )	// 37 tickPrice()				, MarkPrice
		ENUM_N( BidEfpComputation      )	// 38 tickEFP()					,
		ENUM_N( AskEfpComputation      )	// 39 tickEFP()					,
		ENUM_N( LastEfpComputation     )	// 40 tickEFP()					,
		ENUM_N( OpenEfpComputation     )	// 41 tickEFP()					,
		ENUM_N( HighEfpComputation     )	// 42 tickEFP()					,
		ENUM_N( LowEfpComputation      )	// 43 tickEFP()					,
		ENUM_N( CloseEfpComputation    )	// 44 tickEFP()					,
		ENUM_N( LastTimestamp          )	// 45 tickString()				,
		ENUM_N( Shortable              )	// 46 tickGeneric()				, Shortable
		ENUM_N( FundamentalRatios      )	// 47 tickString()				, FundamentalRatios
		ENUM_N( RtVolume               )	// 48 tickGeneric()				, RTVolume
		ENUM_N( Halted                 )	// 49 tickGeneric()				,
		ENUM_N( BidYield               )	// 50 tickPrice()				,
		ENUM_N( AskYield               )	// 51 tickPrice()				,
		ENUM_N( LastYield              )	// 52 tickPrice()				,
		ENUM_N( CustOptionComputation  )	// 53 tickOptionComputation()	,
		ENUM_N(	TRADE_COUNT            )	// 54 tickGeneric()             , since v9.65
		ENUM_N(	TRADE_RATE             )	// 55 tickGeneric()             , since v9.65
		ENUM_N(	VOLUME_RATE            )	// 56 tickGeneric()             , since v9.65
		ENUM_N(	LAST_RTH_TRADE         )	// 57 tickGeneric()             , since v9.65
		ENUM_N( NotSet                 )	// 
	ENUMFunctions( TickTypes )

	// The FUNDAMENTAL_RATIOS tickType (Tick Value 47) tag values, requested via
	ENUMValues( FundamentalRatios )
		ENUM_N( NPRICE	              ) // Closing Price
		ENUM_N( Three_Year_TTM_Growth ) // 3 year trailing twelve months growth.
		ENUM_N( TTM_over_TTM	      ) // Trailing twelve months over trailing twelve months.
		ENUM_N( NHIG	              ) // High Price
		ENUM_N( NLOW	              ) // Low Price
		ENUM_N( PDATE	              ) // Pricing date
		ENUM_N( VOL10DAVG	          ) // Volume
		ENUM_N( MKTCAP	              ) // Market capitalization
		ENUM_N( TTMEPSXCLX	          ) // EPS excluding extraordinary items
		ENUM_N( AEPSNORM	          ) // EPS Normalized
		ENUM_N( TTMREVPS 	          ) // Revenue/share
		ENUM_N( QBVPS	              ) // Book value (Common Equity) per share
		ENUM_N( QTANBVPS	          ) // Book value (tangible) per share
		ENUM_N( QCSHPS	              ) // Cash per share
		ENUM_N( TTMCFSHR	          ) // Cash Flow per share
		ENUM_N( TTMDIVSHR	          ) // Dividends per share
		ENUM_N( IAD	                  ) // Dividend rate
		ENUM_N( PEEXCLXOR	          ) // P/E excluding extraordinary items
		ENUM_N( APENORM	              ) // P/E Normalized
		ENUM_N( TMPR2REV	          ) // Price to sales
		ENUM_N( PR2TANBK	          ) // Price to Tangible Book
		ENUM_N( TTMPRCFPS	          ) // Price to Cash Flow per share
		ENUM_N( PRICE2BK	          ) // Price to Book
		ENUM_N( QCURRATIO	          ) // Current ratio
		ENUM_N( QQUICKRATI	          ) // Quick ratio
		ENUM_N( QLTD2EQ	              ) // LT debt/equity
		ENUM_N( QTOTD2EQ	          ) // Total debt/total equity
		ENUM_N( TTMPAYRAT	          ) // Payout ratio
		ENUM_N( TTMREV	              ) // Revenue 
		ENUM_N( TTMEBITD	          ) // EBITD
		ENUM_N( TTMEBT	              ) // Earnings before taxes
		ENUM_N( TTMNIAC	              ) // Net Income available to common
		ENUM_N( AEBTNORM	          ) // Earnings before taxes Normalized
		ENUM_N( ANIACNORM	          ) // Net Income Available to Common, Normalized
		ENUM_N( TTMGROSMGN	          ) // Gross Margin
		ENUM_N( TTMNPMGN	          ) // Net Profit Margin %
		ENUM_N( TTMOPMGN	          ) // Operating margin
		ENUM_N( APTMGNPCT	          ) // Pretax margin
		ENUM_N( TTMROAPCT	          ) // Return on average assets
		ENUM_N( TTMROEPCT	          ) // Return on average equity
		ENUM_N( TTMROIPCT	          ) // Return on investment
		ENUM_N( REVCHNGYR	          ) // Revenue Change %
		ENUM_N( TTMREVCHG	          ) // Revenue Change %
		ENUM_N( REVTRENDGR	          ) // Revenue growth rate
		ENUM_N( EPSCHNGYR	          ) // EPS Change %
		ENUM_N( TTMEPSCHG	          ) // EPS Change %
		ENUM_N( EPSTRENDGR	          ) // EPS growth rate
		ENUM_N( DIVGRPCT	          ) // Growth rate % - dividend
	ENUMFunctions( FundamentalRatios )
		
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// Could replace faDataType define in CommonDefs.h
	ENUMValues( FADataType )
		ENUM_V( GROUPS, 1)
		ENUM_N( PROFILES )
		ENUM_N( ALIASES  )
	ENUMFunctions( FADataType )



	//------------------------------------------------------------------------
	// EWrapper method support
	//------------------------------------------------------------------------

	//	virtual void orderStatus( OrderId orderId, const IBString& status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId ) {} 
	// Although it is a string, it is defined mixedcase in the documentation?!
	ENUMValues( OrderStatus )
		ENUM_N( Undefined      )
		ENUM_N( PendingSubmit  )
		ENUM_N( PendingCancel  )
		ENUM_N( PreSubmitted   )
		ENUM_N( Submitted      )
		ENUM_N( Cancelled      )
		ENUM_N( Filled         )
	ENUMFunctions( OrderStatus )


	//	virtual void updateMktDepth[L2]( TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size ) {}
	ENUMValues( MktDepthOperation )
		ENUM_N( Insert )
		ENUM_N( Update )
		ENUM_N( Delete )
	ENUMFunctions( MktDepthOperation )

	ENUMValues( MktDeptSide )
		ENUM_N( Ask )
		ENUM_N( Bid )
	ENUMFunctions( MktDeptSide )


	//	virtual void updateNewsBulletin( int msgId, int msgType, const IBString& newsMessage, const IBString& originExch ) {}
	ENUMValues( NewsBulletinMsgType )
		ENUM_V( ReqularNewsBulletin, 1              )	// starts with value = 1
		ENUM_N( ExchangeNoLongerAvailableForTrading )
		ENUM_N( ExchangeIsAvailableForTrading       )
	ENUMFunctions( NewsBulletinMsgType )


	//------------------------------------------------------------------------
	// EClient method support
	//------------------------------------------------------------------------

	#ifndef ENUMImplementation
	extern
	IBString DateTime( int YYYYMMDD, int hh = 0, int mm = 0, int ss = 0, char Sep1 = ' ', char Sep2 = ':' )
	#else
	IBString DateTime( int YYYYMMDD, int hh    , int mm    , int ss    , char Sep1      , char Sep2       )
	{   
		char X[128]; sprintf( X, "%8d%c%02d%c%02d%c%02d", YYYYMMDD, Sep1, hh, Sep2, mm, Sep2, ss ); return X;
	}
	#endif
	;



	//	virtual void setServerLogLevel(int level) = 0;
	ENUMValues( ServerLogLevel )
		ENUM_V( System, 1   )	// starts with value = 1
		ENUM_N( Error       )
		ENUM_N( Warning     )
		ENUM_N( Information )
		ENUM_N( Detail      )
	ENUMFunctions( ServerLogLevel )


	//	virtual void reqHistoricalData( TickerId id, const Contract &contract, 
	//		const IBString &endDateTime, const IBString &durationStr, int barSizeSetting,
	//		const IBString &whatToShow, int useRTH, int formatDate) = 0;
	#ifndef ENUMImplementation
	IBString EndDateTime( int YYYY, int MM, int DD, int hh = 0, int mm = 0, int ss = 0 )
	#else
	IBString EndDateTime( int YYYY, int MM, int DD, int hh    , int mm    , int ss     )
	{
		return DateTime( YYYY*10000 + MM*100 + DD, hh, mm, ss );
	}
	#endif
	;

	ENUMValues( DurationHorizon )
		ENUM_S( Seconds, "S" )
		ENUM_S( Days   , "D" )
		ENUM_S( Weeks  , "W" )
		ENUM_S( Months , "M" )
		ENUM_S( Years  , "Y" )
	ENUMFunctions( DurationHorizon )

	IBString DurationStr( int Duration, const char* DH )
	#ifdef ENUMImplementation
	{
		char X[128]; sprintf( X, "%d %s", Duration, DH ); return X;
	}
	#endif
	;

	// Execption on the rule on the naming convention
	ENUMValues( BarSizeSetting )
		ENUM_S( _1_sec       ,  "1 secs"   )	// just for similarity, but IB requires secs
		ENUM_S( _1_secs      ,  "1 secs"   )
		ENUM_S( _5_secs      ,  "5 secs"   )
		ENUM_S( _10_secs     , "10 secs"   )
		ENUM_S( _15_secs     , "15 secs"   )
		ENUM_S( _30_secs     , "30 secs"   )
		ENUM_S( _1_min       ,  "1 min"    )
		ENUM_S( _2_mins      ,  "2 mins"   )
		ENUM_S( _3_mins      ,  "3 mins"   )
		ENUM_S( _5_mins      ,  "5 mins"   )
		ENUM_S( _10_mins     , "10 mins"   )
		ENUM_S( _15_mins     , "15 mins"   )
		ENUM_S( _20_mins     , "20 mins"   )
		ENUM_S( _30_mins     , "30 mins"   )
		ENUM_S( _1_hour      ,  "1 hour"   )
		ENUM_S( _4_hours     ,  "4 hours"  )
		ENUM_S( _1_day       ,  "1 day"    )
		ENUM_S( _1_week      ,  "1 week"   )
	//	ENUM_S( _1_month     ,  "1 month"  )
	//	ENUM_S( _3_months    ,  "3 months" )
	//	ENUM_S( _1_year      ,  "1 year"   )
	ENUMFunctions( BarSizeSetting )


	ENUMValues( WhatToShow )
		ENUM_N( TRADES    )
		ENUM_N( MIDPOINT  )
		ENUM_N( BID       )
		ENUM_N( ASK       )
		ENUM_N( BID_ASK   )
	ENUMFunctions( WhatToShow )


	ENUMValues( UseRTH )
		ENUM_N( AllTradingData         )
		ENUM_N( OnlyRegularTradingData )
	ENUMFunctions( UseRTH )


	ENUMValues( FormatDate )
		ENUM_V( AsDate     , 1 )	// starts with value = 1
		ENUM_N( AsSecondsSince )
	ENUMFunctions( FormatDate )


	//	virtual void exerciseOptions( TickerId id, const Contract &contract,
	//		int exerciseAction, int exerciseQuantity, const IBString &account, int override) = 0;
	ENUMValues( ExerciseAction )
		ENUM_V( Exercise, 1 )	// starts with value = 1
		ENUM_N( Lapse       )
	ENUMFunctions( ExerciseAction )


	ENUMValues( Override )
		ENUM_N( NoSystemDefaultOverride )
		ENUM_N( DoOverrideSystemDefault )
	ENUMFunctions( Override )



	//------------------------------------------------------------------------
	// Execution properties
	//------------------------------------------------------------------------
	ENUMValues( ExecutionSide )
		ENUM_N( BOT )
		ENUM_N( SLD )
	ENUMFunctions( ExecutionSide )


	//------------------------------------------------------------------------
	// ExecutionFilter properties
	//------------------------------------------------------------------------
	#ifndef ENUMImplementation
	extern
	IBString ExecutionTime( int YYYY, int MM, int DD, int hh = 0, int mm = 0, int ss = 0 )
	#else
	IBString ExecutionTime( int YYYY, int MM, int DD, int hh    , int mm    , int ss     )
	{
		return DateTime( YYYY*10000 + MM*100 + DD, hh, mm, ss, '-' );
	}
	#endif
	;


	//------------------------------------------------------------------------
	// Contract properties
	// http://individuals.interactivebrokers.com/php/apiUsersGuide/apiguide/c/contract.htm
	//	=> STK, OPT, FUT, IND, FOP, CASH, BAG
	// http://www.interactivebrokers.com.hk/php/webhelp/Making_Trades/baskets/manualBasket.htm	

	//	=> stk, opt, fut, fop, war, cash, fund, efp, bag
	//------------------------------------------------------------------------
	ENUMValues( SecType )
		ENUM_N( STK   )
		ENUM_N( OPT   )
		ENUM_N( FUT   )
		ENUM_N( IND   )
		ENUM_N( FOP   )
		ENUM_N( WAR   )
		ENUM_N( CASH  )
		ENUM_N( FUND  )
		ENUM_N( EFP   )
		ENUM_N( BAG   )
	ENUMFunctions( SecType )

	extern IBString ContractExpirationDate( int YYYY, int MM )
	#ifdef ENUMImplementation
	{
		char X[128]; sprintf( X, "%6d", YYYY*100 + MM ); return X;
	}
	#endif
	;

	ENUMValues( ContractRight )
		ENUM_N( PUT  )
		ENUM_N( CALL )
	ENUMFunctions( ContractRight )


	ENUMValues( SecIdType )	// supports the following identifiers:
		ENUM_N( ISIN  )	// - The International Securities Identification Number (ISIN) that uniquely identifies the security but not the location at which it is traded. Example: Apple: US0378331005.
		ENUM_N( CUSIP )	// - The nine-character Committee on Uniform Securities Identification Procedure (CUSIP) identifier for North American securities. Example: Apple: 037833100.
		ENUM_N( SEDOL )	// - The Stock Exchange Daily Official List (SEDOL) identifier consisting of 6-AN + check digit. Example: BAE: 0263494.
		ENUM_N( RIC   )	//  - The Reuters instrument code, consisting of the RIC Root (exchange-independent) and a suffix identifying the exchange at which the contract trades. RIC Root is unique for each company. Example: AAPL.O for Apple on NASDAQ.
	ENUMFunctions( SecIdType )// Notes: If the CUSIP is defined, then ISIP = [US/CA][CUSIP][check digit]. The ISIN and CUSIP can replace the underlying, but when a contract trades in several locations (regions), you must also provide the primary exchange.
		
	//------------------------------------------------------------------------
	//	Order properties
	//------------------------------------------------------------------------
	ENUMValues( OrderAction )
		ENUM_N( BUY    )
		ENUM_N( SELL   )
		ENUM_N( SSHORT )
	ENUMFunctions( OrderAction )


	ENUMValues( OrderType )
		ENUM_N( MKT          )
		ENUM_N( MKTCLS       )
		ENUM_N( LMT          )
		ENUM_N( LMTCLS       )
		ENUM_N( PEGMKT       )
		ENUM_N( STP          )
		ENUM_N( STPLMT       )
		ENUM_N( TRAIL        )
		ENUM_N( TRAILLIMIT   )
		ENUM_N( REL          )
		ENUM_N( VWAP         )
		ENUM_N( NONE         )
	ENUMFunctions( OrderType )


	ENUMValues( TimeInForce )
		ENUM_N( DAY )
		ENUM_N( GTC )
		ENUM_N( OPG )
		ENUM_N( IOC )
		ENUM_N( GTD )
	ENUMFunctions( TimeInForce )


	ENUMValues( OpenClose )
		ENUM_N( O )
		ENUM_N( C )
	ENUMFunctions( OpenClose )


	ENUMValues( OrderOrigin )
		ENUM_N( Customer )
		ENUM_N( Firm     )
		ENUM_N( UnKnown  )
	ENUMFunctions( OrderOrigin )


	ENUMValues( TriggerMethod )
		ENUM_V( Default       , 0) // 0
		ENUM_V( DoubleBidAsk  , 1 ) // 1
		ENUM_V( LastPrice     , 2 )	// 2 'Last' in the documentation
		ENUM_V( DoubleLast    , 3 ) // 3
		ENUM_V( BidAsk        , 4 ) // 4
		ENUM_V( LastOrBidAsk  , 7 ) // 7
		ENUM_V( Midpoint      , 8 ) // 8
	ENUMFunctions( TriggerMethod )


	ENUMValues( ShortSaleSlot )
		ENUM_N( First  )
		ENUM_N( Second )
	ENUMFunctions( ShortSaleSlot )


	#ifndef ENUMImplementation
	extern
	IBString OrderGoodAfterTime( int YYYY, int MM, int DD, int hh = 0, int mm = 0, int ss = 0 )
	#else
	IBString OrderGoodAfterTime( int YYYY, int MM, int DD, int hh    , int mm    , int ss     )
	{
		return DateTime( YYYY*10000 + MM*100 + DD, hh, mm, ss );
	}
	#endif
	;

	#ifndef ENUMImplementation
	extern
	IBString OrderGoodTillDate( int YYYY, int MM, int DD, int hh = 0, int mm = 0, int ss = 0 )
	#else
	IBString OrderGoodTillDate( int YYYY, int MM, int DD, int hh    , int mm    , int ss     )
	{
		return DateTime( YYYY*10000 + MM*100 + DD, hh, mm, ss );
	}
	#endif
	;

	ENUMValues( OcaType )
		ENUM_V( CancelOnFillWithBlock, 1 )	// starts with value = 1
		ENUM_N( ReduceOnFillWithBlock    )
		ENUM_N( ReduceOnFillWithoutBlock )
	ENUMFunctions( OcaType )


	// Execption on the rule on the naming convention.
	ENUMValues( Rule80A )
		ENUM_S( Individual           ,  "I"   )
		ENUM_S( Agency               ,  "A"   )
		ENUM_S( AgentOtherMember     ,  "W"   )
		ENUM_S( IndividualPTIA       ,  "J"   )
		ENUM_S( AgencyPTIA           ,  "U"   )
		ENUM_S( AgentOtherMemberPTIA ,  "M"   )
		ENUM_S( IndividualPT         ,  "K"   )
		ENUM_S( AgencyPT             ,  "Y  " )
		ENUM_S( AgentOtherMemberPT   ,  "N"   )
	ENUMFunctions( Rule80A )


	ENUMValues( AuctionStrategy )
		ENUM_N( Unset       )
		ENUM_N( Match       )
		ENUM_N( Improvement )
		ENUM_N( Transparent )
	ENUMFunctions( AuctionStrategy )


	// For orders on BOX only.
	ENUMValues( VolatilityType )
		ENUM_V( Daily, 1 )	// starts with value = 1
		ENUM_N( Annual   )
	ENUMFunctions( VolatilityType	 )

 
	ENUMValues( ReferencePriceType )
		ENUM_V( AverageOfNBBO, 1 )	// starts with value = 1 // Average
		ENUM_N( NBBOrNBO         )  // BidOrAsk
	ENUMFunctions( ReferencePriceType )


	ENUMValues( HedgeType )  // 'D' - delta, 'B' - beta, 'F' - FX, 'P' - pair
		ENUM_S( Delta      , "D" )
		ENUM_S( Beta       , "B" )
		ENUM_S( FX         , "F" )
		ENUM_S( Pair       , "P" )
	ENUMFunctions( HedgeType )


	ENUMValues( HedgeParam ) // 'beta=X' value for beta hedge, 'ratio=Y' for pair hedge
		ENUM_S( BetaHedge  , "beta=X"  )
		ENUM_S( PairHedge  , "ratio=Y" )
	ENUMFunctions( HedgeParam )


	ENUMValues( ClearingIntent ) // "" (Default), "IB", "Away", "PTA" (PostTrade)
		ENUM_S( Default    , ""     )
		ENUM_S( IB         , "IB"   )
		ENUM_S( Away       , "Away" )
		ENUM_S( PTA        , "PTA"  ) // (post trade allocation).
	ENUMFunctions( ClearingIntent )


	ENUMValues( SmartComboRouting )
		ENUM_S( Priority                              , "LeginPrio"           ) // User can specify which leg to be executed first. Values = -1, 0 or 1
		ENUM_S( DiscretionaryAmount                   , "MaxSegSize"          ) // When one leg is executed, we can adjust the other leg by up to a discretionary amount. Value = An amount
		ENUM_S( MarketIfTouchedTimeoutAfterFirstFill  , "ChangeToMktTime1"    ) // For Market-If-Touched combo orders, we record the firstTradeTime of the first fill of the first leg to execute, and the lastTradeTime of the last partial fill. For these kinds of orders, you can now specify timeout values of the last fill and the timeout since the first fill, in seconds. Value = Number of seconds
		ENUM_S( MarketIfTouchedTimeoutAfterLastFill   , "ChangeToMktTime2"    ) // For Market-If-Touched combo orders, we record the firstTradeTime of the first fill of the first leg to execute, and the lastTradeTime of the last partial fill. For these kinds of orders, you can now specify timeout values of the last fill and the timeout since the first fill, in seconds. Value = Number of seconds
		ENUM_S( MarketIfTouchedStopLoss               , "ChangeToMktOffset"   ) // Specify an absolute stop-loss amount per combo. If specified and if the implied execution price of the combo (based on a leg that has already been executed and current market data) exceeds the combo price plus the stop-oss amount, we convert the order from LMT to MKT immediately in order to finish executing the combo order. If the stop-loss amount is specified but timeouts have not been specified, we will continue to try to execute the second leg at the calculated LMT price until it either executes or the stop-loss amount is reached. Value = An amount.
		ENUM_S( MaximumLegInSize                      , "MaxSegSize"          ) // Specify the maximum allowed leg-in size per segment. Value = Unit of combo size
		ENUM_S( DiscretionaryPercentage               , "DiscretionaryPct"    ) // Specify a percentage of the combo price. This applies to scale combos in which the discretionary amount is calculated from the current scale level. When the discretionary amount is entered as a percentage, the API will convert that to a dollar amount according to the combo. This amount will be updated when the order price changes or for scale orders for each level. You can enter a value for this parameter or for the Discretionary Amt extended attribute one at a time, but not both at the same time. Value = A value between 0 and 100.
	ENUMFunctions( SmartComboRouting )


	//------------------------------------------------------------------------
	// ComboLeg properties
	//------------------------------------------------------------------------
	ENUMValues( ComboAction )
		ENUM_N( BUY  )
		ENUM_N( SELL )
	ENUMFunctions( ComboAction )
	

	ENUMValues( ComboOpenClose )
		ENUM_N( SamePos    )
		ENUM_N( OpenPos    )
		ENUM_N( ClosePos   )
		ENUM_N( UnknownPos )
	ENUMFunctions( ComboOpenClose )


	//------------------------------------------------------------------------
	// ScannerSubscription Properties
	//------------------------------------------------------------------------
	//ScannerSettingPairs
	// ????
	ENUMValues( StockTypeFilter )
		ENUM_N( ALL    )
		ENUM_N( STOCK  )
		ENUM_N( ETF    )
	ENUMFunctions( StockTypeFilter )


	//------------------------------------------------------------------------
	// reqMarketDataType
	//------------------------------------------------------------------------
	ENUMValues( MarketDataType )
		ENUM_V( Realtime  , 1)
		ENUM_N( Frozen    )
	ENUMFunctions( MarketDataType )


	//------------------------------------------------------------------------
	// virtual void updateAccountValue(const IBString& key, const IBString& val,
	//------------------------------------------------------------------------
	ENUMValues( UpdateAccountValueKey )
		ENUM_N( AccountCode                        )
		ENUM_N( AccountReady                       )
		ENUM_N( AccountType                        )
		ENUM_N( AccruedCash                        )
		ENUM_S( AccruedCash_S                      , "AccruedCash-S"                    )
		ENUM_N( AccruedDividend                    )
		ENUM_S( AccruedDividend_S                  , "AccruedDividend-S"                )
		ENUM_N( AvailableFunds                     )
		ENUM_S( AvailableFunds_S                   , "AvailableFunds-S"                 )
		ENUM_N( Billable                           )
		ENUM_S( Billable_S                         , "Billable-S"                       )
		ENUM_N( BuyingPower                        )
		ENUM_N( CashBalance                        )
		ENUM_N( CorporateBondValue                 )
		ENUM_N( Currency                           )
		ENUM_N( Cushion                            )
		ENUM_N( DayTradesRemaining                 )
		ENUM_S( DayTradesRemainingT_1              , "DayTradesRemainingT+1"            )
		ENUM_S( DayTradesRemainingT_2              , "DayTradesRemainingT+2"            )
		ENUM_S( DayTradesRemainingT_3              , "DayTradesRemainingT+3"            )
		ENUM_S( DayTradesRemainingT_4              , "DayTradesRemainingT+4"            )
		ENUM_N( EquityWithLoanValue                )
		ENUM_S( EquityWithLoanValue_S              , "EquityWithLoanValue-S"            )
		ENUM_N( ExcessLiquidity                    )
		ENUM_S( ExcessLiquidity_S                  , "ExcessLiquidity-S"                )
		ENUM_N( ExchangeRate                       )
		ENUM_N( FullAvailableFunds                 )
		ENUM_S( FullAvailableFunds_S               , "FullAvailableFunds-S"             )
		ENUM_N( FullExcessLiquidity                )
		ENUM_S( FullExcessLiquidity_S              , "FullExcessLiquidity-S"            )
		ENUM_N( FullInitMarginReq                  )
		ENUM_S( FullInitMarginReq_S                , "FullInitMarginReq-S"              )
		ENUM_N( FullMaintMarginReq                 )
		ENUM_S( FullMaintMarginReq_S               , "FullMaintMarginReq-S"             )
		ENUM_N( FundValue                          )
		ENUM_N( FutureOptionValue                  )
		ENUM_N( FuturesPNL                         )
		ENUM_N( FxCashBalance                      )
		ENUM_N( GrossPositionValue                 )
		ENUM_S( GrossPositionValue_S               , "GrossPositionValue-S"             )
		ENUM_N( IndianStockHaircut                 )
		ENUM_S( IndianStockHaircut_S               , "IndianStockHaircut-S"             )
		ENUM_N( InitMarginReq                      )
		ENUM_S( InitMarginReq_S                    , "InitMarginReq-S"                  )
		ENUM_S( Leverage_S                         , "Leverage-S"                       )
		ENUM_N( LookAheadAvailableFunds            )
		ENUM_S( LookAheadAvailableFunds_S          , "LookAheadAvailableFunds-S"        )
		ENUM_N( LookAheadExcessLiquidity           )
		ENUM_S( LookAheadExcessLiquidity_S         , "LookAheadExcessLiquidity-S"       )
		ENUM_N( LookAheadInitMarginReq             )
		ENUM_S( LookAheadInitMarginReq_S           , "LookAheadInitMarginReq-S"         )
		ENUM_N( LookAheadMaintMarginReq            )
		ENUM_S( LookAheadMaintMarginReq_S          , "LookAheadMaintMarginReq-S"        )
		ENUM_N( LookAheadNextChange                )
		ENUM_N( MaintMarginReq                     )
		ENUM_S( MaintMarginReq_S                   , "MaintMarginReq-S"                 )
		ENUM_N( MoneyMarketFundValue               )
		ENUM_N( MutualFundValue                    )
		ENUM_N( NetDividend                        )
		ENUM_N( NetLiquidation                     )
		ENUM_S( NetLiquidation_S                   , "NetLiquidation-S"                 )
		ENUM_N( NetLiquidationByCurrency           )
		ENUM_N( OptionMarketValue                  )
		ENUM_N( PASharesValue                      )
		ENUM_S( PASharesValue_S                    , "PASharesValue-S"                  )
		ENUM_N( PNL                                )
		ENUM_N( PreviousDayEquityWithLoanValue     )
		ENUM_S( PreviousDayEquityWithLoanValue_S   , "PreviousDayEquityWithLoanValue-S" )
		ENUM_N( RealizedPnL                        )
		ENUM_N( RegTEquity                         )
		ENUM_S( RegTEquity_S                       , "RegTEquity-S"                     )
		ENUM_N( RegTMargin                         )
		ENUM_S( RegTMargin_S                       , "RegTMargin-S"                     )
		ENUM_N( SMA                                )
		ENUM_S( SMA_S                              , "SMA-S"                            )
		ENUM_N( StockMarketValue                   )
		ENUM_N( TBillValue                         )
		ENUM_N( TBondValue                         )
		ENUM_N( TotalCashBalance                   )
		ENUM_N( TotalCashValue                     )
		ENUM_S( TotalCashValue_S                   , "TotalCashValue-S"                 )
		ENUM_S( TradingType_S                      , "TradingType-S"                    )
		ENUM_N( UnalteredInitMarginReq             )
		ENUM_N( UnalteredMaintMarginReq            )
		ENUM_N( UnrealizedPnL                      )
		ENUM_N( WarrantValue                       )
		ENUM_N( WhatIfPMEnabled                    )
	ENUMFunctions( UpdateAccountValueKey )

};
#endif	// _TwsApiDefs_h
