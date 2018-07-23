//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CRewindabilitySpec.cpp
//
//	@doc:
//		Specification of rewindability of intermediate query results
//---------------------------------------------------------------------------

#include "gpopt/base/CRewindabilitySpec.h"
#include "gpopt/operators/CPhysicalSpool.h"
#include "gpopt/operators/CExpressionHandle.h"


using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::CRewindabilitySpec
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CRewindabilitySpec::CRewindabilitySpec
	(
	ERewindabilityType ert
	)
	:
	m_ert(ert)
{}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::~CRewindabilitySpec
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CRewindabilitySpec::~CRewindabilitySpec()
{}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::FMatch
//
//	@doc:
//		Check for equality between rewindability specs
//
//---------------------------------------------------------------------------
BOOL
CRewindabilitySpec::FMatch
	(
	const CRewindabilitySpec *prs
	)
	const
{
	GPOS_ASSERT(NULL != prs);

	return Ert() == prs->Ert();
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::FSatisfies
//
//	@doc:
//		Check if this rewindability spec satisfies the given one
//
//---------------------------------------------------------------------------
BOOL
CRewindabilitySpec::FSatisfies
	(
	const CRewindabilitySpec *prs
	)
	const
{
	return
		FMatch(prs) ||
		ErtNotRewindableNoMotion == prs->Ert() ||
		(ErtMarkRestore == Ert() && ErtRewindableNoMotion == prs->Ert());
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::UlHash
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CRewindabilitySpec::UlHash() const
{
	return gpos::UlHash<ERewindabilityType>(&m_ert);
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::AppendEnforcers
//
//	@doc:
//		Add required enforcers to dynamic array
//
//---------------------------------------------------------------------------
void
CRewindabilitySpec::AppendEnforcers
	(
	IMemoryPool *pmp,
	CExpressionHandle &exprhdl,
	CReqdPropPlan *prpp,
	DrgPexpr *pdrgpexpr, 
	CExpression *pexpr
	)
{
	GPOS_ASSERT(NULL != prpp);
	GPOS_ASSERT(NULL != pmp);
	GPOS_ASSERT(NULL != pdrgpexpr);
	GPOS_ASSERT(NULL != pexpr);
	GPOS_ASSERT(this == prpp->Per()->PrsRequired() &&
				"required plan properties don't match enforced rewindability spec");

	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	BOOL eager = false;
	if(prpp->Per()->PrsRequired()->Ert() == CRewindabilitySpec::ErtRewindableMotion &&
	   (prs->Ert() == CRewindabilitySpec::ErtNotRewindableMotion || prs->Ert() == CRewindabilitySpec::ErtRewindableMotion))
	{
		eager = true;
	}

	pexpr->AddRef();
	CExpression *pexprSpool = GPOS_NEW(pmp) CExpression
									(
									pmp, 
									GPOS_NEW(pmp) CPhysicalSpool(pmp, eager),
									pexpr
									);
	pdrgpexpr->Append(pexprSpool);
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::OsPrint
//
//	@doc:
//		Print rewindability spec
//
//---------------------------------------------------------------------------
IOstream &
CRewindabilitySpec::OsPrint
	(
	IOstream &os
	)
	const
{
	switch (Ert())
	{
		case ErtRewindableNoMotion:
			return os << "REWINDABLE NO MOTION";

		case ErtRewindableMotion:
			return os << "REWINDABLE MOTION";

		case ErtNotRewindableMotion:
			return os << "NON-REWINDABLE MOTION";

		case ErtMarkRestore:
			return os << "MARK-RESTORE";

		case ErtNotRewindableNoMotion:
			return os << "NON-REWINDABLE NO MOTION";

		default:
			GPOS_ASSERT(!"Unrecognized rewindability type");
			return os;
	}
}


// EOF

