
#ifndef GPOPT_CDrvdPropRelationalMock_H
#define GPOPT_CDrvdPropRelationalMock_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/base/CDrvdPropRelational.h"

#include "naucrates/md/CMDProviderMemory.h"
#include "gpopt/base/CAutoOptCtxt.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpopt/mdcache/CMDCache.h"


#include "naucrates/md/IMDTypeInt4.h"
#include "gpopt/base/CColumnFactory.h"

#include "gpopt/base/CColRefSet.h"

namespace gpopt
{
	using namespace gpos;

	// fwd declaration
	class CExpressionHandleMock;


	class CDrvdPropRelationalMock : public CDrvdPropRelational
	{

		private:


		public:

			// ctor
			CDrvdPropRelationalMock();

			void Derive(IMemoryPool *pmp, CExpressionHandle &exprhdl, CDrvdPropCtxt *pdpctxt);


			// output columns
			CColRefSet *PcrsOuter(IMemoryPool *pmp) const;

	}; // class CDrvdPropRelationalMock

}


#endif // !GPOPT_CDrvdPropRelationalMock_H

// EOF
