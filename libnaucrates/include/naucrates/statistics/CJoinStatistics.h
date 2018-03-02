//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CStatistics.h
//
//	@doc:
//		Statistics implementation over 1D histograms
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CStatistics_H
#define GPNAUCRATES_CStatistics_H

#include "gpos/base.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/sync/CMutex.h"

#include "naucrates/statistics/IStatistics.h"
#include "naucrates/statistics/CStatsPredDisj.h"
#include "naucrates/statistics/CStatsPredConj.h"
#include "naucrates/statistics/CStatsPredLike.h"
#include "naucrates/statistics/CStatsPredUnsupported.h"
#include "naucrates/statistics/CUpperBoundNDVs.h"

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
	using namespace gpdxl;
	using namespace gpmd;
	using namespace gpopt;

	// hash maps ULONG -> array of ULONGs
	typedef CHashMap<ULONG, DrgPul, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupRelease<DrgPul> > HMUlPdrgpul;

	// iterator
	typedef CHashMapIter<ULONG, DrgPul, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
		CleanupDelete<ULONG>, CleanupRelease<DrgPul> > HMIterUlPdrgpul;

	//---------------------------------------------------------------------------
	//	@class:
	//		CStatistics
	//
	//	@doc:
	//		Abstract statistics API
	//---------------------------------------------------------------------------
	class CStatistics: public IStatistics
	{
		public:
			mutateCardinality(CJoinStatsConfig)
		// return join cardinality based on scaling factor and join type
		static
		CDouble DJoinCardinality
				(
						CStatisticsConfig *pstatsconf,
						CDouble dRowsLeft,
						CDouble dRowsRight,
						DrgPdouble *pdrgpd,
						IStatistics::EStatsJoinType esjt
				);
// make a new parent class and then subclass this and make another separate subclass which doesn't have all this stuff
		CStatistics
				(
						IMemoryPool *pmp,
						HMUlDouble *phmuldoubleWidth,
						CDouble dRows,
						BOOL fEmpty,
						BOOL fUnsupported,
						ULONG ulNumPredicates
				);
				// method used to compute for columns of each source it corresponding
				// the cardinality upper bound
				enum ECardBoundingMethod
				{
					EcbmOutputCard = 0, // use the estimated output cardinality as the upper bound cardinality of the source
					EcbmInputSourceMaxCard, // use the upper bound cardinality of the source in the input statistics object
					EcbmMin, // use the minimum of the above two cardinality estimates

					EcbmSentinel
				};


		private:

			// private copy ctor
			CStatistics(const CStatistics &);

			// private assignment operator
			CStatistics& operator=(CStatistics &);

	}; // class CStatistics

}

#endif // !GPNAUCRATES_CStatistics_H

// EOF
