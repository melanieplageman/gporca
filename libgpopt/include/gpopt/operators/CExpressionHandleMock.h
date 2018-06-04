#ifndef GPOPT_CExpressionHandleMock_H
#define GPOPT_CExpressionHandleMock_H

#include "gpos/base.h"
#include "gpopt/operators/CExpressionHandle.h"

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/base/CDrvdProp.h"
#include "gpopt/base/CReqdProp.h"
#include "gpopt/base/CDrvdPropCtxtPlan.h"
#include "gpopt/base/CDrvdPropScalar.h"
#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/search/CGroupExpression.h"

namespace gpopt
{
	class CExpressionHandleMock : public CExpressionHandle
	{
		private:
		public:
			// ctor
			CExpressionHandleMock(IMemoryPool *pmp);

			// dtor
			~CExpressionHandleMock();

			void Attach(CExpression *pexpr);

			void DeriveProps(CDrvdPropCtxt *pdpctxt);



	};
}

#endif // !GPOPT_CExpressionHandleMock_H

// EOF
