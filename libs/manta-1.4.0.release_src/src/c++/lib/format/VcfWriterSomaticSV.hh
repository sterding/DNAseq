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

#include "manta/SVModelScoreInfo.hh"
#include "format/VcfWriterSV.hh"
#include "format/VcfWriterScoredSV.hh"
#include "options/CallOptionsSomatic.hh"


struct VcfWriterSomaticSV : public VcfWriterSV, VcfWriterScoredSV
{
    VcfWriterSomaticSV(
        const CallOptionsSomatic& somaticOpt,
        const bool isMaxDepthFilter,
        const std::string& referenceFilename,
        const bam_header_info& bamHeaderInfo,
        std::ostream& os,
        const bool& isOutputContig) :
        VcfWriterSV(referenceFilename, bamHeaderInfo, os, isOutputContig),
        _somaticOpt(somaticOpt),
        _isMaxDepthFilter(isMaxDepthFilter),
        _somaticInfoPtr(nullptr),
        _singleJunctionSomaticInfoPtr(nullptr)
    {}

    void
    writeSV(
        const SVCandidateSetData& svData,
        const SVCandidateAssemblyData& adata,
        const SVCandidate& sv,
        const SVId& svId,
        const SVScoreInfo& baseInfo,
        const SVScoreInfoSomatic& somaticInfo,
        const EventInfo& event,
        const SVScoreInfoSomatic& singleJunctionSomaticInfo);

private:

    void
    addHeaderInfo() const override;

    void
    addHeaderFormat() const override;

    void
    addHeaderFilters() const override;

    void
    modifyInfo(
        const EventInfo& event,
        std::vector<std::string>& infotags) const override;

    void
    modifyTranslocInfo(
        const SVCandidate& sv,
        const bool isFirstOfPair,
        const SVCandidateAssemblyData& assemblyData,
        std::vector<std::string>& infotags) const override;

    void
    modifySample(
        const SVCandidate& sv,
        SampleTag_t& sampletags) const override;

    void
    writeFilter() const override;

    const SVScoreInfoSomatic&
    getSomaticInfo() const
    {
        assert(nullptr != _somaticInfoPtr);
        return *_somaticInfoPtr;
    }

    const SVScoreInfoSomatic&
    getSingleJunctionSomaticInfo() const
    {
        assert(nullptr != _singleJunctionSomaticInfoPtr);
        return *_singleJunctionSomaticInfoPtr;
    }


    const CallOptionsSomatic& _somaticOpt;
    const bool _isMaxDepthFilter;
    const SVScoreInfoSomatic* _somaticInfoPtr;
    const SVScoreInfoSomatic* _singleJunctionSomaticInfoPtr;
};

