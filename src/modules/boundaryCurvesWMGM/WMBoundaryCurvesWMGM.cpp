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

#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModuleInputData.h"
#include "WBoundaryBuilder.h"
#include "WBoundaryFragments.h"
#include "WBoundaryLines.h"
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
    m_propCondition  = boost::shared_ptr< WCondition >( new WCondition() );

    WPropGroup sliceGroup = m_properties->addPropertyGroup( "Slices",  "Group of slices for the GM and WM curves." );

    m_showSlice[0]   = sliceGroup->addProperty( "Show Sagittal", "Show GM and WM boundary curves on sagittal slice.", true );
    m_showSlice[1]   = sliceGroup->addProperty( "Show Coronal", "Show GM and WM boundary curves on coronal slice.", true );
    m_showSlice[2]   = sliceGroup->addProperty( "Show Axial", "Show GM and WM boundary curves on axial slice.", true );

    // The slice positions. These get update externally.
    // TODO(all): this should somehow be connected to the nav slices.
    m_slicePos[0]    = sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 0 );
    m_slicePos[1]    = sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0 );
    m_slicePos[2]    = sliceGroup->addProperty( "Axial Position", "Slice Z position.", 0 );

    // since we don't know anything yet => make them unusable
    for( char i = 0; i < 3; ++i )
    {
        m_slicePos[i]->setMin( 0 );
        m_slicePos[i]->setMax( 0 );
    }

    WPropDouble grayMatter  = m_properties->addProperty( "Gray Matter", "Isovalue specifying the Gray-Matter-CSF border.", 0.2 );
    grayMatter->setMin( 0.0 );
    grayMatter->setMax( 1.0 );
    WPropDouble whiteMatter = m_properties->addProperty( "White Matter", "Isovalue specifying the White-Matter-Gray-Matter border.", 0.4 );
    whiteMatter->setMin( 0.0 );
    whiteMatter->setMax( 1.0 );

    // for selecting the strategy
    boost::shared_ptr< WItemSelection > strategies( new WItemSelection() );
    strategies->addItem( "Iso-Lines", "Marching lines" );
    strategies->addItem( "Iso-Fragments", "Small environment" );
    m_strategySelector = m_properties->addProperty( "Strategy", "How the boundary curves should be draw",
            strategies->getSelectorFirst(), m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_strategySelector );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_strategySelector );

    // call WModule's initialization
    WModule::properties();
}

void WMBoundaryCurvesWMGM::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_textureIC->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // create the root node for all thing to paint
    m_output = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    while ( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetScalar > texture = m_textureIC->getData();

        if( !texture ) // invalid data
        {
            m_output->clear();
            continue;
        }

        boost::shared_ptr< WBoundaryBuilder > builder;

        // get grid and prepare OSG
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( texture->getGrid() );
        if( grid->getNbCoordsX() > 0 && grid->getNbCoordsY() > 0 &&  grid->getNbCoordsZ() > 0 )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
            m_output->clear();

            for( char i = 0; i < 3; ++i )
            {
                m_slicePos[i]->setMax( getNbCoords( grid )[i] - 1 );
                m_slicePos[i]->set( ( getNbCoords( grid )[i] - 1 ) / 2, true );
                m_slices[i] = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showSlice[i] ) );
            }

            std::string strategyName = m_strategySelector->get( true ).at( 0 )->getName();
            if( strategyName == "Iso-Lines" )
            {
                infoLog() << "Using: " << strategyName << " strategy";
                builder = boost::shared_ptr< WBoundaryBuilder >( new WBoundaryLines( texture, m_properties, &m_slices ) );
            }
            else if( strategyName == "Iso-Fragments" )
            {
                infoLog() << "Using: " << strategyName << " strategy";
                builder = boost::shared_ptr< WBoundaryBuilder >( new WBoundaryFragments( texture, m_properties, &m_slices ) );
            }
            else
            {
                errorLog() << "Skipping invalid strategy: " << strategyName;
                continue;
            }
            builder->run( m_output );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
        }
        else
        {
            warnLog() << "Skipping invalid dataset: at least one dimension is not greater zero";
        }
    }

     WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}
