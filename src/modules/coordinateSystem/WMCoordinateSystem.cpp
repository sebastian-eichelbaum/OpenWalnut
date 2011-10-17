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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>

#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/exceptions/WDHNoSuchSubject.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"
#include "WMCoordinateSystem.h"
#include "WMCoordinateSystem.xpm"
#include "WTalairachConverter.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMCoordinateSystem )

WMCoordinateSystem::WMCoordinateSystem() :
    WModule(), m_dirty( false ), m_drawOffset( 0.02 ),
    m_viewAngle( 0 )
{
}

WMCoordinateSystem::~WMCoordinateSystem()
{
}

boost::shared_ptr< WModule > WMCoordinateSystem::factory() const
{
    return boost::shared_ptr< WMCoordinateSystem >( new WMCoordinateSystem() );
}

const char** WMCoordinateSystem::getXPMIcon() const
{
    return coordinateSystem_xpm;
}

void WMCoordinateSystem::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData< WDataSetScalar > >( new WModuleInputData< WDataSetScalar > ( shared_from_this(), "in",
            "Dataset to create atlas surfaces from." ) );
    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMCoordinateSystem::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_dataSet != m_input->getData() )
        {
            // acquire data from the input connector
            m_dataSet = m_input->getData();

            if( m_dataSet )
            {
                findBoundingBox();
                createGeometry();
                m_dirty = true;
            }
        }
    }
    // clean up stuff
    // NOTE: ALWAYS remove your osg nodes!
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

const std::string WMCoordinateSystem::getName() const
{
    return "Coordinate System";
}

const std::string WMCoordinateSystem::getDescription() const
{
    return "This module displays coordinate systems as overlay withn the main 3D view.";
}

void WMCoordinateSystem::properties()
{
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMCoordinateSystem::propertyChanged, this );

    m_showAxial = m_properties->addProperty( "Show rulers on axial", "", true, propertyCallback );
    m_showCoronal = m_properties->addProperty( "Show rulers on coronal", "", false, propertyCallback );
    m_showSagittal = m_properties->addProperty( "Show rulers on sagittal", "", false, propertyCallback );

    m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_possibleSelections->addItem( "World", "" ); // NOTE: you can add XPM images here.
    m_possibleSelections->addItem( "Canonical", "" );
    m_possibleSelections->addItem( "Talairach", "" );

    m_csSelection = m_properties->addProperty( "Select coordinate system", "", m_possibleSelections->getSelectorFirst(), propertyCallback );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_csSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_csSelection );

    m_showGridAxial = m_properties->addProperty( "Show grid on axial", "", false, propertyCallback );
    m_showGridCoronal = m_properties->addProperty( "Show grid on coronal", "", false, propertyCallback );
    m_showGridSagittal = m_properties->addProperty( "Show grid on sagittal", "", false, propertyCallback );

    m_showNumbersOnRulers = m_properties->addProperty( "Show numbers on rulers", "Does exactly what it says", true, propertyCallback );

    WPosition ch = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    m_crosshair = m_properties->addProperty( "Crosshair", "", ch );
    // initialize the properties with a certain standard set
    // those properties will be updatet as soon as the first dataset is looaded
    m_flt = m_infoProperties->addProperty( "Front left top", "", WPosition( 0.0, 0.0, 0.0 ) );
    m_brb = m_infoProperties->addProperty( "Bottom right back", "", WPosition( 160.0, 200.0, 160.0 ) );

    m_ac = m_infoProperties->addProperty( "Anterior commisure", "", WPosition( 80.0, 119.0, 80.0 ) );
    m_pc = m_infoProperties->addProperty( "Posterior commisure", "", WPosition( 80.0, 80.0, 80.0 ) );
    m_ihp = m_infoProperties->addProperty( "Interhemispherical point", "", WPosition( 80.0, 119.0, 110.0 ) );

    m_acTrigger = m_properties->addProperty( "Set AC", "Press me.", WPVBaseTypes::PV_TRIGGER_READY, propertyCallback );
    m_pcTrigger = m_properties->addProperty( "Set PC", "Press me.", WPVBaseTypes::PV_TRIGGER_READY, propertyCallback );
    m_ihpTrigger = m_properties->addProperty( "Set IHP", "Press me.", WPVBaseTypes::PV_TRIGGER_READY, propertyCallback );

    WModule::properties();
}

void WMCoordinateSystem::propertyChanged()
{
    if( m_csSelection->changed() )
    {
        WItemSelector s = m_csSelection->get( true );
        infoLog() << "Selected " << s.at( 0 )->getName() << " coordinate system.";
        m_coordConverter->setCoordinateSystemMode( static_cast< coordinateSystemMode > ( s.getItemIndexOfSelected( 0 ) ) );
    }

    if( m_acTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_acTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        m_ac->set( m_crosshair->get() );
        initTalairachConverter();
        infoLog() << "Set new AC point.";
    }
    if( m_pcTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_pcTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        m_pc->set( m_crosshair->get() );
        initTalairachConverter();
        infoLog() << "Set new PC point.";
    }
    if( m_ihpTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_ihpTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        m_ihp->set( m_crosshair->get() );
        initTalairachConverter();
        infoLog() << "Set new IHP point.";
    }
    m_dirty = true;
}

void WMCoordinateSystem::activate()
{
    // Activate/Deactivate the coordinate system
    if( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    // Always call WModule's activate!
    WModule::activate();
}

void WMCoordinateSystem::createGeometry()
{
    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );

//    m_rootNode->insert( wge::generateBoundingBoxGeode( m_coordConverter->getBoundingBox().first, m_coordConverter->getBoundingBox().second, WColor(
//            0.3, 0.3, 0.3, 1 ) ) );

    m_rulerNode = osg::ref_ptr< osg::Group >( new osg::Group() );
    m_rootNode->insert( m_rulerNode );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMCoordinateSystem::updateCallback, this ) ) );

    if( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }
}

void WMCoordinateSystem::updateCallback()
{
    if( WKernel::getRunningKernel()->getSelectionManager()->getFrontSector() != m_viewAngle )
    {
        m_viewAngle = WKernel::getRunningKernel()->getSelectionManager()->getFrontSector();
        m_dirty = true;
    }

    WPosition ch = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    WPosition cho = m_crosshair->get();

    if( ch[0] != cho[0] || ch[1] != cho[1] || ch[2] != cho[2] || m_dirty )
    {
        m_crosshair->set( ch );
    }
    else
    {
        return;
    }

    // *******************************************************************************************************
    //    osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_boxNode->getDrawable( 0 ) );
    //    m_boxNode->replaceDrawable( old, createGeometryNode() );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    double xOff = grid->getOffsetX();
    double yOff = grid->getOffsetY();
    double zOff = grid->getOffsetZ();

    m_rulerNode->removeChildren( 0, m_rulerNode->getNumChildren() );

    if( m_showSagittal->get() )
    {
        addRulersSagittal();
    }
    if( m_showGridSagittal->get() )
    {
        addSagittalGrid( m_crosshair->get()[0] + 0.5 * xOff + m_drawOffset );
        addSagittalGrid( m_crosshair->get()[0] + 0.5 * xOff - m_drawOffset );
    }

    if( m_showCoronal->get() )
    {
        addRulersCoronal();
    }
    if( m_showGridCoronal->get() )
    {
        addCoronalGrid( m_crosshair->get()[1] + 0.5 * yOff + m_drawOffset );
        addCoronalGrid( m_crosshair->get()[1] + 0.5 * yOff - m_drawOffset );
    }

    if( m_showAxial->get() )
    {
        addRulersAxial();
    }
    if( m_showGridAxial->get() )
    {
        addAxialGrid( m_crosshair->get()[2] + 0.5 * zOff + m_drawOffset );
        addAxialGrid( m_crosshair->get()[2] + 0.5 * zOff - m_drawOffset );
    }

    // *******************************************************************************************************
    m_dirty = false;
}

void WMCoordinateSystem::findBoundingBox()
{
    // get bounding from dataset
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    double xOff = grid->getOffsetX();
    double yOff = grid->getOffsetY();
    double zOff = grid->getOffsetZ();

    WVector3d offset( xOff, yOff, zOff );
    m_coordConverter = boost::shared_ptr< WCoordConverter >( new WCoordConverter( grid->getTransformationMatrix(), grid->getOrigin(), offset ) );

    WItemSelector s = m_csSelection->get( true );
    m_coordConverter->setCoordinateSystemMode( static_cast< coordinateSystemMode > ( s.getItemIndexOfSelected( 0 ) ) );
    m_coordConverter->setBoundingBox( m_dataSet->getGrid()->getBoundingBox() );

    // now compute the innerBoundingBox for the actual data
    WPosition flt = m_flt->get();
    WPosition brb = m_brb->get();

    for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        int count = 0;
        for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            flt[0] = static_cast< float > ( x * xOff );
            break;
        }
    }
    for( int x = grid->getNbCoordsX() - 1; x > -1; --x )
    {
        int count = 0;
        for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            brb[0] = static_cast< float > ( x * xOff );
            break;
        }
    }

    for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
    {
        int count = 0;
        for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            flt[1] = static_cast< float > ( y * yOff );
            break;
        }
    }
    for( int y = grid->getNbCoordsY() - 1; y > -1; --y )
    {
        int count = 0;
        for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            brb[1] = static_cast< float > ( y * yOff );
            break;
        }
    }

    for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
    {
        int count = 0;
        for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
        {
            for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            flt[2] = static_cast< float > ( z * zOff );
            break;
        }
    }
    for( int z = grid->getNbCoordsZ() - 1; z > -1; --z )
    {
        int count = 0;
        for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
        {
            for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
            {
                double v = m_dataSet->getValueAt( x, y, z );
                if( v > 0 )
                {
                    ++count;
                }
            }
        }
        if( count > 5 )
        {
            brb[2] = static_cast< float > ( z * zOff );
            break;
        }
    }

    WPosition tmpflt = m_coordConverter->worldCoordTransformed( flt );
    WPosition tmpbrb = m_coordConverter->worldCoordTransformed( brb );

    flt[0] = std::min( tmpflt[0], tmpbrb[0] );
    flt[1] = std::max( tmpflt[1], tmpbrb[1] );
    flt[2] = std::max( tmpflt[2], tmpbrb[2] );
    brb[0] = std::max( tmpflt[0], tmpbrb[0] );
    brb[1] = std::min( tmpflt[1], tmpbrb[1] );
    brb[2] = std::min( tmpflt[2], tmpbrb[2] );

    m_flt->set( flt );
    m_brb->set( brb );

    initTalairachConverter();
}

void WMCoordinateSystem::initTalairachConverter()
{
    WVector3d ac_c( m_coordConverter->w2c( m_ac->get() ) );
    WVector3d pc_c( m_coordConverter->w2c( m_pc->get() ) );
    WVector3d ihp_c( m_coordConverter->w2c( m_ihp->get() ) );
    boost::shared_ptr< WTalairachConverter > talairachConverter =
        boost::shared_ptr< WTalairachConverter >( new WTalairachConverter( ac_c, pc_c, ihp_c ) );

    WVector3d flt_c( m_coordConverter->w2c( m_flt->get() ) );
    WVector3d brb_c( m_coordConverter->w2c( m_brb->get() ) );

    WVector3d ap( flt_c[0], 0., 0. );
    WVector3d pp( brb_c[0], 0., 0. );
    WVector3d lp( 0., flt_c[1], 0. );
    WVector3d rp( 0., brb_c[1], 0. );
    WVector3d sp( 0., 0., flt_c[2] );
    WVector3d ip( 0., 0., brb_c[2] );

    talairachConverter->setAp( ap );
    talairachConverter->setPp( pp );
    talairachConverter->setSp( sp );
    talairachConverter->setIp( ip );
    talairachConverter->setLp( lp );
    talairachConverter->setRp( rp );

    m_coordConverter->setTalairachConverter( talairachConverter );
}

void WMCoordinateSystem::addRulersSagittal()
{
    float offset = 0.0;
    if( m_viewAngle < 4 )
    {
        offset = -1.0;
    }
    else
    {
        offset = 1.0;
    }

    osg::ref_ptr< WRulerOrtho > ruler1 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0] + offset, m_crosshair->get()[1], m_crosshair->get()[2] ),
                    RULER_ALONG_Y_AXIS_SCALE_Z, m_showNumbersOnRulers->get() ) );
    osg::ref_ptr< WRulerOrtho > ruler2 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0] + offset, m_crosshair->get()[1], m_crosshair->get()[2] ),
                    RULER_ALONG_Z_AXIS_SCALE_Y, m_showNumbersOnRulers->get() ) );

    m_rulerNode->addChild( ruler1 );
    m_rulerNode->addChild( ruler2 );
}

void WMCoordinateSystem::addRulersAxial()
{
    float offset = 0.0;
    if( ( m_viewAngle == 1 ) || ( m_viewAngle == 2 ) || ( m_viewAngle == 5 ) || ( m_viewAngle == 6 ) )
    {
        offset = 1.0;
    }
    else
    {
        offset = -1.0;
    }

    osg::ref_ptr< WRulerOrtho > ruler1 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0], m_crosshair->get()[1], m_crosshair->get()[2] + offset ),
            RULER_ALONG_X_AXIS_SCALE_Y, m_showNumbersOnRulers->get() ) );
    osg::ref_ptr< WRulerOrtho > ruler2 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0], m_crosshair->get()[1], m_crosshair->get()[2] + offset ),
            RULER_ALONG_Y_AXIS_SCALE_X, m_showNumbersOnRulers->get() ) );

    m_rulerNode->addChild( ruler1 );
    m_rulerNode->addChild( ruler2 );
}

void WMCoordinateSystem::addRulersCoronal()
{
    float offset = 0.0;
    if( ( m_viewAngle == 2 ) || ( m_viewAngle == 3 ) || ( m_viewAngle == 4 ) || ( m_viewAngle == 5 ) )
    {
        offset = 1.0;
    }
    else
    {
        offset = - 1.0;
    }

    osg::ref_ptr< WRulerOrtho > ruler1 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0], m_crosshair->get()[1] + offset, m_crosshair->get()[2] ),
                    RULER_ALONG_X_AXIS_SCALE_Z, m_showNumbersOnRulers->get() ) );
    osg::ref_ptr< WRulerOrtho > ruler2 = osg::ref_ptr< WRulerOrtho >(
            new WRulerOrtho( m_coordConverter, osg::Vec3( m_crosshair->get()[0], m_crosshair->get()[1] + offset, m_crosshair->get()[2] ),
                    RULER_ALONG_Z_AXIS_SCALE_X, m_showNumbersOnRulers->get() ) );

    m_rulerNode->addChild( ruler1 );
    m_rulerNode->addChild( ruler2 );
}

void WMCoordinateSystem::addSagittalGrid( float position )
{
    if( m_viewAngle < 4 )
    {
        position -= 1.0;
    }
    else
    {
        position += 1.0;
    }

    osg::ref_ptr< osg::Geode > gridGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    WBoundingBox boundingBox = m_coordConverter->getBoundingBox();
    WPosition p1 = boundingBox.getMin();
    WPosition p2 = boundingBox.getMax();

    switch ( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
            for( int i = p1[1]; i < p2[1] + 2; ++i )
            {
                if( m_coordConverter->numberToCsY( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( position, i, p1[2] ) );
                    vertices->push_back( osg::Vec3( position, i, p2[2] + 1 ) );
                }
            }
            for( int i = p1[2]; i < p2[2] + 2; ++i )
            {
                if( m_coordConverter->numberToCsZ( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( position, p1[1], i ) );
                    vertices->push_back( osg::Vec3( position, p2[1] + 1, i ) );
                }
            }
            break;
        case CS_TALAIRACH:
        {
            boost::shared_ptr< WTalairachConverter > tc = m_coordConverter->getTalairachConverter();

            for( int i = -110; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( i, 0, -50 ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( i, 0, 80 ) );

                vertices->push_back( osg::Vec3( position, tmpPoint1[1], tmpPoint1[2] ) );
                vertices->push_back( osg::Vec3( position, tmpPoint2[1], tmpPoint2[2] ) );
            }
            for( int i = -50; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( -110, 0, i ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( 80, 0, i ) );

                vertices->push_back( osg::Vec3( position, tmpPoint1[1], tmpPoint1[2] ) );
                vertices->push_back( osg::Vec3( position, tmpPoint2[1], tmpPoint2[2] ) );
            }
        }
            break;
    }

    geometry->setVertexArray( vertices );
    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    for( size_t i = 0; i < vertices->size(); ++i )
    {
        lines->push_back( i );
    }
    geometry->addPrimitiveSet( lines );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    gridGeode->addDrawable( geometry );

    m_rulerNode->addChild( gridGeode );
}

void WMCoordinateSystem::addCoronalGrid( float position )
{
    if( ( m_viewAngle == 2 ) || ( m_viewAngle == 3 ) || ( m_viewAngle == 4 ) || ( m_viewAngle == 5 ) )
    {
        position += 1.0;
    }
    else
    {
        position -= 1.0;
    }

    osg::ref_ptr< osg::Geode > gridGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    WBoundingBox boundingBox = m_coordConverter->getBoundingBox();
    WPosition p1 = boundingBox.getMin();
    WPosition p2 = boundingBox.getMax();

    switch ( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
            for( int i = p1[0]; i < p2[0] + 2; ++i )
            {
                if( m_coordConverter->numberToCsX( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, position, p1[2] ) );
                    vertices->push_back( osg::Vec3( i, position, p2[2] + 1 ) );
                }
            }
            for( int i = p1[2]; i < p2[2] + 2; ++i )
            {
                if( m_coordConverter->numberToCsZ( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( p1[1], position, i ) );
                    vertices->push_back( osg::Vec3( p2[1] + 1, position, i ) );
                }
            }
            break;
        case CS_TALAIRACH:
        {
            boost::shared_ptr< WTalairachConverter > tc = m_coordConverter->getTalairachConverter();

            for( int i = -80; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( 0, i, -50 ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( 0, i, 80 ) );

                vertices->push_back( osg::Vec3( tmpPoint1[0], position, tmpPoint1[2] ) );
                vertices->push_back( osg::Vec3( tmpPoint2[0], position, tmpPoint2[2] ) );
            }
            for( int i = -50; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( 0, -80, i ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( 0, 80, i ) );

                vertices->push_back( osg::Vec3( tmpPoint1[0], position, tmpPoint1[2] ) );
                vertices->push_back( osg::Vec3( tmpPoint2[0], position, tmpPoint2[2] ) );
            }
        }
            break;
    }

    geometry->setVertexArray( vertices );
    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    for( size_t i = 0; i < vertices->size(); ++i )
    {
        lines->push_back( i );
    }
    geometry->addPrimitiveSet( lines );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    gridGeode->addDrawable( geometry );

    m_rulerNode->addChild( gridGeode );
}

void WMCoordinateSystem::addAxialGrid( float position )
{
    if( ( m_viewAngle == 1 ) || ( m_viewAngle == 2 ) || ( m_viewAngle == 5 ) || ( m_viewAngle == 6 ) )
    {
        position += 1.0;
    }
    else
    {
        position -= 1.0;
    }

    osg::ref_ptr< osg::Geode > gridGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    WBoundingBox boundingBox = m_coordConverter->getBoundingBox();
    WPosition p1 = boundingBox.getMin();
    WPosition p2 = boundingBox.getMax();

    switch ( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
            for( int i = p1[1]; i < p2[1] + 2; ++i )
            {
                if( m_coordConverter->numberToCsY( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( p1[0], i, position ) );
                    vertices->push_back( osg::Vec3( p2[0] + 1, i, position ) );
                }
            }
            for( int i = p1[0]; i < p2[0] + 2; ++i )
            {
                if( m_coordConverter->numberToCsZ( i ) % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, p1[1], position ) );
                    vertices->push_back( osg::Vec3( i, p2[1] + 1, position ) );
                }
            }
            break;
        case CS_TALAIRACH:
        {
            boost::shared_ptr< WTalairachConverter > tc = m_coordConverter->getTalairachConverter();

            for( int i = -80; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( -110, i, 0 ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( 80, i, 0 ) );

                vertices->push_back( osg::Vec3( tmpPoint1[0], tmpPoint1[1], position ) );
                vertices->push_back( osg::Vec3( tmpPoint2[0], tmpPoint2[1], position ) );
            }
            for( int i = -110; i < 81; i += 10 )
            {
                WVector3d tmpPoint1 = m_coordConverter->t2w( WVector3d( i, -80, 0 ) );
                WVector3d tmpPoint2 = m_coordConverter->t2w( WVector3d( i, 80, 0 ) );

                vertices->push_back( osg::Vec3( tmpPoint1[0], tmpPoint1[1], position ) );
                vertices->push_back( osg::Vec3( tmpPoint2[0], tmpPoint2[1], position ) );
            }
        }
            break;
    }

    geometry->setVertexArray( vertices );
    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    for( size_t i = 0; i < vertices->size(); ++i )
    {
        lines->push_back( i );
    }
    geometry->addPrimitiveSet( lines );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    gridGeode->addDrawable( geometry );

    m_rulerNode->addChild( gridGeode );
}
