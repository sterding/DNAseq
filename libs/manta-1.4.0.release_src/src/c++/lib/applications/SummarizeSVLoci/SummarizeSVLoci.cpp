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

#include "SummarizeSVLoci.hh"
#include "SSLOptions.hh"

#include "common/OutStream.hh"
#include "svgraph/SVLocusSet.hh"

#include <iostream>



static
void
runSSL(const SSLOptions& opt)
{
    const SVLocusSet set(opt.graphFilename.c_str());

    OutStream outs(opt.outputFilename);
    std::ostream& os(outs.getStream());

    if (opt.isGlobalStats)
    {
        set.dumpStats(os);
    }
    else
    {
        set.dumpLocusStats(os);
    }
}



void
SummarizeSVLoci::
runInternal(int argc, char* argv[]) const
{
    SSLOptions opt;

    parseSSLOptions(*this,argc,argv,opt);
    runSSL(opt);
}
