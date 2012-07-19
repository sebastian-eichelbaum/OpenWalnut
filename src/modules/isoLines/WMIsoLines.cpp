//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "modules/emptyIcon.xpm" // Please put a real icon here.
#include "WMIsoLines.h"

W_LOADABLE_MODULE( WMIsoLines )

WMIsoLines::WMIsoLines():
    WModule()
{
}

WMIsoLines::~WMIsoLines()
{
}

boost::shared_ptr< WModule > WMIsoLines::factory() const
{
    return boost::shared_ptr< WModule >( new WMIsoLines() );
}

const char** WMIsoLines::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMIsoLines::getName() const
{
    return "Isoline";
}

const std::string WMIsoLines::getDescription() const
{
    return "Renders isolines in a specific color.";
}

void WMIsoLines::connectors()
{
    m_scalarIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalarData", "Scalar data." );

    WModule::connectors();
}

void WMIsoLines::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    m_Pos = m_properties->addProperty( "Slice Position", "Where the data shoulde be sliced for drawing contours", 0.0 );
    m_isovalue = m_properties->addProperty( "Isovalue", "Value selecting the contours", 0.0 );
    m_isovalue->setMin( 0.0 );
    m_isovalue->setMax( 0.0 ); // make it unusable at first

    WModule::properties();
}

void WMIsoLines::requirements()
{
}

void WMIsoLines::initOSG( const WBoundingBox& bb )
{
    debugLog() << "Init OSG";
    m_output->clear();

    // grab the current bounding box for computing the size of the slice
    WVector3d minV = bb.getMin();
    WVector3d maxV = bb.getMax();
    WVector3d sizes = ( maxV - minV );
    WVector3d midBB = minV + ( sizes * 0.5 );

    // update the properties
    m_Pos->setMin( minV[1] );
    m_Pos->setMax( maxV[1] );
    m_Pos->set( midBB[1] );

    // each slice is child of an transformation node
    osg::ref_ptr< osg::MatrixTransform > mT = new osg::MatrixTransform();

    osg::ref_ptr< osg::Node > slice = wge::genFinitePlane( minV, osg::Vec3( sizes[0], 0.0, 0.0 ),
                                                                 osg::Vec3( 0.0, 0.0, sizes[2] ) );
    slice->setCullingActive( false );
    mT->addChild( slice );

    osg::ref_ptr< osg::Uniform > u_WorldTransform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    osg::ref_ptr< osg::Uniform > u_isovalue = new WGEPropertyUniform< WPropDouble >( "u_isovalue", m_isovalue );

    osg::StateSet *states = m_output->getOrCreateStateSet();
    states->addUniform( u_WorldTransform );
    states->addUniform( u_isovalue );

    // Control transformation node by properties. We use an additional uniform here to provide the shader
    // the transformation matrix used to translate the slice.
    mT->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 0.0, 1.0, 0.0 ), m_Pos, u_WorldTransform ) );

    m_output->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    m_output->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_output->insert( mT );
    m_output->dirtyBound();
}

void WMIsoLines::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarIC->getDataChangedCondition() );

    ready();

    // graphics setup
    m_output = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
    osg::ref_ptr< WGEShader > shader = new WGEShader( "WIsoline", m_localPath );
    shader->apply( m_output ); // this automatically applies the shader

    // main loop
    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WDataSetScalar > scalarData = m_scalarIC->getData();

        if( !scalarData )
        {
            continue;
        }

        m_isovalue->setMin( 0.0 );
        m_isovalue->setMax( 1.0 );
        // m_isovalue->setMin( scalarData->getMin() );
        // m_isovalue->setMax( scalarData->getMax() );

        initOSG( scalarData->getGrid()->getBoundingBox() );

        wge::bindTexture( m_output, scalarData->getTexture(), 0, "u_scalarData" );

        // TODO(math): unbind textures, so we have a clean OSG root node for this module again
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}
