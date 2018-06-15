#include "gpopt/base/CDrvdPropRelationalMock.h"
#include "gpopt/operators/CLogical.h"


using namespace gpopt;


//		ctor

CDrvdPropRelationalMock::CDrvdPropRelationalMock
	()
	:
CDrvdPropRelational()

{}

void
CDrvdPropRelationalMock::Derive
		(
				IMemoryPool *,
				CExpressionHandle &,
				CDrvdPropCtxt * // pdpctxt
		)
{
	GPOS_CHECK_ABORT;


}


// EOF
