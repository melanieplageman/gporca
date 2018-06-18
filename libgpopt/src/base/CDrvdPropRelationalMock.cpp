#include "gpopt/base/CDrvdPropRelationalMock.h"
#include "gpopt/operators/CLogical.h"

#include "naucrates/md/CMDProviderMemory.h"
#include "gpopt/base/CAutoOptCtxt.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpopt/mdcache/CMDCache.h"
#include "gpopt/mdcache/CMDAccessor.h"


#include "naucrates/md/IMDTypeInt4.h"
#include "gpopt/base/CColumnFactory.h"

#include "gpopt/base/CColRefSet.h"

#include "unittest/gpopt/CTestUtils.h"

using namespace gpopt;

using namespace gpos;

class RelationalDerivedPropFixture
{
	private:
		const CAutoMemoryPool m_amp;
		CMDAccessor m_mda;
		const CAutoOptCtxt m_aoc;

		static IMDProvider* Pmdp()
		{
			CTestUtils::m_pmdpf->AddRef();
			return CTestUtils::m_pmdpf;
		}

	public:
		RelationalDerivedPropFixture():
				m_amp(),
				m_mda(m_amp.Pmp(), CMDCache::Pcache(), CTestUtils::m_sysidDefault, Pmdp()),
				m_aoc(m_amp.Pmp(), &m_mda, NULL /* pceeval */, CTestUtils::Pcm(m_amp.Pmp()))
		{
		}

		~RelationalDerivedPropFixture()
		{

		}

		IMemoryPool* Pmp() const
		{
			return m_amp.Pmp();
		}

		const IMDTypeInt4 *GetInt4()
		{
			return m_mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
		}
};

//		ctor
CDrvdPropRelationalMock::CDrvdPropRelationalMock
	()
	:
CDrvdPropRelational()
{}

void
CDrvdPropRelationalMock::Derive
		(
				IMemoryPool *,
				CExpressionHandle &,
				CDrvdPropCtxt * // pdpctxt
		)
{
	GPOS_CHECK_ABORT;
}

// output columns
CColRefSet *
CDrvdPropRelationalMock::PcrsOuter(IMemoryPool *pmp) const
{
//	CAutoMemoryPool amp;
//	IMemoryPool *pmp = amp.Pmp();

	CMDAccessor *pmda = COptCtxt::PoctxtFromTLS()->Pmda();

	const IMDTypeInt4 *pmdtypeint4 = pmda->PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
	CWStringConst jstrName(GPOS_WSZ_LIT("j"));
	CWStringConst bstrName(GPOS_WSZ_LIT("b"));
	const CName jname(&jstrName);
	const CName bname(&bstrName);

	CColumnFactory *colFactory = COptCtxt::PoctxtFromTLS()->Pcf();
	CColRef *jCol  = colFactory->PcrCreate(pmdtypeint4, IDefaultTypeModifier, jname);
	CColRef *bCol = colFactory->PcrCreate(pmdtypeint4, IDefaultTypeModifier, bname);
	DrgPcr *outerRefs = GPOS_NEW(pmp) DrgPcr(pmp);
	outerRefs->Append(jCol);
	outerRefs->Append(bCol);
	CColRefSet *colref_set = GPOS_NEW(pmp) CColRefSet(pmp, outerRefs, 2);
	return colref_set;
}

//	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
//	pmdp->AddRef();
//	CMDAccessor mda(pmp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);
//
//	CAutoOptCtxt aoc(pmp, &mda, NULL,  /* pceeval */ CTestUtils::Pcm(pmp));
//	const IMDTypeInt4 *pmdtypeint4  = mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

// EOF
