//  Greenplum Database
//  Copyright (C) 2018 Pivotal, Inc.

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "unittest/dxl/CParseHandlerColDescrTest.h"

#include <memory>

#include "gpos/base.h"
#include "gpos/test/CUnittest.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpos/common/CAutoP.h"
#include "gpos/common/CAutoRef.h"
#include "gpos/common/CAutoRg.h"
#include "gpos/io/COstreamString.h"
#include "naucrates/dxl/operators/CDXLOperatorFactory.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"
#include "naucrates/dxl/parser/CParseHandlerTableDescr.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "gpopt/translate/CTranslatorDXLToExpr.h"
#include "gpopt/translate/CTranslatorExprToDXL.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "unittest/gpopt/CTestUtils.h"

XERCES_CPP_NAMESPACE_USE

namespace
{
	class Fixture
	{
		private:
			CAutoMemoryPool m_amp;
			CMDAccessor *m_mda; // do I want an auto-pointer? or do const?
			const CAutoOptCtxt m_aoc;
			gpos::CAutoP<CDXLMemoryManager> m_apmm;
			std::auto_ptr<SAX2XMLReader> m_apxmlreader;
			gpos::CAutoP<CParseHandlerManager> m_apphm;
			gpos::CAutoP<CParseHandlerTableDescr> m_apphTableDescr;
			gpos::CAutoP<CWStringDynamic> m_serializedString;
			gpos::CAutoP<COstreamString> m_os;
			gpos::CAutoP<gpdxl::CXMLSerializer> m_apxmlser;

			static IMDProvider* Pmdp()
			{
				CTestUtils::m_pmdpf->AddRef();
				return CTestUtils::m_pmdpf;
			}
		public:
			Fixture():
					m_mda(GPOS_NEW(m_amp.Pmp()) CMDAccessor(m_amp.Pmp(), CMDCache::Pcache(), CTestUtils::m_sysidDefault, Pmdp())),
					m_aoc(m_amp.Pmp(), m_mda, NULL /* pceeval */, CTestUtils::Pcm(m_amp.Pmp())), // do I need this or can I use the global one
					m_apmm(GPOS_NEW(Pmp()) CDXLMemoryManager(Pmp())),
					m_apxmlreader(XMLReaderFactory::createXMLReader(Pmm())),
					m_apphm(GPOS_NEW(Pmp()) CParseHandlerManager(Pmm(), Pxmlreader())),
					m_apphTableDescr(GPOS_NEW(Pmp()) CParseHandlerTableDescr(Pmp(), Pphm(), NULL)),
					m_serializedString(GPOS_NEW(Pmp()) CWStringDynamic(Pmp())),
					m_os(GPOS_NEW(Pmp()) COstreamString(serializedString())),
					m_apxmlser(GPOS_NEW(Pmp()) CXMLSerializer(Pmp(), *Os(), false))
			{
				m_apphm->ActivateParseHandler(PphTableDescr());
			}

			~Fixture()
			{
				GPOS_DELETE(m_mda);
			}

			IMemoryPool *Pmp() const
			{
				return m_amp.Pmp();
			}

			CMDAccessor *Pmda() const
			{
				return m_mda;
			}

			CDXLMemoryManager *Pmm()
			{
				return m_apmm.Pt();
			}

			SAX2XMLReader *Pxmlreader()
			{
				return m_apxmlreader.get();
			}

			CXMLSerializer *Pxmlser()
			{
				return m_apxmlser.Pt();
			}

			CParseHandlerManager *Pphm()
			{
				return m_apphm.Pt();
			}

			CParseHandlerTableDescr *PphTableDescr()
			{
				return m_apphTableDescr.Pt();
			}

			void Parse(const XMLByte szDXL[], size_t size)
			{
				MemBufInputSource mbis(
						szDXL,
						size,
						"dxl test",
						false,
						Pmm()
				);
				Pxmlreader()->parse(mbis);
			}

			CWStringDynamic* serializedString()
			{
				return m_serializedString.Pt();
			}

			COstreamString* Os()
			{
				return m_os.Pt();
			}

	};
}

static gpos::GPOS_RESULT Eres_OidCollation()
{
	const CHAR szDXLFileName[] = "../data/dxl/parse_tests/TableDescr.xml";
	Fixture f;

	IMemoryPool *pmp = f.Pmp();
	CMDAccessor *mda = f.Pmda();
	gpopt::CColumnFactory *pcf = COptCtxt::PoctxtFromTLS()->Pcf();

	gpos::CAutoRg<CHAR> a_szDXL(CDXLUtils::SzRead(pmp, szDXLFileName));

	CParseHandlerTableDescr *pphtd = f.PphTableDescr();

	f.Parse((const XMLByte *)a_szDXL.Rgt(), strlen(a_szDXL.Rgt()));

	CDXLTableDescr *pdxlTabDescr = pphtd->Pdxltabdesc();

	const CDXLColDescr *pdxlColDescr = pdxlTabDescr->Pdxlcd(0);

	// Check the mdid after parsing
	GPOS_RTL_ASSERT(1 == pdxlColDescr->IAttno());
	// After parsing, the first column descriptor should have a collation oid of 100
	GPOS_RTL_ASSERT(100 == pdxlColDescr->OidCollation());

	// The CDXLTableDescr is the test input
	// Create a table descriptor from the dxltabledescr
	CTableDescriptor *pTabDescr = CTranslatorDXLToExpr::Ptabdesc(pmp, mda, pdxlTabDescr);
	const CColumnDescriptor *pColDescr = pTabDescr->Pcoldesc(0);
	GPOS_RTL_ASSERT(100 == pColDescr->OidCollation());


	// then test creating a dxltabledescr from a tabledescr
	DrgPcr *pfakeDrgPcrOutput = NULL;
	CDXLTableDescr *poutputdxlTabDescr = CTranslatorExprToDXL::Pdxltabdesc(pmp, pcf, pTabDescr, pfakeDrgPcrOutput);
	const CDXLColDescr *poutputdxlColDescr = poutputdxlTabDescr->Pdxlcd(0);

	GPOS_RTL_ASSERT(100 == poutputdxlColDescr->OidCollation());

	pTabDescr->Release();
	poutputdxlTabDescr->Release();
	return gpos::GPOS_OK;
}

namespace gpdxl
{
	gpos::GPOS_RESULT CParseHandlerColDescrTest::EresUnittest()
	{
		CUnittest rgut[] =
				{
						GPOS_UNITTEST_FUNC(Eres_OidCollation)
				};
		return CUnittest::EresExecute(rgut, GPOS_ARRAY_SIZE(rgut));
	}
}

