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

#pragma once


/// enumerate evidence type estimated on input for each sample
struct SVLocusEvidenceCount
{
    void
    clear()
    {
        total = 0;
        ignored = 0;
        anom = 0;
        split = 0;
        anomAndSplit = 0;
        indel = 0;
        assm = 0;
        remoteRecoveryCandidates = 0;
        splitSupplementarySegment = 0;
    }

    void
    merge(
        const SVLocusEvidenceCount& rhs)
    {
        total += rhs.total;
        ignored += rhs.ignored;
        anom += rhs.anom;
        split += rhs.split;
        anomAndSplit += rhs.anomAndSplit;
        indel += rhs.indel;
        assm += rhs.assm;
        remoteRecoveryCandidates += rhs.remoteRecoveryCandidates;
        splitSupplementarySegment += rhs.splitSupplementarySegment;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned /* version */)
    {
        ar& total& ignored& anom& split& anomAndSplit& indel& assm& remoteRecoveryCandidates& splitSupplementarySegment;
    }

    // using doubles for integral counts here because:
    // (1) counts are potentially very high
    // (2) exact counts don't matter

    ///< total number of non-filtered non-supplement/secondary reads scanned
    double total = 0;

    ///< total number of non-filtered non-supplement/secondary reads ignored for SV purposes
    double ignored = 0;

    ///< total number of non-filtered non-supplement/secondary anomalous reads scanned
    double anom = 0;

    ///< total number of non-filtered non-supplement/secondary split (SA-tag) reads scanned
    double split = 0;

    ///< total number of non-filtered non-supplement/secondary reads scanned which are categorized as BOTH anomolous and split
    double anomAndSplit = 0;

    ///< total number of non-filtered non-supplement/secondary CIGAR large indel reads scanned
    double indel = 0;

    ///< total number of non-filtered non-supplement/secondary semi-aligned reads scanned
    double assm = 0;

    ///< subset of anom. these are reads which qualify as candidates for remote recovery
    double remoteRecoveryCandidates = 0;

    ///< total number of non-filtered supplement/secondary split read segments
    double splitSupplementarySegment = 0;
};
