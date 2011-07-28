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

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "WBoundaryFragments.h"

WBoundaryFragments::WBoundaryFragments( boost::shared_ptr< const WDataSetScalar > texture, boost::shared_ptr< const WProperties > properties,
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices, boost::filesystem::path localPath )
    : WBoundaryBuilder( texture, properties, slices )
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMBoundaryCurvesWMGM-Fragments", localPath ) );
}

void WBoundaryFragments::run( osg::ref_ptr< WGEManagedGroupNode > output, const char /* sliceNum */ )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_texture->getGrid() );

    // create a new geodes containing the slices
    m_slices[0]->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsY() * grid->getDirectionY(),
                grid->getNbCoordsZ() * grid->getDirectionZ() ) );
    m_slices[1]->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                grid->getNbCoordsZ() * grid->getDirectionZ() ) );
    m_slices[2]->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                grid->getNbCoordsY() * grid->getDirectionY() ) );

    osg::ref_ptr< osg::Uniform > u_grayMatter = new WGEPropertyUniform< WPropDouble >( "u_grayMatter", m_grayMatter );
    osg::ref_ptr< osg::Uniform > u_whiteMatter = new WGEPropertyUniform< WPropDouble >( "u_whiteMatter", m_whiteMatter );
    osg::ref_ptr< osg::Uniform > u_gmColor = new WGEPropertyUniform< WPropColor >( "u_gmColor", m_gmColor );
    osg::ref_ptr< osg::Uniform > u_wmColor = new WGEPropertyUniform< WPropColor >( "u_wmColor", m_wmColor );

    for( char i = 0; i < 3; ++i )
    {
        m_shader->apply( m_slices[i] );
        wge::bindTexture( m_slices[i], m_texture->getTexture() );
        osg::StateSet *ss = m_slices[i]->getOrCreateStateSet();
        ss->addUniform( u_grayMatter );
        ss->addUniform( u_whiteMatter );
        ss->addUniform( u_gmColor );
        ss->addUniform( u_wmColor );
        ss->addUniform( new WGEPropertyUniform< WPropDouble >( "u_vertexShift", m_slicePos[i] ) );
        ss->addUniform( new osg::Uniform( "u_vertexShiftDirection", static_cast< osg::Vec3f >( getDirections( grid )[i] ) ) );
        output->insert( m_slices[i] );
    }
}
