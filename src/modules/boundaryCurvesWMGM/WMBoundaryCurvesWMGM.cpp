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

#include <boost/array.hpp>

#include <osg/Vec3>
#include <osg/Geode>

#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "../../graphicsEngine/shaders/WGEPropertyUniform.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../kernel/WKernel.h"
#include "WMBoundaryCurvesWMGM.h"
#include "WMBoundaryCurvesWMGM.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMBoundaryCurvesWMGM )

WMBoundaryCurvesWMGM::WMBoundaryCurvesWMGM():
    WModule()
{
}

WMBoundaryCurvesWMGM::~WMBoundaryCurvesWMGM()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMBoundaryCurvesWMGM::factory() const
{
    return boost::shared_ptr< WModule >( new WMBoundaryCurvesWMGM() );
}

const char** WMBoundaryCurvesWMGM::getXPMIcon() const
{
    return WMBoundaryCurvesWMGM_xpm;
}

const std::string WMBoundaryCurvesWMGM::getName() const
{
    return "Boundary Curves GMWM";
}

const std::string WMBoundaryCurvesWMGM::getDescription() const
{
    return "Boundary Curves of Gray Matter White Matter";
}

void WMBoundaryCurvesWMGM::connectors()
{
    m_textureIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "textureInput", "The image for the two curves." );

    // call WModule's initialization
    WModule::connectors();
}

void WMBoundaryCurvesWMGM::properties()
{
    m_sliceGroup      = m_properties->addPropertyGroup( "Slices",  "Group of slices for the GM and WM curves." );

    // enable slices
    m_showonX        = m_sliceGroup->addProperty( "Show Sagittal", "Show GM and WM boundary curves on sagittal slice.", true );
    m_showonY        = m_sliceGroup->addProperty( "Show Coronal", "Show GM and WM boundary curves on coronal slice.", true );
    m_showonZ        = m_sliceGroup->addProperty( "Show Axial", "Show GM and WM boundary curves on axial slice.", true );

    // The slice positions. These get update externally.
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

    m_grayMatter = m_properties->addProperty( "Gray Matter", "todo:", 0.2 );
    m_grayMatter->setMin( 0.0 );
    m_grayMatter->setMax( 1.0 );
    m_whiteMatter = m_properties->addProperty( "White Matter", "todo:", 0.4 );
    m_whiteMatter->setMin( 0.0 );
    m_whiteMatter->setMax( 1.0 );

    // call WModule's initialization
    WModule::properties();
}

void WMBoundaryCurvesWMGM::initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WDataSetScalar > dataset )
{
    // remove the old slices
    m_output->clear();

    // create all the transformation nodes
    m_xSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonX ) );
    m_ySlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonY ) );
    m_zSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonZ ) );

    // create a new geode containing the slices
    m_xSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsY() * grid->getDirectionY(),
                grid->getNbCoordsZ() * grid->getDirectionZ() ) );

    m_ySlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                grid->getNbCoordsZ() * grid->getDirectionZ() ) );

    m_zSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                grid->getNbCoordsY() * grid->getDirectionY() ) );

    // apply shaders
    m_shader->apply( m_xSlice );
    m_shader->apply( m_ySlice );
    m_shader->apply( m_zSlice );

    osg::ref_ptr< osg::Uniform > u_grayMatter = new WGEPropertyUniform< WPropDouble >( "u_grayMatter", m_grayMatter );
    osg::ref_ptr< osg::Uniform > u_whiteMatter = new WGEPropertyUniform< WPropDouble >( "u_whiteMatter", m_whiteMatter );
    wge::bindTexture( m_xSlice, dataset->getTexture()->getTexture() );
    wge::bindTexture( m_ySlice, dataset->getTexture()->getTexture() );
    wge::bindTexture( m_zSlice, dataset->getTexture()->getTexture() );

    // transform X,Y,Z dependent expressions into numbers dependent (0,1,2) to loop over them
    // NOTE: we can remove double curly braces when compiler is standard compliant according to:
    //       http://www.boost.org/doc/libs/1_42_0/doc/html/array/rationale.html
    boost::array< osg::StateSet*, 3 > ss = { { m_xSlice->getOrCreateStateSet(), m_ySlice->getOrCreateStateSet(), m_zSlice->getOrCreateStateSet() } }; // NOLINT curly braces
    boost::array< WPropInt, 3 > pos = { { m_xPos, m_yPos, m_zPos } }; // NOLINT curly braces
    boost::array< wmath::WVector3D, 3 > dir = { { grid->getDirectionX(), grid->getDirectionY(), grid->getDirectionZ() } }; // NOLINT curly braces

    for( int i = 0; i < 3; ++i )
    {
        ss.at( i )->addUniform( u_grayMatter );
        ss.at( i )->addUniform( u_whiteMatter );
        ss.at( i )->addUniform( new WGEPropertyUniform< WPropInt >( "u_vertexShift", pos.at( i ) ) );
        ss.at( i )->addUniform( new osg::Uniform( "u_vertexShiftDirection", dir.at( i ) ) );
    }

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
}

void WMBoundaryCurvesWMGM::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMBoundaryCurvesWMGM", m_localPath ) );

    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_textureIC->getDataChangedCondition() );

    ready();

    // create the root node for all thing to paint
    m_output = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

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
        bool dataUpdated = m_textureIC->handledUpdate();

        boost::shared_ptr< WDataSetScalar > texture = m_textureIC->getData();
        bool dataValid = texture;

        if( dataValid && dataUpdated )
        {
            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( texture->getGrid() );
            if( grid->getNbCoordsX() > 0 && grid->getNbCoordsY() > 0 &&  grid->getNbCoordsZ() > 0 )
            {
                m_xPos->setMax( grid->getNbCoordsX() - 1 );
                m_xPos->set( ( grid->getNbCoordsX() - 1 ) / 2, true );
                m_yPos->setMax( grid->getNbCoordsY() - 1 );
                m_yPos->set( ( grid->getNbCoordsY() - 1 ) / 2, true );
                m_zPos->setMax( grid->getNbCoordsZ() - 1 );
                m_zPos->set( ( grid->getNbCoordsZ() - 1 ) / 2, true );

                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
                initOSG( grid, texture );
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
            }
            else
            {
                warnLog() << "Invalid dataset: at least one dimension is not greater zero.";
                continue;
            }
        }
        if( !dataValid )
        {
            infoLog() << "Cleaning module: remove geodes etc...";
            m_output->clear();
        }

        infoLog() << "Done";
    }

     WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}
