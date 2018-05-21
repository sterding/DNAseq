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

#include "EdgeOptions.hh"

#include "boost/program_options.hpp"

#include <string>


boost::program_options::options_description
getOptionsDescription(
    EdgeOptions& opt);


/// additional parsing beyond default boost::program_options behaviors,
/// and arg validation
///
/// \return is valid
bool
parseOptions(
    const boost::program_options::variables_map& vm,
    EdgeOptions& opt,
    std::string& errorMsg);
