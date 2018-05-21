//
// Manta - Structural Variant and Indel Caller
// Copyright (c) 2013-2018 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

/// \file
/// \author Chris Saunders
///


#include "common/Exceptions.hh"


#ifdef DEBUG_ALN
#include "blt_util/log.hh"
#include <iostream>
#endif



template <typename ScoreType>
template <typename SymIter>
void
GlobalJumpAligner<ScoreType>::
align(
    const SymIter queryBegin, const SymIter queryEnd,
    const SymIter ref1Begin, const SymIter ref1End,
    const SymIter ref2Begin, const SymIter ref2End,
    JumpAlignmentResult<ScoreType>& result) const
{
    result.clear();

    const AlignmentScores<ScoreType>& scores(this->getScores());

    const size_t querySize(std::distance(queryBegin, queryEnd));
    const size_t ref1Size(std::distance(ref1Begin, ref1End));
    const size_t ref2Size(std::distance(ref2Begin, ref2End));

    if (0 == querySize)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("Unexpected empty query sequence"));
    }
    if (0 == ref1Size)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("Unexpected empty reference1 sequence"));
    }
    if (0 == ref2Size)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("Unexpected empty reference2 sequence"));
    }


    _score1.resize(querySize+1);
    _score2.resize(querySize+1);
    _ptrMat1.resize(querySize+1, ref1Size+1);
    _ptrMat2.resize(querySize+1, ref2Size+1);

    ScoreVec* thisSV(&_score1);
    ScoreVec* prevSV(&_score2);

    static const ScoreType badVal(-10000);

    // global alignment of query
    //
    // disallow start from the insert or delete state
    //
    // query can 'fall-off' the end of a short reference, in which case it will
    // be soft-clipped and each base off the end will be scored as offEdge
    //
    for (unsigned queryIndex(0); queryIndex<=querySize; queryIndex++)
    {
        PtrVal& headPtr1(_ptrMat1.val(queryIndex,0));
        PtrVal& headPtr2(_ptrMat2.val(queryIndex,0));
        ScoreVal& val((*thisSV)[queryIndex]);
        headPtr1.match = AlignState::MATCH;
        headPtr2.match = AlignState::MATCH;
        val.match = queryIndex * scores.offEdge;
        headPtr1.del = AlignState::MATCH;
        headPtr2.del = AlignState::MATCH;
        val.del = badVal;
        headPtr1.ins = AlignState::MATCH;
        headPtr2.ins = AlignState::MATCH;
        val.ins = badVal;
        headPtr1.jump = AlignState::MATCH;
        headPtr2.jump = AlignState::MATCH;
        val.jump = badVal;
    }

#ifdef DEBUG_ALN_MATRIX
    // store full matrix of scores to print out later, don't turn this debug option on for large references!
    std::vector<ScoreVec> storeScores;

    storeScores.push_back(*thisSV);
#endif


    BackTrace<ScoreType> btrace;

    {
        unsigned ref1Index(0);
        for (SymIter ref1Iter(ref1Begin); ref1Iter != ref1End; ++ref1Iter, ++ref1Index)
        {
            std::swap(thisSV,prevSV);

            {
                // disallow start from the insert or delete state:
                PtrVal& headPtr(_ptrMat1.val(0,ref1Index+1));
                ScoreVal& val((*thisSV)[0]);
                headPtr.match = AlignState::MATCH;
                val.match = 0;
                headPtr.del = AlignState::MATCH;
                val.del = badVal;
                headPtr.ins = AlignState::MATCH;
                val.ins = badVal;
                headPtr.jump = AlignState::MATCH;
                val.jump = badVal;
            }

            unsigned queryIndex(0);
            for (SymIter queryIter(queryBegin); queryIter != queryEnd; ++queryIter, ++queryIndex)
            {
                // update match
                ScoreVal& headScore((*thisSV)[queryIndex+1]);
                PtrVal& headPtr(_ptrMat1.val(queryIndex+1,ref1Index+1));
                {
                    const ScoreVal& sval((*prevSV)[queryIndex]);
                    headPtr.match = this->max3(
                                        headScore.match,
                                        sval.match,
                                        sval.del,
                                        sval.ins);

                    headScore.match += ((*queryIter==*ref1Iter) ? scores.match : scores.mismatch);
                }

                // update delete
                {
                    const ScoreVal& sval((*prevSV)[queryIndex+1]);
                    headPtr.del = this->max3(
                                      headScore.del,
                                      sval.match + scores.open,
                                      sval.del,
                                      sval.ins);

                    headScore.del += scores.extend;
                    if (0==queryIndex) headScore.del = badVal;
                }

                // update insert
                {
                    const ScoreVal& sval((*thisSV)[queryIndex]);
                    headPtr.ins = this->max3(
                                      headScore.ins,
                                      sval.match + scores.open,
                                      badVal,
                                      sval.ins);

                    headScore.ins += scores.extend;
                    if (0==queryIndex) headScore.ins = badVal;
                }

                // update jump
                {
                    const ScoreVal& sval((*prevSV)[queryIndex+1]);
                    headPtr.jump = this->max4(
                                       headScore.jump,
                                       headScore.match + this->getJumpScore(),
                                       badVal,
                                       headScore.ins + this->getJumpScore(),
                                       sval.jump);
                }

#ifdef DEBUG_ALN
                log_os << "queryIdx refIdx ref1Idx: " << queryIndex+1 << " " << ref1Index+1 << " " << ref1Index+1 << "\n";
                log_os << "MIDJ: " << headScore.match << ":" << headScore.ins << ":" << headScore.del << ":" << headScore.jump << "/"
                       << static_cast<int>(headPtr.match) << static_cast<int>(headPtr.ins) << static_cast<int>(headPtr.del) << static_cast<int>(headPtr.jump) << "\n";
                log_os << "QuerySymbol:" << *queryIter << " RefSymbol:" << *ref1Iter << "\n";
#endif
            }
#ifdef DEBUG_ALN
            log_os << "\n";
#endif

#ifdef DEBUG_ALN_MATRIX
            storeScores.push_back(*thisSV);
#endif

            // get backtrace info:
            {
                const ScoreVal& sval((*thisSV)[querySize]);
                updateBacktrace(sval.match, ref1Index+1, querySize, btrace);
            }
        }
    }

    // in the backtrace start search, also allow for the case where the query falls-off the end of the reference:
    for (unsigned queryIndex(0); queryIndex<querySize; queryIndex++)
    {
        const ScoreVal& sval((*thisSV)[queryIndex]);
        const ScoreType thisMax(sval.match + (querySize-queryIndex) * scores.offEdge);
        updateBacktrace(thisMax, ref1Size, queryIndex, btrace);
    }


    {
        for (unsigned queryIndex(0); queryIndex<=querySize; queryIndex++)
        {
            ScoreVal& val((*thisSV)[queryIndex]);
            val.match = queryIndex * scores.offEdge;
            val.del = badVal;
            val.ins = badVal;
            //val.jump = badVal; // preserve jump setting from last iteration of ref1
        }

#ifdef DEBUG_ALN_MATRIX
        storeScores.push_back(*thisSV);
#endif

        unsigned ref2Index(0);
        for (SymIter ref2Iter(ref2Begin); ref2Iter != ref2End; ++ref2Iter, ++ref2Index)
        {
            std::swap(thisSV,prevSV);

            {
                // disallow start from the insert or delete state:
                PtrVal& headPtr(_ptrMat2.val(0,ref2Index+1));
                ScoreVal& val((*thisSV)[0]);
                headPtr.match = AlignState::MATCH;
                val.match = 0;
                headPtr.del = AlignState::MATCH;
                val.del = badVal;
                headPtr.ins = AlignState::MATCH;
                val.ins = badVal;
                headPtr.jump = AlignState::MATCH;
                val.jump = badVal;
            }

            unsigned queryIndex(0);
            for (SymIter queryIter(queryBegin); queryIter != queryEnd; ++queryIter, ++queryIndex)
            {
                // update match
                ScoreVal& headScore((*thisSV)[queryIndex+1]);
                PtrVal& headPtr(_ptrMat2.val(queryIndex+1,ref2Index+1));
                {
                    const ScoreVal& sval((*prevSV)[queryIndex]);
                    headPtr.match = this->max4(
                                        headScore.match,
                                        sval.match,
                                        sval.del,
                                        sval.ins,
                                        sval.jump);

                    headScore.match += ((*queryIter==*ref2Iter) ? scores.match : scores.mismatch);
                }

                // update delete
                {
                    const ScoreVal& sval((*prevSV)[queryIndex+1]);
                    headPtr.del = this->max3(
                                      headScore.del,
                                      sval.match + scores.open,
                                      sval.del,
                                      sval.ins);

                    headScore.del += scores.extend;
                }

                // update insert
                {
                    const ScoreVal& sval((*thisSV)[queryIndex]);
                    headPtr.ins = this->max4(
                                      headScore.ins,
                                      sval.match + scores.open,
                                      badVal,
                                      sval.ins,
                                      sval.jump); // jump->ins moves get a pass on the gap-open penalty, to support breakend insertions

                    headScore.ins += scores.extend;
                }

                // update jump
                {
                    const ScoreVal& sval((*prevSV)[queryIndex+1]);
                    headPtr.jump = AlignState::JUMP;
                    headScore.jump = sval.jump;
                }

#ifdef DEBUG_ALN
                log_os << "queryIdx refIdx ref2Idx: " << queryIndex+1 << " " << ref1Size+ref2Index+1 << " " << ref2Index+1 << "\n";
                log_os << "MIDJ: " << headScore.match << ":" << headScore.ins << ":" << headScore.del << ":" << headScore.jump << "/"
                       << static_cast<int>(headPtr.match) << static_cast<int>(headPtr.ins) << static_cast<int>(headPtr.del) << static_cast<int>(headPtr.jump) << "\n";
                log_os << "QuerySymbol:" << *queryIter << " RefSymbol:" << *ref2Iter << "\n";
#endif
            }
#ifdef DEBUG_ALN
            log_os << "\n";
#endif

#ifdef DEBUG_ALN_MATRIX
            storeScores.push_back(*thisSV);
#endif

            // get backtrace start info:
            {
                const ScoreVal& sval((*thisSV)[querySize]);
                updateBacktrace(sval.match, ref1Size+ref2Index+1, querySize, btrace);
            }
        }

    }

    // in the backtrace start search, also allow for the case where the query falls-off the end of the reference:
    for (unsigned queryIndex(0); queryIndex<querySize; queryIndex++)
    {
        const ScoreVal& sval((*thisSV)[queryIndex]);
        const ScoreType thisMax(sval.match + (querySize-queryIndex) * scores.offEdge);
        updateBacktrace(thisMax, ref1Size+ref2Size, queryIndex, btrace);
    }

#ifdef DEBUG_ALN_MATRIX
    std::vector<AlignState::index_t> dumpStates {AlignState::MATCH, AlignState::DELETE, AlignState::INSERT, AlignState::JUMP};
    this->dumpTables(queryBegin, queryEnd,
                     ref1Begin, ref1End,
                     ref2Begin, ref2End,
                     querySize,
                     _ptrMat1, _ptrMat2,
                     dumpStates,storeScores);
#endif

    this->backTraceAlignment(
        queryBegin, queryEnd,
        ref1Begin, ref1End,
        ref2Begin, ref2End,
        querySize, ref1Size, ref2Size,
        _ptrMat1, _ptrMat2, btrace, result);
}

