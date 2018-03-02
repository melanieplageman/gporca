//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CStatsPredUnsupported.h
//
//	@doc:
//		Class representing unsupported statistics filter
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CStatsPredUnsupported_H
#define GPNAUCRATES_CStatsPredUnsupported_H

#include "gpos/base.h"
#include "gpos/common/CDouble.h"
#include "naucrates/statistics/CStatsPred.h"

namespace gpnaucrates
{
	using namespace gpos;

	//---------------------------------------------------------------------------
	//	@class:
	//		CStatsPredUnsupported
	//
	//	@doc:
	//		Class representing unsupported statistics filter
	//---------------------------------------------------------------------------
	class CStatsPredUnsupported : public CStatsPred
	{
		private:

			// predicate comparison type
			CStatsPred::EStatsCmpType m_estatscmptype;


			// private copy ctor
			CStatsPredUnsupported(const CStatsPredUnsupported &);

		public:

			// ctors
			CStatsPredUnsupported(ULONG ulColId, CStatsPred::EStatsCmpType espt);


			// comparison types for stats computation
			virtual
			CStatsPred::EStatsCmpType Estatscmptype() const
			{
				return m_estatscmptype;
			}



			// conversion function
			static
			CStatsPredUnsupported *PstatspredConvert
				(
				CStatsPred *pstatspred
				)
			{
				GPOS_ASSERT(NULL != pstatspred);
				GPOS_ASSERT(CStatsPred::EsptUnsupported == pstatspred->Espt());

				return dynamic_cast<CStatsPredUnsupported*>(pstatspred);
			}

			void
			UpdateScaleFactor(CJoinStatsConfig joinStatsConfig, CStatsPredUnsupported statsPredUnsupported)
			{
				// Update joinstatsconfig.scaleFactor using statsPredUnsupported.m_drgPdoubleStats
				// pdrgndvs, pdrgmcvs, etc
			}


	}; // class CStatsPredUnsupported
}

#endif // !GPNAUCRATES_CStatsPredUnsupported_H

// EOF
