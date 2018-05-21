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

#include "blt_util/known_pos_range2.hh"

#include "boost/optional.hpp"

// This extra boost serialization.hpp include is required before including set.hpp, due to bug observed in boost 1.58
// and described here: https://svn.boost.org/trac10/ticket/12126
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/set.hpp"

#include <iosfwd>
#include <map>
#include <set>


/// sort pos range using end_pos as the primary sort key
struct PosRangeEndSort
{
    bool
    operator()(
        const known_pos_range2& lhs,
        const known_pos_range2& rhs) const
    {
        if (lhs.end_pos() < rhs.end_pos()) return true;
        if (lhs.end_pos() == rhs.end_pos())
        {
            if (lhs.begin_pos() < rhs.begin_pos()) return true;
        }
        return false;
    }
};


/// Aggregate multiple regions and support intersection queries against the full region set
///
/// This is designed to (optionally) facilitate 'rolling' region tracking along the chromosome.
///
struct RegionTracker
{
    bool
    empty() const
    {
        return _regions.empty();
    }

    void
    clear()
    {
        _regions.clear();
    }

    /// is single position in a tracked region?
    bool
    isIntersectRegion(
        const pos_t pos) const
    {
        return isIntersectRegionImpl(pos,pos+1);
    }

    /// does range intersect any tracked region?
    bool
    isIntersectRegion(
        const known_pos_range2 range) const
    {
        return isIntersectRegionImpl(range.begin_pos(),range.end_pos());
    }

    /// is range entirely contained in a region?
    bool
    isSubsetOfRegion(
        const known_pos_range2 range) const
    {
        return isSubsetOfRegionImpl(range.begin_pos(),range.end_pos());
    }

    /// add region
    ///
    /// any overlaps and adjacencies with existing regions in the tracker will be collapsed
    void
    addRegion(
        known_pos_range2 range);

    /// remove all regions which end (inclusive) before pos+1
    void
    removeToPos(
        const pos_t pos);

    // debug util
    void
    dump(
        std::ostream& os) const;

    unsigned
    size() const
    {
        return _regions.size();
    }

    /// Merge all range data from \p rhs into this object
    void
    merge(const RegionTracker& rhs);

    template<class Archive>
    void serialize(Archive& ar, const unsigned /* version */)
    {
        ar& _regions;
    }

    typedef std::set<known_pos_range2,PosRangeEndSort>  region_t;

private:

    bool
    isIntersectRegionImpl(
        const pos_t beginPos,
        const pos_t endPos) const;

    bool
    isSubsetOfRegionImpl(
        const pos_t beginPos,
        const pos_t endPos) const;

    region_t _regions;
};

BOOST_CLASS_IMPLEMENTATION(RegionTracker, boost::serialization::object_serializable)


/// Aggregate multiple regions and support intersection queries against the full region set
///
/// This is designed to (optionally) facilitate 'rolling' region tracking along the chromosome.
///
/// This specialization of RegionTracker carries a payload associated with each region
///
template <typename T>
struct RegionPayloadTracker
{
    bool
    empty() const
    {
        return _regions.empty();
    }

    void
    clear()
    {
        _regions.clear();
    }

    /// is single position in a tracked region w/ payload?
    boost::optional<T>
    isIntersectRegion(
        const pos_t pos) const
    {
        return isIntersectRegionImpl(pos,pos+1);
    }

    // commenting out pending definition of expected behavior when
    // the query range intercepts more than one tracked range, what
    // is the payload returned in such a case?
#if 0
    /// does range intersect any tracked region w/ payload?
    boost::optional<T>
    isIntersectRegion(
        const known_pos_range2 range) const
    {
        return isIntersectRegionImpl(range.begin_pos(),range.end_pos());
    }
#endif

    /// is range entirely contained in a tracked region w/ payload?
    boost::optional<T>
    isSubsetOfRegion(
        const known_pos_range2 range) const
    {
        return isSubsetOfRegionImpl(range.begin_pos(),range.end_pos());
    }

    /// add region
    ///
    /// any non-conflicting overlaps and adjacencies with existing regions in the tracker will be collapsed
    ///
    /// \returns false when there is an overlapping payload conflict. in this case the region is not inserted
    bool
    addRegion(
        known_pos_range2 range,
        const T payload);

    /// remove all regions which end (inclusive) before pos+1
    void
    removeToPos(
        const pos_t pos);

    // debug util
    void
    dump(
        std::ostream& os) const;

    unsigned
    size() const
    {
        return _regions.size();
    }

    typedef typename std::map<known_pos_range2,T,PosRangeEndSort> region_t;

private:

    boost::optional<T>
    isIntersectRegionImpl(
        const pos_t beginPos,
        const pos_t endPos) const;

    boost::optional<T>
    isSubsetOfRegionImpl(
        const pos_t beginPos,
        const pos_t endPos) const;

    region_t _regions;
};


#include "RegionTrackerImpl.hh"

