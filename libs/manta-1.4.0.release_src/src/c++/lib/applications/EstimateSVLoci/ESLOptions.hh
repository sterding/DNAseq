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

#include "common/Program.hh"
#include "manta/SVLocusScanner.hh"
#include "options/AlignmentFileOptions.hh"
#include "options/ReadScannerOptions.hh"
#include "options/SVLocusSetOptions.hh"

#include <vector>


struct ESLOptions
{
    ESLOptions() :
        graphOpt(SVObservationWeights::observation)  // initialize noise edge filtration parameters
    {}

    AlignmentFileOptions alignFileOpt;
    ReadScannerOptions scanOpt;
    SVLocusSetOptions graphOpt;

    std::string referenceFilename;
    std::string outputFilename;
    std::vector<std::string> regions;
    std::string statsFilename;
    std::string chromDepthFilename;

    /// TODO remove the need for this bool by having a single overlap pair handler
    bool isRNA = false;
};


void
parseESLOptions(const illumina::Program& prog,
                int argc, char* argv[],
                ESLOptions& opt);
