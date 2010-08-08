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

#ifndef WDXTLOOKUPTABLE_H
#define WDXTLOOKUPTABLE_H

#include <vector>

#include "../exceptions/WOutOfBounds.h"
#include "../WExportCommon.h"

/**
 * Represents a symmetric matrix-like look up table, meaning it stores only
 * the elements inside the triangular matrix without the main diagonal.
 *
 * So in case of a NxN matrix there are only (N^2-N)/2 elements to store.
 *
 * The reason why this is named DXt look up table is, since is it used as
 * look up table for dSt and dLt fiber distance metrics.
 */
class OWCOMMON_EXPORT WDXtLookUpTable
{
friend class WDXtLookUpTableTest;
public:
    /**
     * Generates new look up table.
     *
     * \param dim the dimension of the square matrix
     */
    explicit WDXtLookUpTable( size_t dim );

    /**
     * Element acces operator as if the elements where stored as a matrix.
     * \warning Acessing elements of the main diagonal is forbidden!
     *
     * \param i The i'th row
     * \param j The j'th column
     * \return reference to the (i,j) element of the table
     */
    double& operator()( size_t i, size_t j ) throw( WOutOfBounds );

    /**
     * \return the number of distances inside this table.
     */
    size_t size() const;

    /**
     * \return the dimensionality of this table
     */
    size_t dim() const;

    /**
     * \return readonly access to the internal data
     */
    const std::vector< double >& getData() const;

    /**
     * Resets the internal data (e.g. loaded from file).
     *
     * \param data Vector containing the new elements
     */
    void setData( const std::vector< double > &data ) throw( WOutOfBounds );

private:
    /**
     * Internal data structure to store the elements. The order is line major
     * meaning first element (0,0)...(0,n-1),(1,0)...(1, n-1)...
     */
    std::vector< double > m_data;

    size_t m_dim;  //!< Matrix dimension
};

#endif  // WDXTLOOKUPTABLE_H
