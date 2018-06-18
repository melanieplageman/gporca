//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		main.cpp
//
//	@doc:
//		Startup routines for optimizer
//---------------------------------------------------------------------------

#include "gpos/_api.h"
#include "gpos/types.h"
#include "gpopt/init.h"

#include "naucrates/init.h"

#include "gpos/common/CMainArgs.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpos/test/CFSimulatorTestExt.h"
#include "gpos/test/CUnittest.h"
#include "gpos/test/CTimeSliceTest.h"


#include "gpopt/engine/CEnumeratorConfig.h"
#include "gpopt/engine/CStatisticsConfig.h"
#include "gpopt/mdcache/CMDCache.h"
#include "gpopt/minidump/CMinidumperUtils.h"
#include "gpopt/xforms/CXformFactory.h"
#include "gpopt/optimizer/COptimizerConfig.h"

#include "gpdbcost/CCostModelGPDBLegacy.h"

// test headers

#include "unittest/base.h"
#include "unittest/gpopt/search/CTreeMapTest.h"

#include "unittest/dxl/CDXLMemoryManagerTest.h"
#include "unittest/dxl/CDXLUtilsTest.h"
#include "unittest/dxl/CParseHandlerManagerTest.h"
#include "unittest/dxl/CParseHandlerTest.h"
#include "unittest/dxl/CParseHandlerCostModelTest.h"
#include "unittest/dxl/CParseHandlerOptimizerConfigSerializeTest.h"

#include "unittest/dxl/CXMLSerializerTest.h"

#include "unittest/dxl/base/CDatumTest.h"

#include "unittest/gpopt/CTestUtils.h"
#include "unittest/gpopt/base/CColRefSetIterTest.h"
#include "unittest/gpopt/base/CColRefSetTest.h"
#include "unittest/gpopt/base/CColumnFactoryTest.h"
#include "unittest/gpopt/base/CDistributionSpecTest.h"
#include "unittest/gpopt/base/CEquivalenceClassesTest.h"
#include "unittest/gpopt/base/CFunctionalDependencyTest.h"
#include "unittest/gpopt/base/CKeyCollectionTest.h"
#include "unittest/gpopt/base/CMaxCardTest.h"
#include "unittest/gpopt/base/CStateMachineTest.h"
#include "unittest/gpopt/base/COrderSpecTest.h"
#include "unittest/gpopt/base/CRangeTest.h"
#include "unittest/gpopt/base/CConstraintTest.h"
#include "unittest/gpopt/engine/CEngineTest.h"
#include "unittest/gpopt/engine/CEnumeratorTest.h"

#include "unittest/gpopt/metadata/CColumnDescriptorTest.h"
#include "unittest/gpopt/metadata/CNameTest.h"
#include "unittest/gpopt/metadata/CTableDescriptorTest.h"
#include "unittest/gpopt/metadata/CIndexDescriptorTest.h"
#include "unittest/gpopt/metadata/CPartConstraintTest.h"

#include "unittest/gpopt/mdcache/CMDAccessorTest.h"
#include "unittest/gpopt/mdcache/CMDProviderTest.h"

#include "unittest/gpopt/minidump/CArrayExpansionTest.h"
#include "unittest/gpopt/minidump/CJoinOrderDPTest.h"
#include "unittest/gpopt/minidump/CPullUpProjectElementTest.h"
#include "unittest/gpopt/minidump/CMiniDumperDXLTest.h"
#include "unittest/gpopt/minidump/CMinidumpWithConstExprEvaluatorTest.h"
#include "unittest/gpopt/minidump/CWindowTest.h"
#include "unittest/gpopt/minidump/CICGTest.h"
#include "unittest/gpopt/minidump/CMultilevelPartitionTest.h"
#include "unittest/gpopt/minidump/CTVFTest.h"
#include "unittest/gpopt/minidump/CDMLTest.h"
#include "unittest/gpopt/minidump/CAggTest.h"
#include "unittest/gpopt/minidump/CSubqueryTest.h"
#include "unittest/gpopt/minidump/CCollapseProjectTest.h"
#include "unittest/gpopt/minidump/CPhysicalParallelUnionAllTest.h"
#include "unittest/gpopt/minidump/CPruneColumnsTest.h"
#include "unittest/gpopt/minidump/CMissingStatsTest.h"
#include "unittest/gpopt/minidump/CBitmapTest.h"
#include "unittest/gpopt/minidump/CCTETest.h"
#include "unittest/gpopt/minidump/CExternalTableTest.h"
#include "unittest/gpopt/minidump/CEscapeMechanismTest.h"
#include "unittest/gpopt/minidump/CDirectDispatchTest.h"
#include "unittest/gpopt/minidump/CCastTest.h"
#include "unittest/gpopt/minidump/CConstTblGetTest.h"

#include "unittest/gpopt/operators/CContradictionTest.h"
#include "unittest/gpopt/operators/CExpressionPreprocessorTest.h"
#include "unittest/gpopt/operators/CExpressionTest.h"
#include "unittest/gpopt/operators/CPredicateUtilsTest.h"
#include "unittest/gpopt/operators/CScalarIsDistinctFromTest.h"

#include "unittest/gpopt/search/CSchedulerTest.h"
#include "unittest/gpopt/search/CSearchStrategyTest.h"
#include "unittest/gpopt/minidump/CMultilevelPartitionTest.h"
#include "unittest/gpopt/search/COptimizationJobsTest.h"

#include "unittest/gpopt/translate/CTranslatorDXLToExprTest.h"
#include "unittest/gpopt/translate/CTranslatorExprToDXLTest.h"

#include "unittest/gpopt/csq/CCorrelatedExecutionTest.h"
#include "unittest/gpopt/eval/CConstExprEvaluatorDefaultTest.h"
#include "unittest/gpopt/eval/CConstExprEvaluatorDXLTest.h"
#include "unittest/gpopt/xforms/CDecorrelatorTest.h"
#include "unittest/gpopt/xforms/CJoinOrderTest.h"
#include "unittest/gpopt/xforms/CSubqueryHandlerTest.h"
#include "unittest/gpopt/xforms/CXformTest.h"
#include "unittest/gpopt/xforms/CXformFactoryTest.h"

#include "unittest/dxl/statistics/CStatisticsTest.h"
#include "unittest/dxl/statistics/CFilterCardinalityTest.h"
#include "unittest/dxl/statistics/CPointTest.h"
#include "unittest/dxl/statistics/CBucketTest.h"
#include "unittest/dxl/statistics/CHistogramTest.h"
#include "unittest/dxl/statistics/CMCVTest.h"
#include "unittest/dxl/statistics/CJoinCardinalityTest.h"
#include "unittest/dxl/statistics/CJoinCardinalityNDVBasedEqPredTest.h"
#include "unittest/gpopt/cost/CCostTest.h"
#include "unittest/gpopt/minidump/MinidumpTestHeaders.h" // auto generated header file

using namespace gpos;
using namespace gpopt;
using namespace gpdxl;
using namespace gpnaucrates;
using namespace gpdbcost;

// static array of all known unittest routines
static gpos::CUnittest rgut[] =
{
	GPOS_UNITTEST_STD(CExpressionPreprocessorTest),
};

//---------------------------------------------------------------------------
//	@function:
//		ConfigureTests
//
//	@doc:
//		Configurations needed before running unittests
//
//---------------------------------------------------------------------------
void ConfigureTests()
{
	// initialize DXL support
	InitDXL();

	CMDCache::Init();

	// load metadata objects into provider file
	{
		CAutoMemoryPool amp;
		IMemoryPool *pmp = amp.Pmp();
		CTestUtils::InitProviderFile(pmp);

		// detach safety
		(void) amp.PmpDetach();
	}

#ifdef GPOS_DEBUG
	// reset xforms factory to exercise xforms ctors and dtors
	CXformFactory::Pxff()->Shutdown();
	GPOS_RESULT eres = CXformFactory::EresInit();

	GPOS_ASSERT(GPOS_OK == eres);
#endif // GPOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		Cleanup
//
//	@doc:
//		Cleanup after unittests are done
//
//---------------------------------------------------------------------------
void Cleanup()
{
	CMDCache::Shutdown();
	CTestUtils::DestroyMDProvider();
}

// static variable counting the number of failed tests; PvExec overwrites with
// the actual count of failed tests
static ULONG tests_failed = 0;

//---------------------------------------------------------------------------
//	@function:
//		PvExec
//
//	@doc:
//		Function driving execution.
//
//---------------------------------------------------------------------------
static void *
PvExec
	(
	void *pv
	)
{
	CMainArgs *pma = (CMainArgs*) pv;
	CBitVector bv(ITask::PtskSelf()->Pmp(), CUnittest::UlTests());

	CHAR ch = '\0';

	CHAR *szFileName = NULL;
	BOOL fMinidump = false;
	BOOL fUnittest = false;
	
	while (pma->FGetopt(&ch))
	{
		CHAR *szTestName = NULL;
		
		switch (ch)
		{
			case 'U':
				szTestName = optarg;
				// fallthru
			case 'u':
				CUnittest::FindTest(bv, CUnittest::EttStandard, szTestName);
				fUnittest = true;
				break;

			case 'x':
				CUnittest::FindTest(bv, CUnittest::EttExtended, NULL /*szTestName*/);
				fUnittest = true;
				break;

			case 'T':
				CUnittest::SetTraceFlag(optarg);
				break;
				
			case 'd':
				fMinidump = true;
				szFileName = optarg;
				break;

			default:
				// ignore other parameters
				break;
		}
	}

	if (fMinidump && fUnittest)
	{
		GPOS_TRACE(GPOS_WSZ_LIT("Cannot specify -d and -U/-u options at the same time"));
		return NULL;
	}
	
	if (fMinidump)
	{	
		// initialize DXL support
		InitDXL();

		CMDCache::Init();
		
		CAutoMemoryPool amp;
		IMemoryPool *pmp = amp.Pmp();

		// load dump file
		CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(pmp, szFileName);
		GPOS_CHECK_ABORT;

		COptimizerConfig *poconf = pdxlmd->Poconf();

		if (NULL == poconf)
		{
			poconf = COptimizerConfig::PoconfDefault(pmp);
		}
		else
		{
			poconf -> AddRef();
		}

		ULONG ulSegments = CTestUtils::UlSegments(poconf);

		CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump
								(
								pmp,
								szFileName,
								ulSegments,
								1 /*ulSessionId*/,
								1 /*ulCmdId*/,
								poconf,
								NULL /*pceeval*/
								);

		GPOS_DELETE(pdxlmd);
		poconf->Release();
		pdxlnPlan->Release();
		CMDCache::Shutdown();
	}
	else
	{
		GPOS_ASSERT(fUnittest);
		tests_failed = CUnittest::Driver(&bv);
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		main
//
//	@doc:
//		Entry point for stand-alone optimizer binary; ignore arguments for the
//		time being
//
//---------------------------------------------------------------------------
INT main
	(
	INT iArgs,
	const CHAR **rgszArgs
	)
{	

	// Use default allocator
	struct gpos_init_params gpos_params = { NULL, NULL, NULL };

	gpos_init(&gpos_params);
	gpdxl_init();
	gpopt_init();

	GPOS_ASSERT(iArgs >= 0);

	if (gpos_set_threads(4, 20))
	{
		return GPOS_FAILED;
	}

	// setup args for unittest params
	CMainArgs ma(iArgs, rgszArgs, "uU:d:xT:");
	
	// initialize unittest framework
	CUnittest::Init(rgut, GPOS_ARRAY_SIZE(rgut), ConfigureTests, Cleanup);

	gpos_exec_params params;
	params.func = PvExec;
	params.arg = &ma;
	params.stack_start = &params;
	params.error_buffer = NULL;
	params.error_buffer_size = -1;
	params.abort_requested = NULL;

	if (gpos_exec(&params) || (tests_failed != 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// EOF
