
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

			void Derive(IMemoryPool *pmp, CExpressionHandle &exprhdl, CDrvdPropCtxt *pdpctxt);


			static CDrvdPropRelationalMock *Pdprel(CDrvdProp *pdp);

	}; // class CDrvdPropRelationalMock

}


#endif // !GPOPT_CDrvdPropRelationalMock_H

// EOF
