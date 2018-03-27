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

using namespace gpnaucrates;

const CHAR *szFileName = "../data/dxl/statistics/Join-Statistics-UnsupportedPred-Input.xml";
GPOS_RESULT
CJoinCardinalityUnsupportedPredTest::EresUnittest()
{
	CJoinCardinalityUnsupportedPredTest f(szFileName);
	IMemoryPool *pmp = f.Pmp();
	CDouble dRowsExpected(19001);
// am getting 1900100 after doing the below
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


// EOF
