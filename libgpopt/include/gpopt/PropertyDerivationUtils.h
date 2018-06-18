#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/base/CDrvdPropRelationalMock.h"

namespace gpopt
{
	// shorthand for conversion
	CDrvdPropRelational *Pdprel(CDrvdProp *pdp);

	CDrvdPropRelational *Pdprel(CDrvdPropRelationalMock *pdpMock);
}
