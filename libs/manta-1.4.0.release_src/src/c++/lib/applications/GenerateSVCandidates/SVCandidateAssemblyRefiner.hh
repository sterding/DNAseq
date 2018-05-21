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

#pragma once

#include "GSCOptions.hh"
#include "EdgeRuntimeTracker.hh"

#include "alignment/GlobalAligner.hh"
#include "alignment/GlobalLargeIndelAligner.hh"
#include "alignment/GlobalJumpAligner.hh"
#include "alignment/GlobalJumpIntronAligner.hh"
#include "htsapi/bam_header_info.hh"
#include "manta/SVCandidateAssemblyData.hh"
#include "manta/SVCandidate.hh"
#include "manta/SVCandidateAssembler.hh"
#include "options/SmallAssemblerOptions.hh"
#include "svgraph/GenomeIntervalTracker.hh"


/// \brief Provides additional details on low-resolution SVCandidates via assembly and contig alignment
///
/// Via the getCandidateAssemblyData method, provide a base-pair level 'refinement' of low-resolution SV
/// candidates by assembling reads in the suspected breakpoint regions and aligning the assembly back to
/// the reference.
///
struct SVCandidateAssemblyRefiner
{
    SVCandidateAssemblyRefiner(
        const GSCOptions& opt,
        const bam_header_info& header,
        const AllSampleReadCounts& counts,
        EdgeRuntimeTracker& edgeTracker);

    /// \brief Given a low-resolution SV candidate, compute a possible base-level refinement via assembly
    ///
    /// \param[in] isRNA if true add intron logic to the contig jump aligner
    /// \param[in] isFindLargeInsertions if true search for insertions which can't be completely assembled, and conduct more expensive search for assembly insertion evidence
    /// \param[out] assemblyData All candidate refinement results are returned in this structure.
    void
    getCandidateAssemblyData(
        const SVCandidate& sv,
        const bool isFindLargeInsertions,
        SVCandidateAssemblyData& assemblyData) const;

    void
    clearEdgeData()
    {
        _spanToComplexAssmRegions.clear();
    }

private:

    /// Assembler for large SV candidates
    ///
    /// This assumes an SV candidate with two breakend regions and breakpoint direcdtion associated with each region
    void
    getJumpAssembly(
        const SVCandidate& sv,
        const bool isFindLargeInsertions,
        SVCandidateAssemblyData& assemblyData) const;

    /// Assembler for small SV/indel candidates
    ///
    /// This assumes an SV candidate is a single suspected SV/indel region, a so-called 'complex' SV candidate
    ///
    /// \param[in] isFindLargeInsertions if true search for insertions which can't be completely assembled, and conduct more expensive search for assembly insertion evidence
    ///
    void
    getSmallSVAssembly(
        const SVCandidate& sv,
        const bool isFindLargeInsertions,
        SVCandidateAssemblyData& assemblyData) const;

    //////////////////////////////// data:
    const GSCOptions& _opt;
    const bam_header_info& _header;

    /// Assembler parameterized for 'complex' SV candidate assembly from a single candidate region
    const SVCandidateAssembler _smallSVAssembler;

    /// Assembler parameterized for assembly of a 'spanning' SV candidate spanning two regions
    const SVCandidateAssembler _spanningAssembler;

    /// Aligner optimized to discover a small indel from a single SV candidate region
    const GlobalAligner<int> _smallSVAligner;

    /// Aligner optimized to discover a single large indel from a single SV candidate region
    const GlobalLargeIndelAligner<int> _largeSVAligner;

    const GlobalAligner<int> _largeInsertEdgeAligner;
    const GlobalAligner<int> _largeInsertCompleteAligner;
    const GlobalJumpAligner<int> _spanningAligner;
    const GlobalJumpIntronAligner<int> _RNASpanningAligner;

    const AlignmentScores<int> _contigFilterAlignmentScores;

    /// Keeps track of all regions which have already been assembled while processing spanning SVs
    mutable GenomeIntervalTracker _spanToComplexAssmRegions;
};
