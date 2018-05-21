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

#include "EdgeRuntimeTracker.hh"

#include "common/Exceptions.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>



EdgeRuntimeTracker::
EdgeRuntimeTracker(
    const std::string& outputFile) :
    _osPtr(nullptr),
    _candidateCount(0),
    _complexCandidateCount(0),
    _assembledCandidateCount(0),
    _assembledComplexCandidateCount(0)
{
    if (outputFile.empty()) return;
    _osPtr = new std::ofstream(outputFile.c_str());
    if (! *_osPtr)
    {
        std::ostringstream oss;
        oss << "Can't open output file: '" << outputFile << "'";
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException(oss.str()));
    }

    *_osPtr << std::setprecision(4);
}



EdgeRuntimeTracker::
~EdgeRuntimeTracker()
{
    if (nullptr != _osPtr) delete _osPtr;
}



void
EdgeRuntimeTracker::
stop(const EdgeInfo& edge)
{
    edgeTime.stop();
    const double lastTime(edgeTime.getWallSeconds());

    /// the purpose of the log is to identify the most troublesome cases only, so cutoff the output at a minimum time:
    static const double minLogTime(0.5);
    if (lastTime >= minLogTime)
    {
        if (nullptr != _osPtr)
        {
            edge.write(*_osPtr);
            *_osPtr << '\t' << lastTime
                    << '\t' << _candidateCount
                    << '\t' << _complexCandidateCount
                    << '\t' << _assembledCandidateCount
                    << '\t' << _assembledComplexCandidateCount
                    << '\t' << candidacyTime.getWallSeconds()
                    << '\t' << assemblyTime.getWallSeconds()
                    << '\t' << remoteReadRetrievalTime.getWallSeconds()
                    << '\t' << scoreTime.getWallSeconds()
                    << '\n';
        }
    }
}
