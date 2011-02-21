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

#include <cmath>
#include <string>
#include <utility>

#include "../../kernel/WKernel.h"

#include "../../common/WBoundingBox.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D_2.h"
#include "../../dataHandler/WSubject.h"
#include "WMDatasetManipulator.xpm"

#include "WMDatasetManipulator.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMDatasetManipulator )

WMDatasetManipulator::WMDatasetManipulator():
    WModule()
{
}

WMDatasetManipulator::~WMDatasetManipulator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMDatasetManipulator::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMDatasetManipulator() );
}

const char** WMDatasetManipulator::getXPMIcon() const
{
    return WMDatasetManipulator_xpm; // Please put a real icon here.
}
const std::string WMDatasetManipulator::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "DatasetManipulator";
}

const std::string WMDatasetManipulator::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "This module allows manipulation of the dataset properties via manipulators in the 3D scene";
}

void WMDatasetManipulator::connectors()
{
    // the input dataset is just used as source for resolurtion and transformation matrix
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetSingle  > >(
        new WModuleOutputData< WDataSetSingle >( shared_from_this(), "out", "The modified dataset." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMDatasetManipulator::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_showManipulators = m_properties->addProperty( "Show manipulators", "Hide/Show manipulators.", true, m_propCondition );
    m_rotationMode = m_properties->addProperty( "Rotation mode", "Toggles rotation mode", false, m_propCondition );

    m_groupTexManip = m_properties->addPropertyGroup( "Texture Manipulation",  "Properties only related to the texture manipulation." );

    m_translationX = m_groupTexManip->addProperty( "X translation", "", 0, m_propCondition );
    m_translationX->setMax( 500 );
    m_translationX->setMin( -500 );
    m_translationY = m_groupTexManip->addProperty( "Y translation", "", 0, m_propCondition );
    m_translationY->setMax( 500 );
    m_translationY->setMin( -500 );
    m_translationZ = m_groupTexManip->addProperty( "Z translation", "", 0, m_propCondition );
    m_translationZ->setMax( 500 );
    m_translationZ->setMin( -500 );

    m_stretchX = m_groupTexManip->addProperty( "Voxel size X", "", 1.0, m_propCondition );
    m_stretchX->setMax( 10. );
    m_stretchX->setMin( -10. );
    m_stretchY = m_groupTexManip->addProperty( "Voxel size Y", "", 1.0, m_propCondition );
    m_stretchY->setMax( 10. );
    m_stretchY->setMin( -10. );
    m_stretchZ = m_groupTexManip->addProperty( "Voxel size Z", "", 1.0, m_propCondition );
    m_stretchZ->setMax( 10. );
    m_stretchZ->setMin( -10. );

    m_rotationX = m_groupTexManip->addProperty( "X rotation", "", 0, m_propCondition );
    m_rotationX->setMax( 180 );
    m_rotationX->setMin( -180 );
    m_rotationY = m_groupTexManip->addProperty( "Y rotation", "", 0, m_propCondition );
    m_rotationY->setMax( 180 );
    m_rotationY->setMin( -180 );
    m_rotationZ = m_groupTexManip->addProperty( "Z rotation", "", 0, m_propCondition );
    m_rotationZ->setMax( 180 );
    m_rotationZ->setMin( -180 );
    // TODO(schurade): activate this
    m_rotationX->setHidden( true );
    m_rotationY->setHidden( true );
    m_rotationZ->setHidden( true );

    WModule::properties();
}

void WMDatasetManipulator::init()
{
    m_grid = m_input->getData()->getTexture2()->getGrid();
    WBoundingBox bb = m_grid->getBoundingBox();

    wmath::WPosition center = bb.center();

    m_knobCenter = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knobx1 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knobx2 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knoby1 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knoby2 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knobz1 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knobz2 = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );

    m_knobRotCenter = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );
    m_knobRot = boost::shared_ptr<WROISphere>( new WROISphere( center, 2.5 ) );

    setManipulatorsFromBoundingBox();

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobCenter ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobx1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobx2 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knoby1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knoby2 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobz1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobz2 ) );

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobRotCenter ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_knobRot ) );

    using boost::function;
    m_changeRoiSignal
        = boost::shared_ptr< function< void() > >( new function< void() >( boost::bind( &WMDatasetManipulator::manipulatorMoved, this ) ) );
    m_knobCenter->addROIChangeNotifier( m_changeRoiSignal );
    m_knobx1->addROIChangeNotifier( m_changeRoiSignal );
    m_knobx2->addROIChangeNotifier( m_changeRoiSignal );
    m_knoby1->addROIChangeNotifier( m_changeRoiSignal );
    m_knoby2->addROIChangeNotifier( m_changeRoiSignal );
    m_knobz1->addROIChangeNotifier( m_changeRoiSignal );
    m_knobz2->addROIChangeNotifier( m_changeRoiSignal );

    m_changeRotRoiSignal
        = boost::shared_ptr< function< void() > >( new function< void() >( boost::bind( &WMDatasetManipulator::manipulatorRotMoved, this ) ) );
    m_knobRotCenter->addROIChangeNotifier( m_changeRotRoiSignal );
    m_knobRot->addROIChangeNotifier( m_changeRotRoiSignal );

    setManipulatorMode();
}

void WMDatasetManipulator::setManipulatorsFromBoundingBox()
{
    WBoundingBox bb = m_grid->getBoundingBox();

    m_posCenter = bb.center();

    m_posx1 = wmath::WPosition( bb.xMin(), m_posCenter[1], m_posCenter[2] );
    m_posx2 = wmath::WPosition( bb.xMax(), m_posCenter[1], m_posCenter[2] );
    m_posy1 = wmath::WPosition( m_posCenter[0], bb.yMin(), m_posCenter[2] );
    m_posy2 = wmath::WPosition( m_posCenter[0], bb.yMax(), m_posCenter[2] );
    m_posz1 = wmath::WPosition( m_posCenter[0], m_posCenter[1], bb.zMin() );
    m_posz2 = wmath::WPosition( m_posCenter[0], m_posCenter[1], bb.zMax() );

    m_knobCenter->setPosition( m_posCenter );
    m_knobx1->setPosition( m_posx1 );
    m_knobx2->setPosition( m_posx2 );
    m_knoby1->setPosition( m_posy1 );
    m_knoby2->setPosition( m_posy2 );
    m_knobz1->setPosition( m_posz1 );
    m_knobz2->setPosition( m_posz2 );

    m_posCenterOrig = m_posCenter;
    m_posx1Orig = m_posx1;
    m_posx2Orig = m_posx2;
    m_posy1Orig = m_posy1;
    m_posy2Orig = m_posy2;
    m_posz1Orig = m_posz1;
    m_posz2Orig = m_posz2;

    m_posRotCenter = m_posCenter;
    m_posRot = wmath::WPosition( ( m_posCenter.x() + ( fabs( m_posCenter.x() - m_posx1.x() ) / 2.0 ) ), m_posx1.y(), m_posx1.z() );
    m_posRotOrig = m_posRot;

    m_knobRotCenter->setPosition( m_posRotCenter );
    m_knobRot->setPosition( m_posRot );
}

void WMDatasetManipulator::setManipulatorMode()
{
    m_knobx1->setLockX( false );
    m_knobx1->setLockY( true );
    m_knobx1->setLockZ( true );
    m_knobx2->setLockX( false );
    m_knobx2->setLockY( true );
    m_knobx2->setLockZ( true );

    m_knoby1->setLockY( false );
    m_knoby1->setLockX( true );
    m_knoby1->setLockZ( true );
    m_knoby2->setLockY( false );
    m_knoby2->setLockX( true );
    m_knoby2->setLockZ( true );

    m_knobz1->setLockZ( false );
    m_knobz1->setLockY( true );
    m_knobz1->setLockX( true );
    m_knobz2->setLockZ( false );
    m_knobz2->setLockY( true );
    m_knobz2->setLockX( true );

    if ( m_rotationMode->get( true ) )
    {
        m_knobCenter->hide();
        m_knobx1->hide();
        m_knobx2->hide();
        m_knoby1->hide();
        m_knoby2->hide();
        m_knobz1->hide();
        m_knobz2->hide();

        m_knobRotCenter->unhide();
        m_knobRot->unhide();
    }
    else
    {
        m_knobCenter->unhide();
        m_knobx1->unhide();
        m_knobx2->unhide();
        m_knoby1->unhide();
        m_knoby2->unhide();
        m_knobz1->unhide();
        m_knobz2->unhide();

        m_knobRotCenter->hide();
        m_knobRot->hide();
    }
}

void WMDatasetManipulator::manipulatorMoved()
{
    wmath::WPosition newOffset( m_knobCenter->getPosition() - m_posCenter );
    m_knobx1->setPosition( m_knobx1->getPosition() + newOffset );
    m_knobx2->setPosition( m_knobx2->getPosition() + newOffset );
    m_knoby1->setPosition( m_knoby1->getPosition() + newOffset );
    m_knoby2->setPosition( m_knoby2->getPosition() + newOffset );
    m_knobz1->setPosition( m_knobz1->getPosition() + newOffset );
    m_knobz2->setPosition( m_knobz2->getPosition() + newOffset );

    wmath::WPosition stretch( 1.0, 1.0, 1.0 );

    float orgsizex = static_cast<float>( ( m_posx2Orig - m_posx1Orig ).x() );
    float orgsizey = static_cast<float>( ( m_posy2Orig - m_posy1Orig ).y() );
    float orgsizez = static_cast<float>( ( m_posz2Orig - m_posz1Orig ).z() );

    m_grid->translate( wmath::WPosition( m_grid->getTranslate().x() + ( m_knobx1->getPosition().x() - m_posx1.x() ) *
                                            ( static_cast<float>( m_grid->getNbCoordsX() / orgsizex ) ),
                                         m_grid->getTranslate().y() + ( m_knoby1->getPosition().y() - m_posy1.y() ) *
                                            ( static_cast<float>( m_grid->getNbCoordsY() / orgsizey ) ),
                                         m_grid->getTranslate().z() + ( m_knobz1->getPosition().z() - m_posz1.z() ) *
                                            ( static_cast<float>( m_grid->getNbCoordsZ() / orgsizez ) ) ) );
    m_translationX->set( m_grid->getTranslate().x(), true );
    m_translationY->set( m_grid->getTranslate().y(), true );
    m_translationZ->set( m_grid->getTranslate().z(), true );

    stretch.x() = ( m_knobx2->getPosition().x() - m_knobx1->getPosition().x() ) / orgsizex;
    stretch.y() = ( m_knoby2->getPosition().y() - m_knoby1->getPosition().y() ) / orgsizey;
    stretch.z() = ( m_knobz2->getPosition().z() - m_knobz1->getPosition().z() ) / orgsizez;

    m_grid->stretch( stretch );

    m_stretchX->set( stretch.x(), true );
    m_stretchY->set( stretch.y(), true );
    m_stretchZ->set( stretch.z(), true );

    adjustManipulatorPositions();

    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
}

void WMDatasetManipulator::manipulatorRotMoved()
{
    wmath::WPosition newOffset( m_knobRotCenter->getPosition() - m_posRotCenter );
    m_knobRot->setPosition( m_knobRot->getPosition() + newOffset );

    wmath::WPosition p1 = ( m_posRotCenter - m_posRot );

    m_posRotCenter = m_knobRotCenter->getPosition();
    m_posRot = m_knobRot->getPosition();

    wmath::WPosition p2 = ( m_posRotCenter - m_posRot );

    osg::Matrixf rot;
    rot.makeRotate( p2, p1 );

    m_grid->rotate( rot, m_posRotCenter );

    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
}

void WMDatasetManipulator::adjustManipulatorPositions()
{
    float cx = m_knobx1->getPosition().x() + ( ( m_knobx2->getPosition().x() - m_knobx1->getPosition().x() ) / 2.0 );
    float cy = m_knoby1->getPosition().y() + ( ( m_knoby2->getPosition().y() - m_knoby1->getPosition().y() ) / 2.0 );
    float cz = m_knobz1->getPosition().z() + ( ( m_knobz2->getPosition().z() - m_knobz1->getPosition().z() ) / 2.0 );

    m_knobCenter->setPosition( wmath::WPosition( cx, cy, cz ) );
    m_knobx1->setY( cy );
    m_knobx2->setY( cy );
    m_knobx1->setZ( cz );
    m_knobx2->setZ( cz );

    m_knoby1->setX( cx );
    m_knoby2->setX( cx );
    m_knoby1->setZ( cz );
    m_knoby2->setZ( cz );

    m_knobz1->setY( cy );
    m_knobz2->setY( cy );
    m_knobz1->setX( cx );
    m_knobz2->setX( cx );

    m_posCenter = m_knobCenter->getPosition();
    m_posx1 = m_knobx1->getPosition();
    m_posx2 = m_knobx2->getPosition();
    m_posy1 = m_knoby1->getPosition();
    m_posy2 = m_knoby2->getPosition();
    m_posz1 = m_knobz1->getPosition();
    m_posz2 = m_knobz2->getPosition();
}

void WMDatasetManipulator::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataValid )
        {
            if( dataChanged )
            {
                m_dataSet = newDataSet;
                init();
                break;
            }
        }
    }

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if ( m_active->changed() || m_showManipulators->changed() )
        {
            if ( m_active->get( true ) && m_showManipulators->get( true ) )
            {
                setManipulatorMode();
            }
            else
            {
                m_knobCenter->hide();
                m_knobx1->hide();
                m_knobx2->hide();
                m_knoby1->hide();
                m_knoby2->hide();
                m_knobz1->hide();
                m_knobz2->hide();
                m_knobRotCenter->hide();
                m_knobRot->hide();
            }
        }

        if ( m_rotationMode->changed() )
        {
            setManipulatorMode();
        }

        if ( m_translationX->changed() || m_translationY->changed() || m_translationZ->changed() )
        {
            wmath::WPosition pos( m_translationX->get( true ), m_translationY->get( true ), m_translationZ->get( true ) );
            m_grid->translate( pos );
            setManipulatorsFromBoundingBox();
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
        }
        if ( m_stretchX->changed() || m_stretchY->changed() || m_stretchZ->changed() )
        {
            wmath::WPosition str( m_stretchX->get( true ), m_stretchY->get( true ), m_stretchZ->get( true ) );
            m_grid->stretch( str );
            setManipulatorsFromBoundingBox();
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
        }
        if ( m_rotationX->changed() || m_rotationY->changed() || m_rotationZ->changed() )
        {
            float pi = 3.14159265;
            float rotx = static_cast<float>( m_rotationX->get( true ) ) / 180. * pi;
            float roty = static_cast<float>( m_rotationY->get( true ) ) / 180. * pi;
            float rotz = static_cast<float>( m_rotationZ->get( true ) ) / 180. * pi;

            wmath::WPosition rot( rotx, roty, rotz );
            //grid->rotate( rot );
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
        }
    }

    m_knobCenter->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobx1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobx2->removeROIChangeNotifier( m_changeRoiSignal );
    m_knoby1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knoby2->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobz1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobz2->removeROIChangeNotifier( m_changeRoiSignal );

    m_knobRotCenter->removeROIChangeNotifier( m_changeRotRoiSignal );
    m_knobRot->removeROIChangeNotifier( m_changeRotRoiSignal );
}

