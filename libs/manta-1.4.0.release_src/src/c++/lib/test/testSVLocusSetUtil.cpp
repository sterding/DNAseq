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

#include "testSVLocusSetUtil.hh"
#include "test/testFileMakers.hh"

#include "boost/make_unique.hpp"



std::unique_ptr<SVLocusSet>
getSerializedSVLocusSetCopy(
    const SVLocusSet& set)
{
    TestFilenameMaker testFilenameMaker;
    const char* testFilenamePtr(testFilenameMaker.getFilename().c_str());

    // serialize
    set.save(testFilenamePtr);

    // deserialize
    return boost::make_unique<SVLocusSet>(testFilenamePtr);
}
