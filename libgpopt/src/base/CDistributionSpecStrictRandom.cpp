//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "gpopt/base/CDistributionSpecStrictRandom.h"

using namespace gpopt;

CDistributionSpecStrictRandom::CDistributionSpecStrictRandom()
{
}

BOOL CDistributionSpecStrictRandom::FMatch(const CDistributionSpec *pds) const
{
    return pds->Edt() == Edt();
}

BOOL CDistributionSpecStrictRandom::FSatisfies(const CDistributionSpec *pds) const
{
    return FMatch(pds);
}
