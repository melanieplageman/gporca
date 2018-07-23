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
	if (FMatch(prs))
	{
		return true;
	}

	if (prs->Ert() == ErtNotRewindableNoMotion || prs->Ert() == ErtNotRewindableMotion)
	{
		return true;
	}

	if (prs->Ert() == ErtRewindableNoMotion && Ert() == ErtRewindableMotion)
	{
		return true;
	}

	if (prs->Ert() == ErtRewindableMotion && Ert() == ErtRewindableNoMotion)
	{
		return true;
	}


	return false;
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
	// if required is rewindablemotion and derived is notrewindablemotion , then eager = true
	// do we need eager=true when derived is rewindable but has a motion (see diagram, maybe if already rewindable, we won't add a spool)
	//if(prpp->Per()->PrsRequired()->Ert() == CRewindabilitySpec::ErtRewindableMotion &&
	  // prs->Ert() == CRewindabilitySpec::ErtNotRewindableMotion)
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

