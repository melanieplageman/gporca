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

	if (ErtNone == prs->Ert())
	{
		return true;
	}

	if (ErtNoneDueToMotion == prs->Ert())
	{
		if (ErtNone == Ert())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	if (ErtGeneralBlocking == prs->Ert())
	{
		if (/*ErtGeneralStreamingMotionHazard != Ert() &&*/
			ErtNoneDueToMotion != Ert())
		{
			return true;
		}
	}

	if (ErtGeneralStreaming == prs->Ert())
	{
		if (ErtGeneralStreamingMotionHazard == Ert() ||
			ErtGeneralBlocking == Ert())
		{
			return true;
		}
		else
		{
			return false;
		}
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
	CExpressionHandle &exprhdl, // exprhdl
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
	if ((prs->Ert() == CRewindabilitySpec::ErtNoneDueToMotion || prs->Ert() == CRewindabilitySpec::ErtGeneralStreamingMotionHazard)
		&& prpp->Per()->PrsRequired()->Ert() == CRewindabilitySpec::ErtGeneralBlocking)
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
		case ErtGeneralStreaming:
			return os << "REWINDABLE STREAMING";

		case ErtGeneralBlocking:
			return os << "REWINDABLE BLOCKING";
			
		case ErtGeneralStreamingMotionHazard:
			return os << "REWINDABLE STREAMING MOTION HAZARD";

		case ErtNone:
			return os << "NON-REWINDABLE";

		case ErtNoneDueToMotion:
			return os << "NON-REWINDABLE DUE TO MOTION";

		default:
			GPOS_ASSERT(!"Unrecognized rewindability type");
			return os;
	}
}


// EOF

