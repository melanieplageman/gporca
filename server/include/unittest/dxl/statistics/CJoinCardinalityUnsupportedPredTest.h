//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.

#ifndef GPNAUCRATES_CJoinCardinalityUnsupportedPredTest_H
#define GPNAUCRATES_CJoinCardinalityUnsupportedPredTest_H

#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CBucket.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CStatistics.h"
#include "unittest/gpopt/CTestUtils.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"


namespace gpnaucrates
{
	class CJoinCardinalityUnsupportedPredTest
	{
	private:
		const CAutoMemoryPool m_amp;
		CMDAccessor m_mda;
		const CAutoOptCtxt m_aoc;
		DrgPstats *m_pdrgpstatJoinOuterInner;
		DrgPstatspredjoin *m_pdrgpstatspredjoin;
		static IMDProvider *Pmdp()
		{
			CTestUtils::m_pmdpf->AddRef();
			return CTestUtils::m_pmdpf;
		}
	public:
		CJoinCardinalityUnsupportedPredTest(const CHAR *szFileName):
				m_amp(),
				m_mda(m_amp.Pmp(), CMDCache::Pcache(), CTestUtils::m_sysidDefault, Pmdp()),
				m_aoc(m_amp.Pmp(), &m_mda, NULL /* pceeval */, CTestUtils::Pcm(m_amp.Pmp())),
				m_pdrgpstatspredjoin(NULL)
		{
			// Make the array of statistics for outer and inner side of join
			// In case there is ever need for an N-ary join which is not binary
			// Store it in an array
			CHAR *szDXLInput = CDXLUtils::SzRead(Pmp(), szFileName);
			GPOS_CHECK_ABORT;
			DrgPdxlstatsderrel *pdrgpdxlstatsderrel = CDXLUtils::PdrgpdxlstatsderrelParseDXL(Pmp(), szDXLInput, NULL);
			m_pdrgpstatJoinOuterInner = CDXLUtils::PdrgpstatsTranslateStats(Pmp(), &m_mda, pdrgpdxlstatsderrel);
			pdrgpdxlstatsderrel->Release();
			GPOS_ASSERT(NULL != m_pdrgpstatJoinOuterInner);
			GPOS_ASSERT(2 == m_pdrgpstatJoinOuterInner->UlLength());
			m_pdrgpstatspredjoin = GPOS_NEW(Pmp()) DrgPstatspredjoin(Pmp());

			// Make the array of CStatsPredJoin
			// Store it in an array so you can have any number of predicates
			// Here initialize it with a default predicate
			CStatsPredJoin *pStatsPredJoin = GPOS_NEW(Pmp()) CStatsPredJoin(16, CStatsPred::EstatscmptOther, 31);
			m_pdrgpstatspredjoin->Append(pStatsPredJoin);
			GPOS_DELETE_ARRAY(szDXLInput);
		}

		~CJoinCardinalityUnsupportedPredTest()
		{
			m_pdrgpstatJoinOuterInner->Release();
			m_pdrgpstatspredjoin->Release();
		}

		IMemoryPool *Pmp() const
		{
			return m_amp.Pmp();
		}

		DrgPstats *PdrgPstatJoinOuterInner()
		{
			return m_pdrgpstatJoinOuterInner;
		}

		DrgPstatspredjoin *Pdrgpstatspredjoin()
		{
			GPOS_ASSERT(m_pdrgpstatspredjoin != NULL);
			return m_pdrgpstatspredjoin;
		}
		static GPOS_RESULT EresUnittest();
	};
}

#endif // !GPNAUCRATES_CJoinCardinalityUnsupportedPredTest_H


// EOF
