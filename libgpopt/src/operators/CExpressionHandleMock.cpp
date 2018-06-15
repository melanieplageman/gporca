#include "gpos/base.h"
#include "gpopt/operators/CExpressionHandleMock.h"
#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/operators/CExpression.h"

using namespace gpnaucrates;
using namespace gpopt;

// ctor
CExpressionHandleMock::CExpressionHandleMock(IMemoryPool *pmp)
:
CExpressionHandle(pmp)
{

}

// dtor
CExpressionHandleMock::~CExpressionHandleMock()
{
	CRefCount::SafeRelease(m_pexpr);
}

void
CExpressionHandleMock::Attach
		(
		CExpression *pexpr
		)
{

	// increment ref count on base expression
	pexpr->AddRef();
	m_pexpr = pexpr;
}

void
CExpressionHandleMock::DeriveProps
	(
	CDrvdPropCtxt *pdpctxt
	)
{
	/* GPOS_ASSERT(NULL == m_pdrgpdp); */
	/* GPOS_ASSERT(NULL == m_pdp); */
	GPOS_CHECK_ABORT;

	/* if (NULL != m_pgexpr) */
	/* { */
	/* 	CopyGroupProps(); */
	/* 	return; */
	/* } */
	/* GPOS_ASSERT(NULL != m_pexpr); */

	// check if expression already has derived props
	/* if (NULL != m_pexpr->Pdp(m_pexpr->Ept())) */
	/* { */
	/* 	// When an expression handle is attached to a group expression, there is no */
	/* 	// actual work that needs to happen in terms of property derivation. This */
	/* 	// is because a group expression must originate from a Memo group. At the */
	/* 	// first time a Memo group is created, the relational/scalar properties are */
	/* 	// copied from the expression that caused the creation of the group to the */
	/* 	// property containers of the group itself. So, simply copy the properties */
	/* 	// from the group to the handle. */
	/* 	CopyExprProps(); */
	/* 	return; */
	/* } */

	// copy stats of attached expression
	/* CopyStats(); */

	// extract children's properties
	//m_pdrgpdp = GPOS_NEW(m_pmp) DrgPdp(m_pmp);
	GPOS_ASSERT(pdpctxt == NULL);

//	const ULONG ulArity = m_pexpr->UlArity();
//	for (ULONG ul = 0; ul < ulArity; ul++)
//	{
//		CExpression *pexprChild = (*m_pexpr)[ul];
//		CDrvdProp *pdp = pexprChild->PdpDerive(pdpctxt);
//		pdp->AddRef();
//		m_pdrgpdp->Append(pdp);
//
//		// add child props to derivation context
//		gpopt::CDrvdPropCtxt::AddDerivedProps(pdp, pdpctxt);
//	}

	// create/derive local properties
	//m_pdp = Pop()->PdpCreate(m_pmp);
//	m_pdp->Derive(m_pmp, *this, pdpctxt);
}

// EOF
