//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Inc.
//
//	@filename:
//		CHistogramUtils.cpp
//
//	@doc:
//		Utility functions that operate on histograms
//---------------------------------------------------------------------------

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/statistics/CHistogramUtils.h"
#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CScaleFactorUtils.h"

#include "gpos/common/CBitSet.h"
#include "gpos/memory/CAutoMemoryPool.h"

#include "gpopt/base/CColumnFactory.h"

#include "gpopt/engine/CStatisticsConfig.h"
#include "gpopt/optimizer/COptimizerConfig.h"

using namespace gpopt;

// append given histograms to current object
void
CHistogramUtils::AddHistograms
	(
	IMemoryPool *pmp,
	HMUlHist *phmulhistSrc,
	HMUlHist *phmulhistDest
	)
{
	HMIterUlHist hmiterulhist(phmulhistSrc);
	while (hmiterulhist.FAdvance())
	{
		ULONG ulColId = *(hmiterulhist.Pk());
		const CHistogram *phist = hmiterulhist.Pt();
		CStatisticsUtils::AddHistogram(pmp, ulColId, phist, phmulhistDest);
	}
}

// add dummy histogram buckets and column information for the array of columns
void
CHistogramUtils::AddDummyHistogramAndWidthInfo
	(
	IMemoryPool *pmp,
	CColumnFactory *pcf,
	HMUlHist *phmulhistOutput,
	HMUlDouble *phmuldoubleWidthOutput,
	const DrgPul *pdrgpul,
	BOOL fEmpty
	)
{
	GPOS_ASSERT(NULL != pcf);
	GPOS_ASSERT(NULL != phmulhistOutput);
	GPOS_ASSERT(NULL != phmuldoubleWidthOutput);
	GPOS_ASSERT(NULL != pdrgpul);

	const ULONG ulCount = pdrgpul->UlLength();
	// for computed aggregates, we're not going to be very smart right now
	for (ULONG ul = 0; ul < ulCount; ul++)
	{
		ULONG ulColId = *(*pdrgpul)[ul];

		CColRef *pcr = pcf->PcrLookup(ulColId);
		GPOS_ASSERT(NULL != pcr);

		CHistogram *phist = CHistogram::PhistDefault(pmp, pcr, fEmpty);
		phmulhistOutput->FInsert(GPOS_NEW(pmp) ULONG(ulColId), phist);

		CDouble dWidth = CStatisticsUtils::DDefaultColumnWidth(pcr->Pmdtype());
		phmuldoubleWidthOutput->FInsert(GPOS_NEW(pmp) ULONG(ulColId), GPOS_NEW(pmp) CDouble(dWidth));
	}
}

//	add empty histogram for the columns in the input histogram
void
CHistogramUtils::AddEmptyHistogram
	(
	IMemoryPool *pmp,
	HMUlHist *phmulhistOutput,
	HMUlHist *phmulhistInput
	)
{
	GPOS_ASSERT(NULL != phmulhistOutput);
	GPOS_ASSERT(NULL != phmulhistInput);

	HMIterUlHist hmiterulhist(phmulhistInput);
	while (hmiterulhist.FAdvance())
	{
		ULONG ulColId = *(hmiterulhist.Pk());

		// empty histogram
		CHistogram *phist =  GPOS_NEW(pmp) CHistogram(GPOS_NEW(pmp) DrgPbucket(pmp), false /* fWellDefined */);
		phmulhistOutput->FInsert(GPOS_NEW(pmp) ULONG(ulColId), phist);
	}
}

// EOF

