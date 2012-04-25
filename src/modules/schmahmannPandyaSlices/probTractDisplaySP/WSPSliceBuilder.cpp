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

#include <cmath>
#include <string>
#include <vector>

#include <osg/ref_ptr>

#include "core/common/exceptions/WTypeMismatch.h"
#include "core/common/WLogger.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "WSPSliceBuilder.h"

WSPSliceBuilder::WSPSliceBuilder( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap )
    : m_slicePos( 3 ),
      m_probTracts( probTracts ),
      m_sliceBB( 3 ),
      m_colorMap( colorMap ) // yes this is a COPY of the vector but WPropGroup is a boost::shared_ptr so updates will propagate!
{
    m_slicePos[2] = sliceGroup->findProperty( "Axial Position" )->toPropDouble();
    m_slicePos[1] = sliceGroup->findProperty( "Coronal Position" )->toPropDouble();
    m_slicePos[0] = sliceGroup->findProperty( "Sagittal Position" )->toPropDouble();

    checkAndExtractGrids();
    computeSliceBB(); // just to be sure those are initialized, since they may change due to m_slicePos[0], et al. anyway
}

WSPSliceBuilder::~WSPSliceBuilder()
{
    // since we are having virtual member functions we also need a virtual destructor
}

// helper functions only to be DRY
namespace
{
    /**
     * Try a cast to WGridRegular3D, and return the cast result if it was successful, otherwise throw an exception of
     * WTypeMismatched.
     *
     * \param dataset The dataset of which the grid is taken from to check.
     *
     * \return The grid of the dataset casted to WGridRegular3D.
     */
    boost::shared_ptr< const WGridRegular3D > ensureWGridRegular3D( boost::shared_ptr< const WDataSetScalar > dataset )
    {
        boost::shared_ptr< const WGridRegular3D > result = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
        if( !result )
        {
            wlog::error( "WSPSliceBuilder" ) << "Cast to WGridRegular3D failed.";
            throw WTypeMismatch( "WSPSliceBuilder::extractGrid(): WGridRegular3D expected, but cast failed." );
        }
        return result;
    }
}

void WSPSliceBuilder::checkAndExtractGrids()
{
    if( m_probTracts.empty() )
    {
        m_grid.reset();
    }
    else
    {
        try
        {
            m_grid = ensureWGridRegular3D( m_probTracts.front() );

            for( ProbTractList::const_iterator cit = m_probTracts.begin(); cit != m_probTracts.end(); ++cit )
            {
                boost::shared_ptr< const WGridRegular3D > grid = ensureWGridRegular3D( *cit );
                // TODO(math): ensure that each WGridRegular3D is the same once the operator== is available for WGridRegular3D
                // grid == m_grid
            }
        }
        catch( const WTypeMismatch& e )
        {
            wlog::error( "WSPSliceBuilder" ) << "At least one probabilistic tractogram has a grid which is not castable to WGridRegluar3D";
            throw e;
        }
    }
}

WColor WSPSliceBuilder::colorMap( size_t probTractNum ) const
{
    std::string dataSetFileName = m_probTracts[probTractNum]->getFilename();

    for( size_t i = 0; i < m_colorMap.size(); ++i )
    {
        std::string colorMapFileName = m_colorMap[i]->findProperty( "Filename" )->toPropString()->get();
        if( colorMapFileName == dataSetFileName )
        {
            return m_colorMap[i]->findProperty( "Color" )->toPropColor()->get();
        }
    }

    // keep old behaviour
    return m_colorMap.at( probTractNum )->findProperty( "Color" )->toPropColor()->get();
}

bool WSPSliceBuilder::alphaBelowThreshold( const WColor& c, const double threshold ) const
{
    return c[3] < threshold;
}

WColor WSPSliceBuilder::lookUpColor( const WPosition& pos, size_t tractID ) const
{
    WColor c = colorMap( tractID );
    bool success = false;
    double probability = m_probTracts.at( tractID )->interpolate( pos, &success );
    if( m_probTracts.at( tractID )->getMax() > 1 )
    {
        probability /= static_cast< double >( m_probTracts.at( tractID )->getMax() );
    }
    if( c[3] != 0.0 )
    {
        // linear mapping
        c[3] = ( success ? probability : -1.0 );

        // // sinusiodal mapping
        // double pi2 = 2*3.14159265358979323846;
        // c[3] = ( success ? ( pi2*probability - std::sin(pi2*probability) ) / ( pi2 ) : -1.0 );

        // // square root mapping
        // c[3] = ( success ? std::sqrt( probability ) : -1.0 );
    }

    return c;
}

osg::ref_ptr< osg::Vec4Array > WSPSliceBuilder::computeColorsFor( const osg::Vec3& pos ) const
{
    osg::ref_ptr< osg::Vec4Array > result( new osg::Vec4Array );
    result->reserve( m_probTracts.size() );

    // for each probabilisitc tractogram look up if its probability at this vertex is below a certain threshold or not
    for( size_t tractID = 0; tractID < m_probTracts.size(); ++tractID )
    {
        WColor c = lookUpColor( WPosition( pos ), tractID );
        if( c[3] != -1.0 )
        {
            result->push_back( c );
        }
    }

    return result;
}

void WSPSliceBuilder::computeSliceBB()
{
    if( !m_grid )
    {
        wlog::warn( "WSPSliceBuilder" ) << "Invalid grid while BB computation!";
        return;
    }
    m_sliceBB[0] = WBoundingBox( m_grid->getOrigin() + m_slicePos[0]->get() * m_grid->getDirectionX(),
            m_grid->getOrigin() + m_slicePos[0]->get() * m_grid->getDirectionX() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_sliceBB[1] = WBoundingBox( m_grid->getOrigin() + m_slicePos[1]->get() * m_grid->getDirectionY(),
            m_grid->getOrigin() + m_slicePos[1]->get() * m_grid->getDirectionY() + m_grid->getNbCoordsX() * m_grid->getDirectionX() +
            m_grid->getNbCoordsZ() * m_grid->getDirectionZ() );
    m_sliceBB[2] = WBoundingBox( m_grid->getOrigin() + m_slicePos[2]->get() * m_grid->getDirectionZ(),
            m_grid->getOrigin() + m_slicePos[2]->get() * m_grid->getDirectionZ() + m_grid->getNbCoordsY() * m_grid->getDirectionY() +
            m_grid->getNbCoordsX() * m_grid->getDirectionX() );
}
