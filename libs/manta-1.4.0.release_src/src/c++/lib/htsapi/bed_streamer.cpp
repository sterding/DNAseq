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

#include "bed_streamer.hh"
#include "common/Exceptions.hh"

#include <iostream>
#include <sstream>



bed_streamer::
bed_streamer(
    const char* filename,
    const char* region,
    const bool requireNonZeroRegionLength) :
    hts_streamer(filename,region),
    _requireNonZeroRegionLength(requireNonZeroRegionLength)
{
    if (! region)
    {
        BOOST_THROW_EXCEPTION(illumina::common::GeneralException("BED streamer region is nullptr"));
    }
}



bool
bed_streamer::
next()
{
    if (_is_stream_end || (! _hfp) || (! _titr)) return false;

    while (true)
    {
        if (tbx_itr_next(_hfp, _tidx, _titr, &_kstr) < 0)
        {
            _is_stream_end=true;
        }
        else
        {
            _is_stream_end=(! _kstr.s);
        }
        _is_record_set=(! _is_stream_end);
        if (! _is_record_set) break;

        // filter out header for whole file access case:
        if (_kstr.s[0] == '#') continue;

        _record_no++;

        if (! _bedrec.set(_kstr.s))
        {
            std::ostringstream oss;
            oss << "Can't parse BED record: '" << _kstr.s << "'";
            report_state(oss);
            BOOST_THROW_EXCEPTION(illumina::common::GeneralException(oss.str()));
        }
        if (! _bedrec.is_valid())
        {
            if (_requireNonZeroRegionLength)
            {
                // throw on invalid records:
                std::ostringstream oss;
                oss << "Input BED record has size less than one:\n";
                report_state(oss);
                BOOST_THROW_EXCEPTION(illumina::common::GeneralException(oss.str()));
            }
            else
            {
                // skip invalid records:
                continue;
            }
        }
        break;
    }

    return _is_record_set;
}



void
bed_streamer::
report_state(std::ostream& os) const
{
    const bed_record* bedp(get_record_ptr());

    os << "\tbed_stream_label: " << name() << "\n";
    if (bedp)
    {
        os << "\tbed_stream_record_no: " << record_no() << "\n"
           << "\tbed_record: " << *(bedp) << "\n";
    }
    else
    {
        os << "\tno bed record currently set\n";
    }
}
