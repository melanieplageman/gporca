//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		ConvertGetToConstTest.h
//
//	@doc:
//		Test for expression preprocessing
//---------------------------------------------------------------------------
#ifndef GPOPT_ConvertGetToConstTest_H
#define GPOPT_ConvertGetToConstTest_H

#include "gpos/base.h"

namespace gpopt
{
	//---------------------------------------------------------------------------
	//	@class:
	//		ConvertGetToConstTest
	//
	//	@doc:
	//		Unittests
	//
	//---------------------------------------------------------------------------
	class ConvertGetToConstTest
	{


	public:

		// unittests
		static GPOS_RESULT EresUnittest();
		static GPOS_RESULT EresUnittest_ConvertGetToConst();


	}; // class ConvertGetToConstTest
}

#endif // !GPOPT_ConvertGetToConstTest_H

// EOF
