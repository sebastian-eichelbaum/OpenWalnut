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
#include <cstdlib>
#include <vector>
#include <string>

#include <osg/Vec3>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/math/WMath.h"
#include "../../common/math/WPlane.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../graphicsEngine/callbacks/WGEShaderAnimationCallback.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEOffscreenRenderPass.h"
#include "../../graphicsEngine/WGEOffscreenRenderNode.h"
#include "../../graphicsEngine/WGEPropertyUniform.h"

#include "WMProbTractDisplaySP.h"
#include "WMProbTractDisplaySP.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractDisplaySP )

WMProbTractDisplaySP::WMProbTractDisplaySP():
    WModule()
{
}

WMProbTractDisplaySP::~WMProbTractDisplaySP()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractDisplaySP::factory() const
{
    return boost::shared_ptr< WModule >( new WMProbTractDisplaySP() );
}

const char** WMProbTractDisplaySP::getXPMIcon() const
{
    return WMProbTractDisplaySP_xpm;
}

const std::string WMProbTractDisplaySP::getName() const
{
    return "Prob Tract Display SP";
}

const std::string WMProbTractDisplaySP::getDescription() const
{
    return "Slice based probabilistic tract display based on Schmahmann y Pandya";
}

void WMProbTractDisplaySP::connectors()
{
    m_fibersIC = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "fibersInput", "The deterministic tractograms." );
    m_probIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "probTractInput", "The probabilistic tractogram" );

    // call WModule's initialization
    WModule::connectors();
}

void WMProbTractDisplaySP::properties()
{
    m_sliceGroup      = m_properties->addPropertyGroup( "Slices",  "Slice based probabilistic tractogram display." );

    // enable slices
    m_showonX        = m_sliceGroup->addProperty( "Show Sagittal", "Show instersection of deterministic tracts on sagittal slice.", true );
    m_showonY        = m_sliceGroup->addProperty( "Show Coronal", "Show instersection of deterministic tracts on coronal slice.", true );
    m_showonZ        = m_sliceGroup->addProperty( "Show Axial", "Show instersection of deterministic tracts on axial slice.", true );

    // The slice positions. These get update externally.
    // TODO(math): get the dimensions and MinMax's directly from the probabilistic tractorgram
    // TODO(all): this should somehow be connected to the nav slices.
    m_xPos           = m_sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 0 );
    m_yPos           = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0 );
    m_zPos           = m_sliceGroup->addProperty( "Axial Position", "Slice Z position.", 0 );

    // since we don't know anything yet => make them unusable
    m_xPos->setMin( 0 );
    m_xPos->setMax( 0 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 0 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 0 );

    // call WModule's initialization
    WModule::properties();
}

void WMProbTractDisplaySP::initOSG( boost::shared_ptr< WGridRegular3D > grid )
{
    // remove the old slices
    m_output->clear();

    // we want the tex matrix for each slice to be modified too,
    osg::ref_ptr< osg::TexMat > texMat;

    // create all the transformation nodes
    m_xSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonX ) );
    m_ySlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonY ) );
    m_zSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonZ ) );

    texMat = new osg::TexMat();
    m_xSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos, texMat ) );
    m_xSlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    texMat = new osg::TexMat();
    m_ySlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 1.0, 0.0 ), m_yPos, texMat ) );
    m_ySlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    texMat = new osg::TexMat();
    m_zSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 0.0, 1.0 ), m_zPos, texMat ) );
    m_zSlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    // create a new geode containing the slices
//    m_xSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsY() * grid->getDirectionY(),
//                grid->getNbCoordsZ() * grid->getDirectionZ() ) );
//
//    m_ySlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
//                grid->getNbCoordsZ() * grid->getDirectionZ() ) );
//
//    m_zSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
//                grid->getNbCoordsY() * grid->getDirectionY() ) );

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
}

void WMProbTractDisplaySP::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_probIC->getDataChangedCondition() );
    m_moduleState.add( m_fibersIC->getDataChangedCondition() );

    ready();

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // main loop
    while ( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_probIC->handledUpdate() || m_fibersIC->handledUpdate();

        boost::shared_ptr< WDataSetFibers > fibers = m_fibersIC->getData();
        boost::shared_ptr< WDataSetScalar > probTract = m_probIC->getData();
        bool dataValid = fibers && probTract;

        if( dataValid && dataUpdated )
        {
            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( probTract->getGrid() );
            if( grid->getNbCoordsX() > 0 && grid->getNbCoordsY() > 0 &&  grid->getNbCoordsZ() > 0 )
            {
                m_xPos->setMax( grid->getNbCoordsX() - 1 );
                m_xPos->set( ( grid->getNbCoordsX() - 1 ) / 2, true );
                m_yPos->setMax( grid->getNbCoordsY() - 1 );
                m_yPos->set( ( grid->getNbCoordsY() - 1 ) / 2, true );
                m_zPos->setMax( grid->getNbCoordsZ() - 1 );
                m_zPos->set( ( grid->getNbCoordsZ() - 1 ) / 2, true );

                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
                initOSG( grid );
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
            }
            else
            {
                warnLog() << "Invalid dataset: at least one dimension is not greater zero.";
                continue;
            }

        }

        infoLog() << "Done";
    }

    // WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
}

