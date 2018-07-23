//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CRewindabilitySpec.h
//
//	@doc:
//		Rewindability of intermediate query results;
//		Can be used as required or derived property;
//---------------------------------------------------------------------------
#ifndef GPOPT_CRewindabilitySpec_H
#define GPOPT_CRewindabilitySpec_H

#include "gpos/base.h"
#include "gpopt/base/CPropSpec.h"
#include "gpos/common/CRefCount.h"


namespace gpopt
{
	using namespace gpos;


	//---------------------------------------------------------------------------
	//	@class:
	//		CRewindabilitySpec
	//
	//	@doc:
	//		Rewindability specification
	//
	//---------------------------------------------------------------------------
	class CRewindabilitySpec : public CPropSpec
	{

		public:

			enum ERewindabilityType
            {
                // require: I require you to be rewindable, also your sibling has a motion hazard
				// derive: I am rewindable and I impose a motion hazard (streaming spool with a motion in its child will derive this)
                ErtRewindableMotion,

				// require: I require you to be rewindable, don't worry about handling motion hazard
				// derive: I am rewindable and I do not impose motion hazard (any rwindable operator without a motion in it or blocking spool with a motion in it, will derive this)
                ErtRewindableNoMotion,

				// require: I do not require you to be rewindable (this is never request)
				// derive: I am not rewindable, also I impose motion hazard (motions will derive this)
                ErtNotRewindableMotion,

				// require: I do not require you to be rewindable.
				// derive: I am not rewindable and I do not impose any motion hazard
                ErtNotRewindableNoMotion,

				ErtMarkRestore, // currently unused

                ErtSentinel
            };

		private:

			// rewindability support
			ERewindabilityType m_ert;

		public:

			// ctor
			explicit
			CRewindabilitySpec(ERewindabilityType ert);

			// dtor
			virtual
			~CRewindabilitySpec();

			// accessor of rewindablility type
			ERewindabilityType Ert() const
			{
				return m_ert;
			}

			// check if rewindability specs match
 			BOOL FMatch(const CRewindabilitySpec *prs) const;

			// check if rewindability spec satisfies a req'd rewindability spec
			BOOL FSatisfies(const CRewindabilitySpec *prs) const;

			// append enforcers to dynamic array for the given plan properties
			virtual
			void AppendEnforcers(IMemoryPool *pmp, CExpressionHandle &exprhdl, CReqdPropPlan *prpp, DrgPexpr *pdrgpexpr, CExpression *pexpr);

			// hash function
			virtual
			ULONG UlHash() const;

			// extract columns used by the rewindability spec
			virtual
			CColRefSet *PcrsUsed
				(
				IMemoryPool *pmp
				)
				const
			{
				// return an empty set
				return GPOS_NEW(pmp) CColRefSet(pmp);
			}

			// property type
			virtual
			EPropSpecType Epst() const
			{
				return EpstRewindability;
			}

			// print
			IOstream &OsPrint(IOstream &os) const;

	}; // class CRewindabilitySpec

}

#endif // !GPOPT_CRewindabilitySpec_H

// EOF
