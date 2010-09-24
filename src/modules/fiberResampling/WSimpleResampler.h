//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WSIMPLERESAMPLER_H
#define WSIMPLERESAMPLER_H

#include <vector>

#include <boost/shared_ptr.hpp>

/**
 * Algorithm to resample a single tract into almost equidistant segments.
 *
 * \note Since this is a SIMPLE resampler, it measures along the given tract or
 * curve an equidistant width, and places its sample points at each segment
 * end. This clearly leads to shorter segments incase of strong curvature
 * (which is taken to measure the segments) replaced by a straight segment line
 * which may be shorter then.
 */
class WSimpleResampler
{
friend class WSimpleResamplerTest;
public:
    /**
     * Constructs a new Sampling instance for a given number of sample points.
     *
     * \param numSamples The new number of sample points.
     */
    explicit WSimpleResampler( size_t numSamples );

    /**
     * Resamples a tract (given via <em>verts, startIdx</em> and \e length) to
     * the number of samples points given in the constructor of this instance.
     *
     * \param verts Pointer to the original vertices array
     *
     * \param startIdx Start position of the first vertex.
     *
     * \param length The number of VERTICES (not components nor doubles) the
     * tract is about.
     *
     * \param newVerts The place where to save the new sample points. Since all
     * new tracts will have the same length the start index can be derived from
     * startIdx.
     *
     * \param newStartIdx The new offset where to write the new sample points.
     * This is also a direct component or double index (as startIdx is) but not
     * a vertex index.
     *
     * \note The only reason for the fourth argument is that the memory can be
     * allocated before the sampling takes places which could lead to
     * performance losses when multithreading and dynamic allocation comes into
     * play.
     */
    void resample( boost::shared_ptr< const std::vector< double > > verts,
                   const size_t startIdx,
                   const size_t length,
                   boost::shared_ptr< std::vector< double > > newVerts,
                   const size_t newStartIdx ) const;

protected:
    /**
     * Accumulates all line segment lengths of a certain tract.
     *
     * \param verts Vertex array of all tracts
     * \param startIdx Idx where the specific tract starts
     * \param length How many vertices are used.
     *
     * \return The sum of all line segement lengths
     */
    double lineIntegration( boost::shared_ptr< const std::vector< double > > verts,
                            const size_t startIdx,
                            const size_t length ) const;

private:
    /**
     * The new number of points where each given tract is sampled to.
     */
    size_t m_numSamples;
};

#endif  // WSIMPLERESAMPLER_H
