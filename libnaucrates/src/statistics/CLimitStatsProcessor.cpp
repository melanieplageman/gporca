//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2018 Pivotal, Inc.
//
//	@filename:
//		CLimitStatsProcessor.cpp
//
//	@doc:
//		Statistics helper routines for processing all join types
//---------------------------------------------------------------------------

#include "gpopt/operators/ops.h"
#include "gpopt/optimizer/COptimizerConfig.h"

#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CLimitStatsProcessor.h"
#include "naucrates/statistics/CHistogramUtils.h"

using namespace gpopt;

//	compute the statistics of a limit operation
CStatistics *
CLimitStatsProcessor::PstatsLimit
	(
	IMemoryPool *pmp,
	const CStatistics *pstatsInput,
	CDouble dLimitCount
	)
{
	GPOS_ASSERT(NULL != pstatsInput);

	// create hash map from colid -> histogram for resultant structure
	HMUlHist *phmulhistNew = GPOS_NEW(pmp) HMUlHist(pmp);

	CDouble dRowsLimit = CStatistics::DMinRows;
	if (pstatsInput->FEmpty())
	{
		CHistogramUtils::AddEmptyHistogram(pmp, phmulhistNew, pstatsInput->PHMUlHist());
	}
	else
	{
		HMIterUlHist hmiterulhist(pstatsInput->PHMUlHist());
		while (hmiterulhist.FAdvance())
		{
			ULONG ulColId = *(hmiterulhist.Pk());
			const CHistogram *phist = hmiterulhist.Pt();
			CStatisticsUtils::AddHistogram(pmp, ulColId, phist, phmulhistNew);
		}

		dRowsLimit = std::max(CStatistics::DMinRows, dLimitCount);
	}

	HMUlDouble *phmuldWidth = pstatsInput->PHMUlDoubleWidth();
	phmuldWidth->AddRef();

	// create an output stats object
	CStatistics *pstatsLimit = GPOS_NEW(pmp) CStatistics
											(
											pmp,
											phmulhistNew,
											phmuldWidth,
											dRowsLimit,
											pstatsInput->FEmpty(),
											pstatsInput->UlNumberOfPredicates()
											);

	// In the output statistics object, the upper bound source cardinality of the join column
	// cannot be greater than the upper bound source cardinality information maintained in the input
	// statistics object. Therefore we choose CStatistics::EcbmMin the bounding method which takes
	// the minimum of the cardinality upper bound of the source column (in the input hash map)
	// and estimated limit cardinality.

	// modify source id to upper bound card information
	CStatisticsUtils::ComputeCardUpperBounds(pmp, pstatsInput, pstatsLimit, dRowsLimit, CStatistics::EcbmMin /* ecbm */);

	return pstatsLimit;
}

// EOF
