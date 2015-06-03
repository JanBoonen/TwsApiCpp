//============================================================================
// TwsApi Empty
//============================================================================
#include "TwsApiL0.h"
//#include "TwsApiDefs.h"
//using namespace TwsApi;

class MyEWrapper: public EWrapperL0
{
};



int main( void )
{
	MyEWrapper	MW;
	EClientL0*	EC = EClientL0::New( &MW );

	delete EC;
}
