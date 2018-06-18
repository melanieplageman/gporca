#include <typeinfo>

#include "gpopt/PropertyDerivationUtils.h"
#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/base/CDrvdPropRelationalMock.h"
#include "gpopt/base/CDrvdProp.h"

namespace gpopt
{
	CDrvdPropRelational *
	Pdprel
			(
					CDrvdProp *pdp
			)
	{
		GPOS_ASSERT(NULL != pdp);

		const std::type_info& o = typeid(*pdp);
		const std::type_info& c = typeid(CDrvdPropRelationalMock);

		if (o == c)
		{
			return dynamic_cast<CDrvdPropRelationalMock*>(pdp);
		}
		else
		{
			return dynamic_cast<CDrvdPropRelational*>(pdp);
		}
	}

	CDrvdPropRelational *
	Pdprel
			(
					CDrvdPropRelationalMock *pdpMock
			)
	{
		GPOS_ASSERT(NULL != pdpMock);

		return pdpMock;
	}
}
