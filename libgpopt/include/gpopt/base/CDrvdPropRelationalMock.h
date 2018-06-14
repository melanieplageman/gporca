
#ifndef GPOPT_CDrvdPropRelationalMock_H
#define GPOPT_CDrvdPropRelationalMock_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/base/CDrvdPropRelational.h"


namespace gpopt
{
	using namespace gpos;

	// fwd declaration
	class CExpressionHandleMock;
	class CColRefSet;


	class CDrvdPropRelationalMock : public CDrvdPropRelational
	{

		private:


		public:

			// ctor
			CDrvdPropRelationalMock();

	}; // class CDrvdPropRelationalMock

}


#endif // !GPOPT_CDrvdPropRelationalMock_H

// EOF
