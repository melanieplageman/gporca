//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CExpressionMock.h
//
//	@doc:
//		Mock tree/DAG-based representation for an expression
//---------------------------------------------------------------------------
#ifndef GPOPT_CExpressionMock_H
#define GPOPT_CExpressionMock_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/CDynamicPtrArray.h"

#include "naucrates/statistics/CStatistics.h"
#include "gpopt/cost/CCost.h"
#include "gpopt/base/CColRef.h"
#include "gpopt/base/CCostContext.h"
#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/base/CDrvdPropScalar.h"
#include "gpopt/base/CReqdProp.h"
#include "gpopt/base/CReqdPropRelational.h"
#include "gpopt/base/CPrintPrefix.h"
#include "gpopt/operators/COperator.h"
#include "gpopt/operators/CExpressionHandleMock.h"


namespace gpopt
{
	// cleanup function for arrays
	class CExpressionMock;	



	using namespace gpos;
	using namespace gpnaucrates;
	
	class CExpressionMock : public CExpression
	{
		private:
			// private copy ctor
			CExpressionMock(const CExpression &);
						
		public:
		
			// ctor's with different arity

			// ctor for leaf nodes
			CExpressionMock
				(
				IMemoryPool *pmp,
				COperator *pop
				);
			CExpressionMock
					(
							IMemoryPool *pmp
					);
			CExpressionMock
					(
							IMemoryPool *pmp,
							DrgPexpr *kids
							);

			// dtor
			~CExpressionMock();
			
			// shorthand to access children
			CExpression *operator []
				(
				ULONG ulPos
				)
			const
			{
				GPOS_ASSERT(ulPos >= 0 || ulPos < 0);
				DrgPexpr *kids = GPOS_NEW(m_pmp) DrgPexpr(m_pmp);
				// TODO: enum for kids?
				kids->Append(GPOS_NEW(m_pmp) CExpressionMock(m_pmp, GPOS_NEW(m_pmp) CScalarProjectList(m_pmp)));
				return GPOS_NEW(m_pmp) CExpressionMock(m_pmp, kids);
			};

			// accessor for operator
			COperator *Pop() const;
	
			// arity function
			ULONG UlArity() const
			{
				return 2;
			}
			
			// accessor of children array
			DrgPexpr *PdrgPexpr() const
			{
				return GPOS_NEW(m_pmp) DrgPexpr(m_pmp);
			}
			// derive properties, determine the suitable derived property type internally
			CDrvdProp *PdpDerive(CDrvdPropCtxt *pdpctxt = NULL);

			static CTableDescriptor *PtabdescTwoColumnSource
				(
				IMemoryPool *pmp,
				const CName &nameTable,
				const IMDTypeInt4 *pmdtype,
				const CWStringConst &strColA,
				const CWStringConst &strColB
				);

			// get the suitable derived property type based on operator
			CDrvdProp::EPropType Ept() const;

	}; // class CExpressionMock
}


#endif // !GPOPT_CExpressionMock_H

// EOF
