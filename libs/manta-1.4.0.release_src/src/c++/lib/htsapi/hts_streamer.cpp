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


#include "hts_streamer.hh"
#include "common/Exceptions.hh"

#include <iostream>
#include <sstream>


static const kstring_t kinit = {0,0,0};



hts_streamer::
hts_streamer(
    const char* filename,
    const char* region) :
    _is_record_set(false),
    _is_stream_end(false),
    _record_no(0),
    _stream_name(filename),
    _hfp(nullptr),
    _tidx(nullptr),
    _titr(nullptr),
    _kstr(kinit)
{
    if (! filename)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("hts filename is null ptr"));
    }

    if ('\0' == *filename)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("hts filename is empty string"));
    }

    _hfp = hts_open(filename, "r");
    if (! _hfp)
    {
        std::ostringstream oss;
        oss << "Failed to open hts file: '" << filename << "'";
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException(oss.str()));
    }

    _load_index();

    // read only a region of HTS file:
    if (region)
    {
        resetRegion(region);
    }
}



hts_streamer::
~hts_streamer()
{
    if (_titr) tbx_itr_destroy(_titr);
    if (_tidx) tbx_destroy(_tidx);
    if (_hfp) hts_close(_hfp);
    if (_kstr.s) free(_kstr.s);
}



void
hts_streamer::
resetRegion(
    const char* region)
{
    if (_titr) tbx_itr_destroy(_titr);

    _titr = tbx_itr_querys(_tidx, region);
    _is_stream_end = (! _titr);
}



void
hts_streamer::
_load_index()
{
    if (_tidx) return;

    _tidx = tbx_index_load(name());
    if (! _tidx)
    {
        std::ostringstream oss;
        oss << "Failed to load index for hts file: '" << name() << "'";
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException(oss.str()));
    }
}
