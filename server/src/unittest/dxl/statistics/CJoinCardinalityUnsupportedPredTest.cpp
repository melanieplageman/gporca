//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.

#include <stdint.h>

#include "gpos/io/COstreamString.h"
#include "gpos/string/CWStringDynamic.h"

#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"
#include "naucrates/statistics/CStatsPred.h"
#include "naucrates/statistics/CStatsPredJoin.h"
#include "naucrates/dxl/CDXLUtils.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/dxl/statistics/CJoinCardinalityUnsupportedPredTest.h"
#include "unittest/dxl/statistics/CJoinCardinalityUnsupportedPredTest.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"
#include "naucrates/statistics/CStatsPredUnsupported.h"
#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CBucket.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CStatistics.h"
#include "unittest/gpopt/CTestUtils.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"

namespace gpnaucrates
{

	class CJoinCardinalityUnsupportedPredTestFixture
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
		CJoinCardinalityUnsupportedPredTestFixture(const CHAR *szFileName):
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

		~CJoinCardinalityUnsupportedPredTestFixture()
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
	};

	const CHAR *szFileName = "../data/dxl/statistics/Join-Statistics-UnsupportedPred-Input.xml";
	GPOS_RESULT
	CJoinCardinalityUnsupportedPredTest::EresUnittest()
	{
		CJoinCardinalityUnsupportedPredTestFixture f(szFileName);
		IMemoryPool *pmp = f.Pmp();
		CDouble dRowsExpected(1900100);
		IStatistics *pstatsOuter = (*f.PdrgPstatJoinOuterInner())[0]->PstatsCopy(pmp);
		IStatistics *pstatsInner = (*f.PdrgPstatJoinOuterInner())[1];
		DrgPstatspredjoin *pdrgpstatspredjoin = f.Pdrgpstatspredjoin();
		CDouble dRowsOuter(10000);
		BOOL fLeftOuterJoin = false;

		CStatsPredUnsupported *pstatspredUnsupported = NULL;

		// calculate the output stats
		IStatistics *pstatsJoin = NULL;

		pstatsJoin = CJoinStatsProcessor::GetJoinStats(pmp, fLeftOuterJoin, dRowsOuter,
													   pstatsOuter, pstatsInner, pdrgpstatspredjoin, pstatspredUnsupported);

		GPOS_ASSERT(NULL != pstatsJoin);
		CDouble dRowsActual(pstatsJoin->DRows());
		GPOS_RESULT eres = GPOS_OK;

		if (dRowsActual != dRowsExpected)
		{
			eres = GPOS_FAILED;
		}

		pstatsJoin->Release();

		return eres;

	}
}

// EOF
