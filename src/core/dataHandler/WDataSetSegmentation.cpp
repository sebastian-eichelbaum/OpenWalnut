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

#include <string>
#include <vector>

#include "WDataSetScalar.h"

#include "WDataSetSegmentation.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetSegmentation::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetSegmentation::WDataSetSegmentation( boost::shared_ptr< WDataSetScalar > whiteMatter,
                                            boost::shared_ptr< WDataSetScalar > grayMatter,
                                            boost::shared_ptr< WDataSetScalar > cerebrospinalFluid )
    : WDataSetSingle( convert( whiteMatter, grayMatter, cerebrospinalFluid ), whiteMatter->getGrid() )
{
    boost::shared_ptr< WGrid > grid( whiteMatter->getGrid() );
}

WDataSetSegmentation::WDataSetSegmentation( boost::shared_ptr< WValueSetBase > segmentation,
                                            boost::shared_ptr< WGrid > grid )
    : WDataSetSingle( segmentation, grid )
{
//     countVoxel();
}

WDataSetSegmentation::WDataSetSegmentation()
    : WDataSetSingle()
{
    // default constructor used by the prototype mechanism
}

WDataSetSegmentation::~WDataSetSegmentation()
{
}


const std::string WDataSetSegmentation::getName() const
{
    return "WDataSetSegmentation";
}

const std::string WDataSetSegmentation::getDescription() const
{
    return "Segmentation of brain into white and gray matter, and CSF.";
}

WDataSetSingle::SPtr WDataSetSegmentation::clone( boost::shared_ptr< WValueSetBase > newValueSet, boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetSegmentation( newValueSet, newGrid ) );
}

WDataSetSingle::SPtr WDataSetSegmentation::clone( boost::shared_ptr< WValueSetBase > newValueSet ) const
{
    return WDataSetSingle::SPtr( new WDataSetSegmentation( newValueSet, getGrid() ) );
}

WDataSetSingle::SPtr WDataSetSegmentation::clone( boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetSegmentation( getValueSet(), newGrid ) );
}

WDataSetSingle::SPtr WDataSetSegmentation::clone() const
{
    return WDataSetSingle::SPtr( new WDataSetSegmentation( getValueSet(), getGrid() ) );
}

boost::shared_ptr< WPrototyped > WDataSetSegmentation::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetSegmentation() );
    }

    return m_prototype;
}

// uint WDataSetSegmentation::xsize() const
// {
//   return m_xsize;
// }

// uint WDataSetSegmentation::ysize() const
// {b
//   return m_ysize;
// }

// uint WDataSetSegmentation::zsize() const
// {
//   return m_zsize;
// }

float WDataSetSegmentation::getWMProbability( int x, int y, int z ) const
{
  boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );
  size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

  return WDataSetSingle::getSingleRawValue( whiteMatter + ( 3*id ) );
}

float WDataSetSegmentation::getGMProbability( int x, int y, int z ) const
{
  boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );
  size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

  return WDataSetSingle::getSingleRawValue( grayMatter + ( 3*id ) );
}

float WDataSetSegmentation::getCSFProbability( int x, int y, int z ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );
    size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

    return WDataSetSingle::getSingleRawValue( csf + ( 3*id ) );
}

boost::shared_ptr< WValueSetBase > WDataSetSegmentation::convert( boost::shared_ptr< WDataSetScalar > whiteMatter,
                                                                  boost::shared_ptr< WDataSetScalar > grayMatter,
                                                                  boost::shared_ptr< WDataSetScalar > cerebrospinalFluid )
{
    // valid pointer?
    WAssert( whiteMatter, "No white matter data given." );
    WAssert( grayMatter, "No gray matter data given." );
    WAssert( cerebrospinalFluid, "No CSF data given." );

    // check for same dimension of all three tissue types
    boost::shared_ptr< WGridRegular3D > wm_grid = boost::dynamic_pointer_cast< WGridRegular3D >( whiteMatter->getGrid() );
    boost::shared_ptr< WGridRegular3D > gm_grid = boost::dynamic_pointer_cast< WGridRegular3D >( grayMatter->getGrid() );
    boost::shared_ptr< WGridRegular3D > csf_grid = boost::dynamic_pointer_cast< WGridRegular3D >( cerebrospinalFluid->getGrid() );

    WAssert( ( wm_grid->getNbCoordsX() == gm_grid->getNbCoordsX() ) &&  ( gm_grid->getNbCoordsX() == csf_grid->getNbCoordsX() ),
             "Different X size of GrayMatter, WhiteMatter or CSF-Input" );
    WAssert( ( wm_grid->getNbCoordsY() == gm_grid->getNbCoordsY() ) &&  ( gm_grid->getNbCoordsY() == csf_grid->getNbCoordsY() ),
             "Different Y size of GrayMatter, WhiteMatter or CSF-Input" );
    WAssert( ( wm_grid->getNbCoordsZ() == gm_grid->getNbCoordsZ() ) &&  ( gm_grid->getNbCoordsZ() == csf_grid->getNbCoordsZ() ),
             "Different Z size of GrayMatter, WhiteMatter or CSF-Input" );

    boost::shared_ptr< WValueSetBase > segmentation;
    std::vector< boost::shared_ptr< WDataSetScalar > > dataSets;
    dataSets.push_back( whiteMatter );
    dataSets.push_back( grayMatter );
    dataSets.push_back( cerebrospinalFluid );

    switch( whiteMatter->getValueSet()->getDataType() )
    {
    case W_DT_UNSIGNED_CHAR:
    {
        boost::shared_ptr< std::vector< unsigned char > > data( new std::vector< unsigned char > );
        *data = copyDataSetsToArray< unsigned char >( dataSets );
        segmentation = boost::shared_ptr< WValueSetBase >( new WValueSet< unsigned char >( 1, dataSets.size(), data, W_DT_UNSIGNED_CHAR ) );
        break;
    }
    case W_DT_INT16:
    {
        boost::shared_ptr< std::vector< int16_t > > data( new std::vector< int16_t > );
        *data = copyDataSetsToArray< int16_t >( dataSets );
        segmentation = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( 1, dataSets.size(), data, W_DT_INT16 ) );
        break;
    }
    case W_DT_SIGNED_INT:
    {
        boost::shared_ptr< std::vector< int32_t > > data( new std::vector< int32_t > );
        *data = copyDataSetsToArray< int32_t >( dataSets );
        segmentation = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( 1, dataSets.size(), data, W_DT_SIGNED_INT ) );
        break;
    }
    case W_DT_FLOAT:
    {
        boost::shared_ptr< std::vector< float > > data( new std::vector< float > );
        *data = copyDataSetsToArray< float >( dataSets );
        segmentation = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( 1, dataSets.size(), data, W_DT_FLOAT ) );
        break;
    }
    case W_DT_DOUBLE:
    {
        boost::shared_ptr< std::vector< double > > data( new std::vector< double > );
        *data = copyDataSetsToArray< double >( dataSets );
        segmentation = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( 1, dataSets.size(), data, W_DT_DOUBLE ) );
        break;
    }
    default:
        WAssert( false, "Unknown data type in dataset." );
    }
    return segmentation;
}

// void WDataSetSegmentation::countVoxel() const
// {
//   size_t WMVoxel = 0;
//   size_t GMVoxel = 0;
//   size_t CSFVoxel = 0;
//   boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( getGrid() );
//   for( size_t x = 0; x < grid->getNbCoordsX(); x++ )
//     for( size_t y = 0; y < grid->getNbCoordsY(); y++ )
//       for( size_t z = 0; z < grid->getNbCoordsZ(); z++ )
//       {
// //         std::cerr << getWMProbability( x, y, z ) << std::endl;
//         if ( getWMProbability( x, y, z ) > 0.95 ) WMVoxel++;
//         if ( getGMProbability( x, y, z ) > 0.95 ) GMVoxel++;
//         if ( getCSFProbability( x, y, z ) > 0.95 ) CSFVoxel++;
//       }
//   std::cerr << "WMVoxel: " << WMVoxel << std::endl;
//   std::cerr << "GMVoxel: " << GMVoxel << std::endl;
//   std::cerr << "CSFVoxel: " << CSFVoxel << std::endl;
// }
