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


namespace gpopt
{
	// cleanup function for arrays
	class CExpressionMock;	
	typedef CDynamicPtrArray<CExpressionMock, CleanupRelease> DrgPexprMock;

	// array of arrays of expression pointers
	typedef CDynamicPtrArray<DrgPexprMock, CleanupRelease> DrgPdrgPexprMock;

	class CGroupExpression;
	class CDrvdPropPlan;
	class CDrvdPropCtxt;
	class CDrvdPropCtxtPlan;

	using namespace gpos;
	using namespace gpnaucrates;
	
	//---------------------------------------------------------------------------
	//	@class:
	//		CExpression
	//
	//	@doc:
	//		Simply dynamic array for pointer types
	//
	//---------------------------------------------------------------------------
	class CExpressionMock : public CExpression
	{
		private:
		
			// memory pool
			IMemoryPool *m_pmp;
			
			// operator class
			COperator *m_pop;
			
			// array of children
			DrgPexprMock *m_pdrgpexprmock;

			// derived relational properties
			CDrvdPropRelational *m_pdprel;

			// derived stats
			IStatistics *m_pstats;

			// required plan properties
			CReqdPropPlan *m_prpp;

			// derived physical properties
			CDrvdPropPlan *m_pdpplan;

			// derived scalar properties
			CDrvdPropScalar *m_pdpscalar;

			// group reference to Memo
			CGroupExpression *m_pgexpr;

			// cost of physical expression node when copied out of the memo
			CCost m_cost;

			// id of origin group, used for debugging expressions extracted from memo
			ULONG m_ulOriginGrpId;

			// id of origin group expression, used for debugging expressions extracted from memo
			ULONG m_ulOriginGrpExprId;

			// set expression's derivable property
			void SetPdp(CDrvdProp *pdp, const CDrvdProp::EPropType ept);

#ifdef GPOS_DEBUG

			// assert valid property derivation
			void AssertValidPropDerivation(const CDrvdProp::EPropType ept);

			// print expression properties
			void PrintProperties(IOstream &os, CPrintPrefix &pfx) const;

#endif // GPOS_DEBUG

			// check if the expression satisfies partition enforcer condition
			BOOL FValidPartEnforcers(CDrvdPropCtxtPlan *pdpctxtplan);

			// check if the distributions of all children are compatible
			BOOL FValidChildrenDistribution(CDrvdPropCtxtPlan *pdpctxtplan);

			// copy group properties and stats to expression
			void CopyGroupPropsAndStats(IStatistics *pstatsInput);

			// decorate expression tree with required plan properties
			CReqdPropPlan* PrppDecorate(IMemoryPool *pmp, CReqdPropPlan *prppInput);

			// private copy ctor
			CExpressionMock(const CExpression &);
						
		public:
		
			// ctor's with different arity

			// ctor for leaf nodes
			CExpressionMock
				(
				IMemoryPool *pmp,
				COperator *pop,
				CGroupExpression *pgexpr = NULL
				);

			// ctor for unary expressions
			CExpressionMock
				(
				IMemoryPool *pmp,
				COperator *pop,
				CExpressionMock *pexpr
				);

			// ctor for binary expressions
			CExpressionMock
				(
				IMemoryPool *pmp,
				COperator *pop,
				CExpressionMock *pexprChildFirst,
				CExpressionMock *pexprChildSecond
				);

			// ctor for ternary expressions
			CExpressionMock
				(
				IMemoryPool *pmp,
				COperator *pop,
				CExpressionMock *pexprChildFirst,
				CExpressionMock *pexprChildSecond,
				CExpressionMock *pexprChildThird
				);
//
//			// ctor n-ary expressions
//			CExpressionMock
//				(
//				IMemoryPool *pmp,
//				COperator *pop,
//				DrgPexprMock *pdrgpexprmock
//				);
//
//			// ctor for n-ary expression with origin group expression
//			CExpressionMock
//				(
//				IMemoryPool *pmp,
//				COperator *pop,
//				CGroupExpression *pgexpr,
//				DrgPexprMock *pdrgpexprmock,
//				IStatistics *pstatsInput,
//				CCost cost = GPOPT_INVALID_COST
//				);
//
//			// ctor for expression with derived properties
//			CExpressionMock
//				(
//				IMemoryPool *pmp,
//				CDrvdProp *pdprop
//				);
			
			// dtor
			~CExpressionMock();
			
			// shorthand to access children
			CExpressionMock *operator []
				(
				ULONG ulPos
				)
			const
			{
				GPOS_ASSERT(NULL != m_pdrgpexprmock);
				return (*m_pdrgpexprmock)[ulPos];
			};
	
			// arity function
			ULONG UlArity() const
			{
				return m_pdrgpexprmock == NULL ? 0 : m_pdrgpexprmock->UlLength();
			}
			
			// accessor for operator
			COperator *Pop() const
			{
				GPOS_ASSERT(NULL != m_pop);
				return m_pop;
			}
		
			// accessor of children array
			DrgPexprMock *PdrgPexpr() const
			{
				return m_pdrgpexprmock;
			}

			// accessor for origin group expression
			CGroupExpression *Pgexpr() const
			{
				return m_pgexpr;
			}

			// accessor for computed required plan props
			CReqdPropPlan *Prpp() const
			{
				return m_prpp;
			}

			// get expression's derived property given its type
			CDrvdProp *Pdp(const CDrvdProp::EPropType ept) const;

			// get derived statistics object
			const IStatistics *Pstats() const
			{
				return m_pstats;
			}

			// cost accessor
			CCost Cost() const
			{
				return m_cost;
			}

			// get the suitable derived property type based on operator
			CDrvdProp::EPropType Ept() const;

			// derive properties, determine the suitable derived property type internally
			CDrvdProp *PdpDerive(CDrvdPropCtxt *pdpctxt = NULL);

			// derive statistics
			IStatistics *PstatsDerive(CReqdPropRelational *prprel, DrgPstat *pdrgpstatCtxt);

			// reset a derived property
			void ResetDerivedProperty(CDrvdProp::EPropType ept);

			// reset all derived properties
			void ResetDerivedProperties();

			// reset expression stats
			void ResetStats();

			// compute required plan properties of all expression nodes
			CReqdPropPlan* PrppCompute(IMemoryPool *pmp, CReqdPropPlan *prppInput);

			// check for outer references
			BOOL FHasOuterRefs();

			// print driver
			IOstream &OsPrint
						(
						IOstream &os,
						const CPrintPrefix * = NULL,
						BOOL fLast = true
						)
						const;
			
			// match with group expression
			BOOL FMatchPattern(CGroupExpression *pgexpr) const;
			
			// return a copy of the expression with remapped columns
			CExpression *PexprCopyWithRemappedColumns(IMemoryPool *pmp, HMUlCr *phmulcr, BOOL fMustExist) const;

			// compare entire expression rooted here
			BOOL FMatch(CExpression *pexpr) const;

#ifdef GPOS_DEBUG
			// match against given pattern
			BOOL FMatchPattern(CExpression *pexpr) const;
			
			// match children against given pattern
			BOOL FMatchPatternChildren(CExpression *pexpr) const;
			
			// compare entire expression rooted here
			BOOL FMatchDebug(CExpression *pexpr) const;

			// debug print; for interactive debugging sessions only
			void DbgPrint() const;
#endif // GPOS_DEBUG

			// check if the expression satisfies given required properties
			BOOL FValidPlan(const CReqdPropPlan *prpp, CDrvdPropCtxtPlan *pdpctxtplan);

			// static hash function
			static
			ULONG UlHash(const CExpression *pexpr);

			// static hash function
			static
			ULONG UlHashDedup(const CExpression *pexpr);

			// rehydrate expression from a given cost context and child expressions
//			static
//			CExpression *PexprRehydrate(IMemoryPool *pmp, CCostContext *pcc, DrgPexprMock *pdrgpexprmock, CDrvdPropCtxtPlan *pdpctxtplan);


	}; // class CExpressionMock


	// shorthand for printing
	inline
	IOstream &operator << (IOstream &os, CExpressionMock &exprmock)
	{
		return exprmock.OsPrint(os);
	}

	// hash map from ULONG to expression
	typedef CHashMap<ULONG, CExpressionMock, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupRelease<CExpressionMock> > HMUlExprMock;

	// map iterator
	typedef CHashMapIter<ULONG, CExpressionMock, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupRelease<CExpressionMock> > HMUlExprMockIter;

}


#endif // !GPOPT_CExpressionMock_H

// EOF
