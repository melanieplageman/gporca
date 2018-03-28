//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Inc.
//
//	@filename:
//		CHistogramUtils.h
//
//	@doc:
//		Utility functions that operate on histograms
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CHistogramUtils_H
#define GPNAUCRATES_CHistogramUtils_H

#include "gpos/base.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/sync/CMutex.h"

#include "naucrates/statistics/CStatsPredDisj.h"
#include "naucrates/statistics/CStatsPredConj.h"
#include "naucrates/statistics/CStatsPredLike.h"
#include "naucrates/statistics/CStatsPredUnsupported.h"

#include "naucrates/statistics/CHistogram.h"
#include "gpos/common/CBitSet.h"

namespace gpopt
{
	class CStatisticsConfig;
	class CColumnFactory;
}

namespace gpnaucrates
{
	using namespace gpos;
	using namespace gpopt;

	//---------------------------------------------------------------------------
	//	@class:
	//		CHistogramUtils
	//
	//	@doc:
	//		Utility functions that operate on histograms
	//---------------------------------------------------------------------------
	class CHistogramUtils
	{
		public:

			// helper method to append histograms from one map to the other
			static
			void AddHistograms(IMemoryPool *pmp, HMUlHist *phmulhistSrc, HMUlHist *phmulhistDest);

			// add dummy histogram buckets and column width for the array of columns
			static
			void AddDummyHistogramAndWidthInfo
				(
				IMemoryPool *pmp,
				CColumnFactory *pcf,
				HMUlHist *phmulhistOutput,
				HMUlDouble *phmuldoubleWidthOutput,
				const DrgPul *pdrgpul,
				BOOL fEmpty
				);

			// add dummy histogram buckets for the columns in the input histogram
			static
			void AddEmptyHistogram(IMemoryPool *pmp, HMUlHist *phmulhistOutput, HMUlHist *phmulhistInput);

	}; // class CHistogramUtils
}

#endif // !GPNAUCRATES_CHistogramUtils_H

// EOF
