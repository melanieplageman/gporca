//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.

#include <stdint.h>

#include "gpos/io/COstreamString.h"
#include "gpos/string/CWStringDynamic.h"

#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"

#include "naucrates/dxl/CDXLUtils.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/dxl/statistics/CJoinCardinalityUnsupportedPredTest.h"

const CHAR *szFileName = "../data/dxl/statistics/Join-Statistics-UnsupportedPred-Input.xml";
//
//GPOS_RESULT
//CJoinCardinalityUnsupportedPredTest::EresUnittest()
//{
//
//
//	CJoinCardinalityUnsupportedPredTest f(szFileName);
//	IMemoryPool *pmp = f.Pmp();
//	CDouble dRowsExpected(25334667);
//// am getting 1900100 after doing the below
//
//	CStatistics *pstatsOuter = (*f.PdrgPstatJoinOuterInner())[0];
//	CStatistics *pstatsInner = (*f.PdrgPstatJoinOuterInner())[1];
//	DrgPstatspredjoin *pdrgpstatspredjoin = f.Pdrgpstatspredjoin();
//
//	// calculate the output stats
//	CStatistics *pstatsJoin = NULL;
//	pstatsJoin = pstatsOuter->PstatsInnerJoin(pmp, pstatsInner, pdrgpstatspredjoin);
//	GPOS_ASSERT(NULL != pstatsJoin);
//	CDouble dRowsActual(pstatsJoin->DRows());
//
//	GPOS_ASSERT(dRowsActual == dRowsExpected);
//	return GPOS_OK;
//}

GPOS_RESULT
CJoinCardinalityUnsupportedPredTest::EresUnittest()
{


	CJoinCardinalityUnsupportedPredTest f(szFileName);
	IMemoryPool *pmp = f.Pmp();
	CDouble dRowsExpected(25334667);
// am getting 1900100 after doing the below

	CStatistics *pstatsOuter = (*f.PdrgPstatJoinOuterInner())[0];
	CStatistics *pstatsInner = (*f.PdrgPstatJoinOuterInner())[1];
	DrgPstatspredjoin *pdrgpstatspredjoin = f.Pdrgpstatspredjoin();
	CDouble dRowsOuter(10000);

	CStatsPred *pstatspredUnsupported = GPOS_NEW(pmp) CStatsPred(16);

	// calculate the output stats
	CStatistics *pstatsJoin = NULL;
	pstatsJoin = CJoinStatsProcessor::getJoinStats(pmp, dRowsOuter, pstatsOuter, pstatsInner, pstatsJoin, pdrgpstatspredjoin, IStatistics::EsjtInnerJoin);
	GPOS_ASSERT(NULL != pstatsJoin);
	CDouble dRowsActual(pstatsJoin->DRows());

	GPOS_ASSERT(dRowsActual == dRowsExpected);
	return GPOS_OK;
}

// EOF
