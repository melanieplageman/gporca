//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CExpressionMock.cpp
//
//	@doc:
//		Mock implementation of expressions
//---------------------------------------------------------------------------

#include "gpos/base.h"

#include "gpopt/exception.h"

#include "gpopt/base/CAutoOptCtxt.h"
#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CDistributionSpec.h"
#include "gpopt/base/CDrvdPropCtxtRelational.h"
#include "gpopt/base/CDrvdPropCtxtPlan.h"
#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/base/CUtils.h"
#include "gpopt/base/CPrintPrefix.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "gpopt/operators/ops.h"
#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/operators/CExpressionHandleMock.h"
#include "gpopt/search/CGroupExpression.h"
#include "naucrates/traceflags/traceflags.h"

#include "gpopt/operators/CExpressionMock.h"


using namespace gpnaucrates;
using namespace gpopt;


#define GPOPT_TEST_REL_OID1	OID(11111)

//		Ctor
CExpressionMock::CExpressionMock
	(
	IMemoryPool *pmp
	)
	:
	CExpression(pmp)
{
	m_pdrgpexpr = GPOS_NEW(pmp) DrgPexpr(pmp);
	
	GPOS_ASSERT(pmp);
}

//		Ctor with operator
CExpressionMock::CExpressionMock
		(
				IMemoryPool *pmp,
				COperator *pop
		)
		:
		CExpression(pmp, pop)
{
	m_pdrgpexpr = GPOS_NEW(pmp) DrgPexpr(pmp);
	GPOS_ASSERT(pmp);
}




//		Dtor
CExpressionMock::~CExpressionMock()
{
	{
		CAutoSuspendAbort asa;

		CRefCount::SafeRelease(m_pdprel);
		CRefCount::SafeRelease(m_pstats);
		CRefCount::SafeRelease(m_prpp);
		CRefCount::SafeRelease(m_pdpplan);
		CRefCount::SafeRelease(m_pdpscalar);
		CRefCount::SafeRelease(m_pdrgpexpr);

		m_pop->Release();
	}

#ifdef GPOS_DEBUG
	CWorker::PwrkrSelf()->ResetTimeSlice();
#endif // GPOS_DEBUG
}

COperator *
CExpressionMock::Pop() const
{
	if (m_pop == NULL)
				{
					const IMDTypeInt4 *pmdtypeint4 = COptCtxt::PoctxtFromTLS()->Pmda()->PtMDType<IMDTypeInt4>();
					CWStringConst strRelAlias(GPOS_WSZ_LIT("Rel1"));
					CWStringConst strColA(GPOS_WSZ_LIT("a"));
					CWStringConst strColB(GPOS_WSZ_LIT("b"));
					// TODO: redo this without using CStatisticsTest
					CTableDescriptor *ptabdesc = PtabdescTwoColumnSource(m_pmp, CName(&strRelAlias), pmdtypeint4, strColA, strColB);
					/* CExpression *pexprGet = CTestUtils::PexprLogicalGet(m_pmp, ptabdesc, &strRelAlias); */
					/* IMDTypeInt8 *pmdtypeint8 = (IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault); */
					/* IMDId *pmdid = pmdtypeint8->Pmdid(); */
					CLogicalGet *logGet = GPOS_NEW(m_pmp) CLogicalGet
								(
								m_pmp,
								GPOS_NEW(m_pmp) CName(m_pmp, CName(&strRelAlias)),
								ptabdesc);

					/* CScalarIdent *scalarIdent = GPOS_NEW(m_pmp) CScalarIdent(m_pmp, pcr); */
					return logGet;
				}
				return m_pop;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionMock::PdpDerive
//
//	@doc:
//		Derive properties;
//		Determine the suitable derived property type internally
//
//---------------------------------------------------------------------------
CDrvdProp *
CExpressionMock::PdpDerive
	(
	CDrvdPropCtxt *pdpctxt // derivation context, passed by caller
	)
{
	GPOS_CHECK_STACK_SIZE;
	GPOS_CHECK_ABORT;
	const CDrvdProp::EPropType ept = Ept();
#ifdef GPOS_DEBUG
	AssertValidPropDerivation(ept);
#endif // GPOS_DEBUG
	// see if suitable prop is already cached
	 if (NULL == Pdp(ept))
	 {
		CExpressionHandleMock exprhdlmock(m_pmp);
		exprhdlmock.Attach(this);

		// trigger recursive property derivation
		 exprhdlmock.DeriveProps(pdpctxt);

	 	// cache handle's derived properties on expression
	 	CRefCount::SafeRelease(Pdp(ept));
	 	CDrvdProp *pdp = exprhdlmock.Pdp();
	 	pdp->AddRef();
	 	SetPdp(pdp, ept);
	 }
	return Pdp(ept);
}

CTableDescriptor *
CExpressionMock::PtabdescTwoColumnSource
	(
	IMemoryPool *pmp,
	const CName &nameTable,
	const IMDTypeInt4 *pmdtype,
	const CWStringConst &strColA,
	const CWStringConst &strColB
	)
{
	CTableDescriptor *ptabdesc = GPOS_NEW(pmp) CTableDescriptor
									(
									pmp,
									GPOS_NEW(pmp) CMDIdGPDB(GPOPT_TEST_REL_OID1, 1, 1),
									nameTable,
									false, // fConvertHashToRandom
									IMDRelation::EreldistrRandom,
									IMDRelation::ErelstorageHeap,
									0  // ulExecuteAsUser
									);

	for (ULONG ul = 0; ul < 2; ul++)
	{
		// create a shallow constant string to embed in a name
		const CWStringConst *pstrName = &strColA;
		if (0 < ul)
		{
			pstrName = &strColB;
		}
		CName nameColumn(pstrName);

		CColumnDescriptor *pcoldesc = GPOS_NEW(pmp) CColumnDescriptor
											(
											pmp,
											pmdtype,
											IDefaultTypeModifier,
											nameColumn,
											ul + 1,
											false /*fNullable*/
											);
		ptabdesc->AddColumn(pcoldesc);
	}

	return ptabdesc;
}

// EOF
