	//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IStatistics.h
//
//	@doc:
//		Abstract base statistics
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_IStatistics_H
#define GPNAUCRATES_IStatistics_H

#include "gpos/base.h"
#include "gpos/common/CBitSet.h"
#include "gpos/common/CHashMapIter.h"

#include "naucrates/statistics/CStatsPred.h"
#include "naucrates/statistics/CStatsPredPoint.h"
#include "naucrates/statistics/CStatsPredJoin.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/md/CDXLStatsDerivedRelation.h"

#include "gpopt/base/CColRef.h"

namespace gpopt
{
	class CMDAccessor;
	class CReqdPropRelational;
	class CColRefSet;
}

namespace gpnaucrates
{
	using namespace gpos;
	using namespace gpmd;
	using namespace gpopt;

	// fwd declarations
	class IStatistics;

	// hash map from column id to a histogram
	typedef CHashMap<ULONG, CHistogram, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupDelete<CHistogram> > HMUlHist;

	// iterator
	typedef CHashMapIter<ULONG, CHistogram, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupDelete<CHistogram> > HMIterUlHist;

	// hash map from column ULONG to CDouble
	typedef CHashMap<ULONG, CDouble, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupDelete<CDouble> > HMUlDouble;

	// iterator
	typedef CHashMapIter<ULONG, CDouble, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupDelete<CDouble> > HMIterUlDouble;

	typedef CHashMap<ULONG, ULONG, gpos::UlHash<ULONG>, gpos::FEqual<ULONG>,
					CleanupDelete<ULONG>, CleanupDelete<ULONG> > HMUlUl;

	// hash maps mapping INT -> ULONG
	typedef CHashMap<INT, ULONG, gpos::UlHash<INT>, gpos::FEqual<INT>,
					CleanupDelete<INT>, CleanupDelete<ULONG> > HMIUl;

	//---------------------------------------------------------------------------
	//	@class:
	//		IStatistics
	//
	//	@doc:
	//		Abstract statistics API
	//
	//---------------------------------------------------------------------------
	class IStatistics: public CRefCount
	{
		private:

			// private copy ctor
			IStatistics(const IStatistics &);

			// private assignment operator
			IStatistics& operator=(IStatistics &);

		public:

			// ctor
			IStatistics()
			{}

			// dtor
			virtual
			~IStatistics()
			{}

			// how many rows
			virtual
			CDouble DRows() const = 0;

			// is statistics on an empty input
			virtual
			BOOL FEmpty() const = 0;

			// statistics could be computed using predicates with external parameters (outer references),
			// this is the total number of external parameters' values
			virtual
			CDouble DRebinds() const = 0;

			// skew estimate for given column
			virtual
			CDouble DSkew(ULONG ulColId) const = 0;

			// what is the width in bytes
			virtual
			CDouble DWidth() const = 0;

			// what is the width in bytes of set of column id's
			virtual
			CDouble DWidth(DrgPul *pdrgpulColIds) const = 0;

			// what is the width in bytes of set of column references
			// TODO: change this to being for a single column
			virtual
			CDouble DWidth(IMemoryPool *pmp, CColRefSet *pcrs) const = 0;



			// look up the number of distinct values of a particular column
			virtual
			CDouble DNDV(const CColRef *pcr) = 0;

			// print function
			virtual
			IOstream &OsPrint(IOstream &os) const = 0;

	}; // class IStatistics

	// shorthand for printing
	inline
	IOstream &operator << (IOstream &os, IStatistics &stat)
	{
		return stat.OsPrint(os);
	}
	// release istats
	inline void CleanupStats(IStatistics *pstats)
	{
		if (NULL != pstats)
		{
			(dynamic_cast<CRefCount*>(pstats))->Release();
		}
	}

	// dynamic array for derived stats
	typedef CDynamicPtrArray<IStatistics, CleanupStats> DrgPstat;
}

#endif // !GPNAUCRATES_IStatistics_H

// EOF
