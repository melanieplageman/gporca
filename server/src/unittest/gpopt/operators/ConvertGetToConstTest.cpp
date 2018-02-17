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

CExpression *getLogicalProject(IMemoryPool *pmp, CExpression *pexprFirstChild, CMDAccessor *cmdAccessor)
{
	CExpression *pexprScalarConst1 = CUtils::PexprScalarConstInt4(pmp, 5);
	IMDId *pmdidType1 = CScalarConst::PopConvert(pexprScalarConst1->Pop())->PmdidType(); // lifecycle of the guy before it?

	const IMDType *pmdtype1 = cmdAccessor->Pmdtype(pmdidType1); // copy from a cast
	CColumnFactory *pcf = COptCtxt:: PoctxtFromTLS()->Pcf(); // copy from reinterpret cast
	CColRef *pcrComputed1 = pcf->PcrCreate(pmdtype1); // destructor does nothing

	CExpression *pexprPrjElem1 = CUtils::PexprScalarProjectElement(pmp, pcrComputed1, pexprScalarConst1); // releases the paramters passed in
	CExpression *pexprPrjList = GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CScalarProjectList(pmp), pexprPrjElem1); // no utility to make this?
	CExpression *logicalProject = CUtils::PexprLogicalProject(pmp, pexprFirstChild, pexprPrjList, true); // is taking responsiblity for pexprPrjList and pexprlogicalget

	return logicalProject;
}

CColRefSet *PcrsOutput(IMemoryPool *pmp, CExpression *pexprLogical)
{
	CExpressionHandle exprhdl(pmp);
	exprhdl.Attach(pexprLogical);
	return CLogical::PopConvert(pexprLogical->Pop())->PcrsDeriveOutput(pmp, exprhdl); // doesn't take ownership of expr handle or logicalproject
}

CExpression *pexprLogicalUnion(IMemoryPool *pmp, CExpression *pexprLogicalProject, CExpression *pexprLogicalGet)
{
	CColRefSet *pcrsProject = PcrsOutput(pmp, pexprLogicalProject);

	CExpression *logicalProjectFirstChild = (*pexprLogicalProject)[0];
	CColRefSet *pcrsGetUnderProject = NULL;
	switch (logicalProjectFirstChild->Pop()->Eopid())
	{
		case COperator::EopLogicalGet:
			pcrsGetUnderProject = PcrsOutput(pmp, (*pexprLogicalProject)[0]);
			break;
		case COperator::EopLogicalLimit:
			pcrsGetUnderProject = PcrsOutput(pmp, (*logicalProjectFirstChild)[0]);
			break;
		default:
			GPOS_ASSERT(!"First child of Union is not a Limit or a Get");
	}
	// get all of the colrefs that are in the project but are not in the get which is under the project
	// this will give you what was in the project list for this arm of the union (what we are truly projecting)
	pcrsProject->Exclude(pcrsGetUnderProject);

	pcrsGetUnderProject->Release();
	// this will also serve as the output for the logical union since it is the first child

	// the other input to the union will be from the other arm of it
	// in this very contrived test case, we will assume that we are using all of the colrefs in the table
	// otherwise, we would have to determine which we actually want to have as input to the union
	// and I have no idea how to do that (TODO: look this up)

	CColRefSet *pcrsGet = PcrsOutput(pmp, pexprLogicalGet);
	DrgPcr *pdrgpcrGet = GPOS_NEW(pmp) DrgPcr(pmp);
	pdrgpcrGet->Append(pcrsGet->PcrFirst());
	pcrsGet->Release();

	DrgDrgPcr *pdrgpdrgpcrInput = GPOS_NEW(pmp) DrgDrgPcr(pmp);
	// append the input from one side of the union
	pdrgpdrgpcrInput->Append(pcrsProject->Pdrgpcr(pmp));
	// and from the other side of the union
	pdrgpdrgpcrInput->Append(pdrgpcrGet);

	// the output from the logical union is the first child
	DrgPcr *pdrgpcrOutput = pcrsProject->Pdrgpcr(pmp);
	pcrsProject->Release();

	CExpression *pexprLogicalUnion = GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CLogicalUnion(pmp, pdrgpcrOutput, pdrgpdrgpcrInput), pexprLogicalProject, pexprLogicalGet);
	return pexprLogicalUnion;
}

GPOS_RESULT
ConvertGetToConstTest::EresUnittest_ConvertGetToConst()
{
	// how is a dynamic cast different than gpos_new? if it makes a copy, where does it make the copy?
	// TODO: make it actually work with limit injected
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

	// RHS is shared by input and output
	CExpression *pexprLogicalGetRHS = getLogicalGet(pmp);

	// Construct Input
	CExpression *pexprLogicalGetLHS = getLogicalGet(pmp);

	CExpression *pexprLogicalProjectInput = getLogicalProject(pmp, pexprLogicalGetLHS, &mda); // takes ownership of pexprLogicalGetLHS

	CExpression *pexprLogicalUnionInput = pexprLogicalUnion(pmp, pexprLogicalProjectInput, pexprLogicalGetRHS);

	// Construct expected output by remaking logicalproject but with same scalar project list
	CExpression *pexprScalarProjectList = (*pexprLogicalProjectInput)[1];
	// same logical get left hand side but now we want to put the limit on top of it
	pexprLogicalGetLHS->AddRef();
	CExpression *logicalLimit = CUtils::PexprLimit(pmp, pexprLogicalGetLHS, 0, 1);
	pexprScalarProjectList->AddRef();
	CExpression *pexprLogicalProjectOutput = CUtils::PexprLogicalProject(pmp, logicalLimit, pexprScalarProjectList, false);
	pexprLogicalGetRHS->AddRef();
	CExpression *pexprLogicalUnionExpectedOutput = pexprLogicalUnion(pmp, pexprLogicalProjectOutput, pexprLogicalGetRHS);

	CExpression *pexprPreprocessed = CExpressionPreprocessor::PexprPreprocess(pmp, pexprLogicalUnionInput);
	pexprLogicalUnionInput->DbgPrint();
	pexprLogicalUnionExpectedOutput->DbgPrint();
	pexprPreprocessed->DbgPrint();
	pexprLogicalUnionInput->Release();

	GPOS_RTL_ASSERT(pexprLogicalUnionExpectedOutput->FMatch(pexprPreprocessed));
	pexprPreprocessed->Release();

	pexprLogicalUnionExpectedOutput->Release();

	return GPOS_OK;
}
// EOF
