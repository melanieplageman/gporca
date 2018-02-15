//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		ConvertGetToConstTest.cpp
//
//	@doc:
//		Test for expression preprocessing
//---------------------------------------------------------------------------
#include <string.h>

#include "gpos/io/COstreamString.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/task/CAutoTraceFlag.h"
#include "gpos/common/CAutoRef.h"

#include "gpopt/base/CUtils.h"
#include "gpopt/eval/CConstExprEvaluatorDefault.h"
#include "gpopt/mdcache/CAutoMDAccessor.h"
#include "gpopt/operators/CPredicateUtils.h"
#include "gpopt/operators/CScalarProjectElement.h"
#include "gpopt/operators/CLogicalNAryJoin.h"
#include "gpopt/operators/CLogicalInnerJoin.h"
#include "gpopt/operators/CLogicalLeftOuterJoin.h"
#include "gpopt/operators/CExpressionUtils.h"
#include "gpopt/optimizer/COptimizerConfig.h"
#include "gpopt/xforms/CXformUtils.h"

#include "unittest/base.h"
#include "unittest/gpopt/operators/ConvertGetToConstTest.h"
#include "unittest/gpopt/CTestUtils.h"

#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/CMDIdGPDB.h"

using namespace gpopt;
using namespace gpmd;
//---------------------------------------------------------------------------
//	@function:
//		ConvertGetToConstTest::EresUnittest
//
//	@doc:
//		Unittest for convertgettoconst pre-processing step
//
//---------------------------------------------------------------------------
GPOS_RESULT
ConvertGetToConstTest::EresUnittest()
{

	CUnittest rgut[] =
			{

					GPOS_UNITTEST_FUNC(EresUnittest_ConvertGetToConst),
			};

	return CUnittest::EresExecute(rgut, GPOS_ARRAY_SIZE(rgut));
}
CExpression *getLogicalGet(IMemoryPool *pmp)
{
	// make a table
	CWStringConst strName(GPOS_WSZ_LIT("foo"));
	CMDIdGPDB *pmdid = GPOS_NEW(pmp) CMDIdGPDB(GPOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = CTestUtils::PtabdescCreate(pmp, 2, pmdid, CName(&strName));
	CWStringConst strAlias = CWStringConst(GPOS_WSZ_LIT("fooAlias"));
	CExpression *logicalGet = CTestUtils::PexprLogicalGet(pmp, ptabdesc, &strAlias);
	return logicalGet;
}

CExpression *getLogicalProject(IMemoryPool *pmp, CExpression *pexprFirstChild, CMDAccessor *cmdAccessor, BOOL isConst)
{
	CExpression *pexprScalarConst1 = CUtils::PexprScalarConstInt4(pmp, 5);
	IMDId *pmdidType1 = CScalarConst::PopConvert(pexprScalarConst1->Pop())->PmdidType(); // lifecycle of the guy before it?

	const IMDType *pmdtype1 = cmdAccessor->Pmdtype(pmdidType1); // copy from a cast
	CColumnFactory *pcf = COptCtxt:: PoctxtFromTLS()->Pcf(); // copy from reinterpret cast
	CColRef *pcrComputed1 = pcf->PcrCreate(pmdtype1); // destructor does nothing

	if (isConst)
	{
		// if logical project has a logicalconsttableget instead of a logical get
		DrgPcr *pdrgpcr = GPOS_NEW(pmp) DrgPcr(pmp);
		DrgPdrgPdatum *pdrgpdrgpdatum = GPOS_NEW(pmp) DrgPdrgPdatum(pmp);
		CLogicalConstTableGet *popCTG = GPOS_NEW(pmp) CLogicalConstTableGet(pmp, pdrgpcr, pdrgpdrgpdatum); // dtor safe releases inputs; ctor doesn't new
		// do we gpos_new when the ctor doesn't?
	}

	CExpression *pexprPrjElem1 = CUtils::PexprScalarProjectElement(pmp, pcrComputed1, pexprScalarConst1); // releases the paramters passed in
	CExpression *pexprPrjList = GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CScalarProjectList(pmp), pexprPrjElem1); // no utility to make this?
	CExpression *logicalProject = CUtils::PexprLogicalProject(pmp, pexprFirstChild, pexprPrjList, true); // is taking responsiblity for pexprPrjList and pexprlogicalget
//	popCTG->Release();

	return logicalProject;
}

CColRefSet *PcrsOutput(IMemoryPool *pmp, CExpression *pexprLogical)
{
	CExpressionHandle exprhdl(pmp);
	exprhdl.Attach(pexprLogical);
	return CLogical::PopConvert(pexprLogical->Pop())->PcrsDeriveOutput(pmp, exprhdl); // doesn't take ownership of expr handle or logicalproject
}

CExpression *pexprLogicalUnion(IMemoryPool *pmp, CMDAccessor *mda, BOOL withLimit)
{
	// Construct Input
	CExpression *logicalGetLHS = getLogicalGet(pmp);
	CExpression *logicalGetRHS = getLogicalGet(pmp);
	CExpression *logicalProject1;
	if (withLimit)
	{
		CExpression *logicalLimit = CUtils::PexprLimit(pmp, logicalGetLHS, 0, 1);
		logicalProject1 = getLogicalProject(pmp, logicalLimit, mda, false);
	}
	else
		logicalProject1 = getLogicalProject(pmp, logicalGetLHS, mda, false);

	CColRefSet *pcrsLhs = PcrsOutput(pmp, logicalProject1);
	CColRefSet *pcrsLogicalGet = PcrsOutput(pmp, logicalGetLHS);
	pcrsLhs->Exclude(pcrsLogicalGet);
	pcrsLogicalGet->Release();
	// makes a drgpcr new but doesn't take ownership of inputcols
	DrgDrgPcr *pdrgpdrgpcrInput = GPOS_NEW(pmp) DrgDrgPcr(pmp);
	pdrgpdrgpcrInput->Append(pcrsLhs->Pdrgpcr(pmp)); // should probably take over the pdrgpcrInput1 since it is appending it to an array
	CColRefSet *pcrsRhs = PcrsOutput(pmp, logicalGetRHS);

	DrgPcr *pdrgpcrRhs = GPOS_NEW(pmp) DrgPcr(pmp);
	pdrgpcrRhs->Append(pcrsRhs->PcrFirst());
	pcrsRhs->Release();

	pdrgpdrgpcrInput->Append(pdrgpcrRhs);
	logicalProject1->AddRef();
	CExpression *pexprLogicalUnionInput = GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CLogicalUnion(pmp, pcrsLhs->Pdrgpcr(pmp), pdrgpdrgpcrInput), logicalProject1, logicalGetRHS);
	pcrsLhs->Release();
	logicalProject1->Release();
	return pexprLogicalUnionInput;
}

GPOS_RESULT
ConvertGetToConstTest::EresUnittest_ConvertGetToConst()
{
	// how is a dynamic cast different than gpos_new? if it makes a copy, where does it make the copy?
	// TODO: make it actually work with limit injected
	// TODO: move tests into own files
	// TODO: refactor into fixture
	// TODO: figure out logic of when we use helper functions
	CAutoMemoryPool amp;
	IMemoryPool *pmp = amp.Pmp();
	// reset metadata cache
	CMDCache::Reset();
	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(pmp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);
	CAutoOptCtxt aoc(pmp, &mda, NULL, CTestUtils::Pcm(pmp));
	//construct input
	CExpression *pexprLogicalUnionInput = pexprLogicalUnion(pmp, &mda, false);
	// construct expected output
	CExpression *pexprLogicalUnionExpectedOutput = pexprLogicalUnion(pmp, &mda, true);
	CExpression *pexprPreprocessed = CExpressionPreprocessor::PexprPreprocess(pmp,pexprLogicalUnionInput);
	pexprLogicalUnionInput->DbgPrint();
	pexprPreprocessed->DbgPrint();
	GPOS_RTL_ASSERT(pexprLogicalUnionExpectedOutput->FMatch(pexprPreprocessed));

	pexprLogicalUnionInput->Release();
	pexprLogicalUnionExpectedOutput->Release();
	pexprPreprocessed->Release();

	return GPOS_OK;
}
// EOF
