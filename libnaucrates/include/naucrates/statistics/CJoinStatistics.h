//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CJoinStatistics.h
//
//	@doc:
//		Statistics implementation over 1D histograms for joins
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CJoinStatistics_H
#define GPNAUCRATES_CJoinStatistics_H

#include "gpos/base.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/sync/CMutex.h"

#include "naucrates/statistics/IStatistics.h"
#include "naucrates/statistics/CJoinStatistics.h"
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
	//		CJoinStatistics
	//
	//	@doc:
	//		Join Statistics
	//---------------------------------------------------------------------------
	class CJoinStatistics
	{


		private:
			IStatistics::EStatsJoinType m_eStatsJoinType;

		public:
		CJoinStatistics(){}
		CJoinStatistics(IStatistics::EStatsJoinType m_eStatsJoinType) : m_eStatsJoinType(m_eStatsJoinType)
		{}

		IStatistics::EStatsJoinType EstatsJoinType() const
			{
				return m_eStatsJoinType;
			}

	}; // class CJoinStatistics

}

#endif // !GPNAUCRATES_CJoinStatistics_H

// EOF
