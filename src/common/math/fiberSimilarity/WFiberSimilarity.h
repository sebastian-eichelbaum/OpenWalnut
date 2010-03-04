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

#ifndef WFIBERSIMILARITY_H
#define WFIBERSIMILARITY_H

// forward declaration
namespace wmath
{
    class WFiber;
}

/**
 * Abstract interface to all fiber similarity measures. The term
 * "fiber-similarity" is closely related to the term fiber-distance. Basically
 * fibers that are very close to each other (low distance) are considered
 * similar.
 */
class WFiberSimilarity
{
public:
    /**
     * Computes the similarity between two fibers Q and R. Values near 0
     * indicate very strong similarity and low distance.
     *
     * \param q First fiber
     * \param r Second fiber
     * \return Distance between fibers
     */
    virtual double dist( const wmath::WFiber &q, const wmath::WFiber &r ) const = 0;

    /**
     * Virtual destructor since there is a virtual member function.
     */
    virtual ~WFiberSimilarity();

protected:
private:
};

#endif  // WFIBERSIMILARITY_H
