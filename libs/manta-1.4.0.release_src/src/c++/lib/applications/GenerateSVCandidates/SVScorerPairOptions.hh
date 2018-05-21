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
/// \author Chris Saunders and Xiaoyu Chen
///

#pragma once

#include "blt_util/blt_types.hh"


/// shared options related to read pair support:
struct PairOptions
{
    explicit
    PairOptions(const bool isRNA) :
        minFragSupport(50),
        minFragProb(0.0001f),
        RNA(isRNA),
        useProperPairFlag(isRNA)

    {}

    /// we're interested in any fragments which cross center pos with at least N bases of support on each side
    /// (note this definition is certain to overlap the split read definition whenever N is less than the read length
    ///
    /// for reads shorter than this length, the whole read is required...
    const pos_t minFragSupport;

    const float minFragProb;

    /// This is an RNA dataset. Insert size distribution is less meaningful due to splicing
    const bool RNA;

    /// Use the proper pair flag to decide if read pairs are are anomalous for ref and alt scoring
    /// Used for RNA, since splicing makes the pair insert size distribution less informative
    const bool useProperPairFlag;
};
