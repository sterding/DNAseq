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

#include "manta/SVScoreInfoDiploid.hh"
#include "blt_util/log.hh"

#include <iostream>



std::ostream&
operator<<(
    std::ostream& os,
    const SVScoreInfoDiploidSample& sids)
{
    os << "DiploidSVScoreInfoSample "
       << " gt=" << DIPLOID_GT::label(sids.gt)
       << " gtScore=" << sids.gtScore
       << " pl=";

    for (unsigned gt(0); gt<DIPLOID_GT::SIZE; ++gt)
    {
        if (gt!=0) os << ',';
        os << sids.phredLoghood[gt];
    }
    os << " pprob=";
    for (unsigned gt(0); gt<DIPLOID_GT::SIZE; ++gt)
    {
        if (gt!=0) os << ',';
        os << sids.pprob[gt];
    }
    os << " sampleFilters:";
    for (const std::string& filter : sids.filters)
    {
        os << " " << filter;
    }
    return os;
}



std::ostream&
operator<<(
    std::ostream& os,
    const SVScoreInfoDiploid& sid)
{
    os << "DiploidSVScoreInfo "
       << " altScore=" << sid.altScore;
    os << " filters:";
    for (const std::string& filter : sid.filters)
    {
        os << " " << filter;
    }
    os << "\n";
    for (const auto& sample : sid.samples)
    {
        os << sample << "\n";
    }
    return os;
}
