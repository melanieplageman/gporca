//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJoinStatistics.cpp
//
//	@doc:
//		Statistics for joins
//---------------------------------------------------------------------------

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CJoinStatistics.h"
#include "naucrates/statistics/IStatistics.h"
#include "naucrates/statistics/CHistogramUtils.h"
#include "naucrates/statistics/CScaleFactorUtils.h"

#include "gpos/common/CBitSet.h"
#include "gpos/sync/CAutoMutex.h"
#include "gpos/memory/CAutoMemoryPool.h"

#include "gpopt/base/CColumnFactory.h"
#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CColRefSetIter.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CReqdPropRelational.h"
#include "gpopt/mdcache/CMDAccessor.h"


#include "gpopt/engine/CStatisticsConfig.h"
#include "gpopt/optimizer/COptimizerConfig.h"

using namespace gpmd;
using namespace gpdxl;
using namespace gpopt;



// EOF
