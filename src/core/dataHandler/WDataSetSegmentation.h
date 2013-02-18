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

#ifndef WDATASETSEGMENTATION_H
#define WDATASETSEGMENTATION_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSet.h"
#include "WDataSetScalar.h"
#include "WDataSetSingle.h"


/**
 * A dataset that stores the segmentation of the brain into CSF, white and gray matter.
 *
 * It also offers some convenience functions for this task.
 *
 * \ingroup dataHandler
 */
class WDataSetSegmentation : public WDataSetSingle
{
public:
    /**
     * Constructs an instance out of a value set and a grid.
     *
     * \param segmentation the value set to use
     * \param grid the grid which maps world space to the value set
     */
    WDataSetSegmentation( boost::shared_ptr< WValueSetBase > segmentation, boost::shared_ptr< WGrid > grid );

    /**
     * Constructs an instance out of three WDataSetScalar.
     *
     * \param whiteMatter the value set to use
     * \param grayMatter the value set to use
     * \param cerebrospinalFluid the value set to use
     */
    WDataSetSegmentation( boost::shared_ptr< WDataSetScalar > whiteMatter,
                          boost::shared_ptr< WDataSetScalar > grayMatter,
                          boost::shared_ptr< WDataSetScalar > cerebrospinalFluid );
    /**
     * Construct an empty and unusable instance. This is useful for prototypes.
     */
    WDataSetSegmentation();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetSegmentation();

    /**
     * Returns the white matter probability for the given cell.
     *
     * \param x, y, z The coordinates in grid space.
     *
     * \return white matter probability.
     */
    float getWMProbability( int x, int y, int z ) const;

    /**
     * Returns the gray matter probability for the given cell.
     *
     * \param x, y, z The coordinates in grid space.
     *
     * \return gray matter probability.
     */
    float getGMProbability( int x, int y, int z ) const;

    /**
     * Returns the cerebrospinal fluid probability for the given cell.
     *
     * \param x, y, z The coordinates in grid space.
     *
     * \return cerebrospinal fluid probability.
     */
    float getCSFProbability( int x, int y, int z ) const;

    // template < typename T > T getWMValueAt( int x, int y, int z ) const;

    // template < typename T > T getGMValueAt( int x, int y, int z ) const;

    // template < typename T > T getCSFValueAt( int x, int y, int z ) const;

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Creates a copy (clone) of this instance but allows one to change the valueset. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newValueSet the new valueset.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WValueSetBase > newValueSet ) const;

    /**
     * Creates a copy (clone) of this instance but allows one to change the grid. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newGrid the new grid.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WGrid > newGrid ) const;

    /**
     * Creates a copy (clone) of this instance. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Enumerator for the three different classification types.
     */
    enum matterType
    {
      whiteMatter = 0,
      grayMatter = 1,
      csf = 2
    };

    // double getValueAt( int x, int y, int z );

    // void countVoxel() const;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * This helper function converts the probabilities given by three separate WDataSetScalars to one WValueSetBase.
     *
     * \param whiteMatter the probabilities for white matter.
     * \param grayMatter the probabilities for gray matter.
     * \param cerebrospinalFluid the probabilities for cerebrospinal fluid.
     *
     * \return The probabilities in one value set.
     */
    static boost::shared_ptr< WValueSetBase > convert( boost::shared_ptr< WDataSetScalar > whiteMatter,
                                                       boost::shared_ptr< WDataSetScalar > grayMatter,
                                                       boost::shared_ptr< WDataSetScalar > cerebrospinalFluid );

    /**
     * This helper function copies the content of several WDataSetScalars to one std::vector.
     *
     * \param dataSets the std::vector of data WDataSetScalars.
     *
     * \return The WDataSetScalars merged to a std::vector.
     */
    template< typename T > static std::vector< T > copyDataSetsToArray( const std::vector< boost::shared_ptr< WDataSetScalar > > &dataSets );
};

template< typename T > std::vector< T >
WDataSetSegmentation::copyDataSetsToArray( const std::vector< boost::shared_ptr< WDataSetScalar > > &dataSets )
{
    const size_t voxelDim = dataSets.size();
    size_t countVoxels = 0;
    if( !dataSets.empty() ) countVoxels = ( *dataSets.begin() )->getValueSet()->size();

    std::vector< T > data( countVoxels * voxelDim );

    // loop over images
    size_t dimIndex = 0;
    for( std::vector< boost::shared_ptr< WDataSetScalar > >::const_iterator it = dataSets.begin(); it != dataSets.end(); it++ )
    {
      for( size_t voxelNumber = 0; voxelNumber < countVoxels; voxelNumber++ )
      {
        data[ voxelNumber * voxelDim + dimIndex ] =  ( boost::static_pointer_cast< WDataSetSingle > ( *it ) )->getValueAt< T >( voxelNumber );
      }
      dimIndex++;
    }
    return data;
}


// template < typename T > T WDataSetSegmentation::getValueAt( int x, int y, int z )
// {
//     boost::shared_ptr< WValueSet< T > > vs = boost::dynamic_pointer_cast< WValueSet< T > >( m_valueSet );
//     boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );
//
//     size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();
//
//     T v = vs->getScalar( id );
//     return v;
// }


#endif  // WDATASETSEGMENTATION_H
