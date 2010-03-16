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

#include "WDataTexture3D.h"
#include "WValueSet.h"
#include "WGrid.h"
#include "../common/WPrototyped.h"
#include "../common/WException.h"

#include "WDataSetSingle.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetSingle::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetSingle::WDataSetSingle( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSet()
{
    assert( newValueSet );
    assert( newGrid );
    assert( newValueSet->size() == newGrid->size() );

    m_valueSet = newValueSet;
    m_grid = newGrid;
    m_texture3D = boost::shared_ptr< WDataTexture3D >( new WDataTexture3D( m_valueSet, m_grid ) );
}

WDataSetSingle::WDataSetSingle()
    : WDataSet(),
    m_valueSet(),
    m_grid(),
    m_texture3D()
{
    // default constructor used by the prototype mechanism
}

WDataSetSingle::~WDataSetSingle()
{
}

boost::shared_ptr< WValueSetBase > WDataSetSingle::getValueSet() const
{
    return m_valueSet;
}

boost::shared_ptr< WGrid > WDataSetSingle::getGrid() const
{
    return m_grid;
}

bool WDataSetSingle::isTexture() const
{
    // TODO(all): this is not sophisticated. This should depend on type of data (vectors? scalars? tensors?)
    return true;
}

boost::shared_ptr< WDataTexture3D > WDataSetSingle::getTexture()
{
    return m_texture3D;
}

const std::string WDataSetSingle::getName() const
{
    return "WDataSetSingle";
}

const std::string WDataSetSingle::getDescription() const
{
    // TODO(all): write this
    return "Please write me.";
}

boost::shared_ptr< WPrototyped > WDataSetSingle::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetSingle() );
    }

    return m_prototype;
}

double WDataSetSingle::getValueAt( size_t id )
{
    switch( getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< unsigned char > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_INT16:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< int16_t > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_SIGNED_INT:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< int32_t > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_FLOAT:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< float > >( getValueSet() )->getScalar( id ) );
        }
        case W_DT_DOUBLE:
        {
            return static_cast< double >( boost::shared_dynamic_cast< WValueSet< double > >( getValueSet() )->getScalar( id ) );
        }
        default:
            assert( false && "Unknow data type in dataset." );
    }

    return 0.0;
}

double WDataSetSingle::getValueAt( int x, int y, int z )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );
    size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

    return getValueAt( id );
}

double WDataSetSingle::interpolate( wmath::WPosition pos, bool* success )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    // TODO(wiebel): change this to eassert.
    if( !grid )
    {
        throw WException( std::string(  "This data set has a grid whose type is not yet supported for interpolation." ) );
    }
    // TODO(wiebel): change this to eassert.
    if( grid->getTransformationMatrix() != wmath::WMatrix<double>( 4, 4 ).makeIdentity()  )
    {
        throw WException( std::string( "Only feasible for untranslated grid so far." ) );
    }
    // TODO(wiebel): change this to eassert.
    if( !( m_valueSet->order() == 0 &&  m_valueSet->dimension() == 1 ) )
    {
        throw WException( std::string( "Only implemented for scalar values so far." ) );
    }

    *success = grid->encloses( pos );

    if( !*success )
    {
        return 0;
    }

    std::vector< size_t > vertexIds = grid->getCellVertexIds( grid->getCellId( pos ) );

    wmath::WPosition localPos = pos - grid->getPosition( vertexIds[0] );

    double lambdaX = localPos[0] / grid->getOffsetX();
    double lambdaY = localPos[1] / grid->getOffsetY();
    double lambdaZ = localPos[2] / grid->getOffsetZ();
    std::vector< double > h( 8 );
//         lZ     lY
//         |      /
//         | 6___/_7
//         |/:    /|
//         4_:___5 |
//         | :...|.|
//         |.2   | 3
//         |_____|/ ____lX
//        0      1
    h[0] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[1] = (     lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[2] = ( 1 - lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[3] = (     lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[4] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[5] = (     lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[6] = ( 1 - lambdaX ) * (     lambdaY ) * (     lambdaZ );
    h[7] = (     lambdaX ) * (     lambdaY ) * (     lambdaZ );

    double result = 0;
    for( size_t i = 0; i < 8; ++i )
    {
        result += h[i] * getValueAt( vertexIds[i] );
    }

    *success = true;
    return result;
}
