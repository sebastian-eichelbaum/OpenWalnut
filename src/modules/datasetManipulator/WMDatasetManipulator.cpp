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

#include "core/kernel/WKernel.h"

#include "core/common/WBoundingBox.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/dataHandler/WSubject.h"
#include "WMDatasetManipulator.xpm"

#include "WMDatasetManipulator.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMDatasetManipulator )

WMDatasetManipulator::WMDatasetManipulator():
    WModule(),
    m_updated( false ),
    m_updateMutex()
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
    return "Dataset Manipulator";
}

const std::string WMDatasetManipulator::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "This module allows manipulation of the dataset properties via manipulators in the 3D scene.";
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
    m_transformChangedCondition = boost::shared_ptr< WCondition >( new WCondition() );

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
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_input->getData()->getGrid() );
    WAssert( m_grid, "The grid needs to be a regular 3D grid." );
    m_transform = boost::shared_ptr< WGridTransformOrtho >( new WGridTransformOrtho( m_grid->getTransform() ) );

    WBoundingBox bb = m_grid->getBoundingBox();

    WPosition center = bb.center();

    m_knobCenter = new WROISphere( center, 2.5 );
    m_knobx1 = new WROISphere( center, 2.5 );
    m_knobx2 = new WROISphere( center, 2.5 );
    m_knoby1 = new WROISphere( center, 2.5 );
    m_knoby2 = new WROISphere( center, 2.5 );
    m_knobz1 = new WROISphere( center, 2.5 );
    m_knobz2 = new WROISphere( center, 2.5 );

    m_knobRotCenter = new WROISphere( center, 2.5 );
    m_knobRot = new WROISphere( center, 2.5 );

    setManipulatorsFromBoundingBox();

    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobCenter );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobx1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobx2 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knoby1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knoby2 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobz1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobz2 );

    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobRotCenter );
    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_knobRot );

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

void WMDatasetManipulator::shutdown()
{
    // remove the notifier
    m_knobCenter->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobx1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobx2->removeROIChangeNotifier( m_changeRoiSignal );
    m_knoby1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knoby2->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobz1->removeROIChangeNotifier( m_changeRoiSignal );
    m_knobz2->removeROIChangeNotifier( m_changeRoiSignal );

    m_knobRotCenter->removeROIChangeNotifier( m_changeRotRoiSignal );
    m_knobRot->removeROIChangeNotifier( m_changeRotRoiSignal );

    // remove knobs from OSG
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobCenter );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobx1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobx2 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knoby1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knoby2 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobz1 );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobz2 );

    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobRotCenter );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_knobRot );
}

void WMDatasetManipulator::setManipulatorsFromBoundingBox()
{
    WGridRegular3D g( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), *m_transform );
    WBoundingBox bb = g.getBoundingBox();

    m_posCenter = bb.center();

    m_posx1 = WPosition( bb.xMin(), m_posCenter[1], m_posCenter[2] );
    m_posx2 = WPosition( bb.xMax(), m_posCenter[1], m_posCenter[2] );
    m_posy1 = WPosition( m_posCenter[0], bb.yMin(), m_posCenter[2] );
    m_posy2 = WPosition( m_posCenter[0], bb.yMax(), m_posCenter[2] );
    m_posz1 = WPosition( m_posCenter[0], m_posCenter[1], bb.zMin() );
    m_posz2 = WPosition( m_posCenter[0], m_posCenter[1], bb.zMax() );

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
    m_posRot = WPosition( ( m_posCenter.x() + ( fabs( m_posCenter.x() - m_posx1.x() ) / 2.0 ) ), m_posx1.y(), m_posx1.z() );
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

    if( m_rotationMode->get( true ) )
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
    WPosition newOffset( m_knobCenter->getPosition() - m_posCenter );
    m_knobx1->setPosition( m_knobx1->getPosition() + newOffset );
    m_knobx2->setPosition( m_knobx2->getPosition() + newOffset );
    m_knoby1->setPosition( m_knoby1->getPosition() + newOffset );
    m_knoby2->setPosition( m_knoby2->getPosition() + newOffset );
    m_knobz1->setPosition( m_knobz1->getPosition() + newOffset );
    m_knobz2->setPosition( m_knobz2->getPosition() + newOffset );

    WPosition stretch( 1.0, 1.0, 1.0 );

    // write changes to the transformation stored in the module using a write lock
    // open a new scope for the write lock
    {
        boost::unique_lock< boost::mutex > lock( m_updateMutex );

        float orgsizex = static_cast<float>( ( m_posx2Orig - m_posx1Orig ).x() );
        float orgsizey = static_cast<float>( ( m_posy2Orig - m_posy1Orig ).y() );
        float orgsizez = static_cast<float>( ( m_posz2Orig - m_posz1Orig ).z() );

        WPosition translate( ( m_knobx1->getPosition().x() - m_posx1.x() ) *
                             ( static_cast<float>( m_grid->getNbCoordsX() / orgsizex ) ),
                             ( m_knoby1->getPosition().y() - m_posy1.y() ) *
                             ( static_cast<float>( m_grid->getNbCoordsY() / orgsizey ) ),
                             ( m_knobz1->getPosition().z() - m_posz1.z() ) *
                             ( static_cast<float>( m_grid->getNbCoordsZ() / orgsizez ) ) );

        m_translationX->set( translate[ 0 ], true );
        m_translationY->set( translate[ 1 ], true );
        m_translationZ->set( translate[ 2 ], true );

        stretch.x() = ( m_knobx2->getPosition().x() - m_knobx1->getPosition().x() ) / orgsizex;
        stretch.y() = ( m_knoby2->getPosition().y() - m_knoby1->getPosition().y() ) / orgsizey;
        stretch.z() = ( m_knobz2->getPosition().z() - m_knobz1->getPosition().z() ) / orgsizez;

        m_stretchX->set( stretch.x(), true );
        m_stretchY->set( stretch.y(), true );
        m_stretchZ->set( stretch.z(), true );

        adjustManipulatorPositions();

        m_transform->translate( translate );
        m_transform->scale( stretch );
    } // write lock goes out of scope and is released

    // notify the module of changes to the transformation,
    // so it can do a thread-safe update of the grid
    notifyChanged();
}

void WMDatasetManipulator::manipulatorRotMoved()
{
    // write changes to the transformation stored in the module using a write lock
    // open a new scope for the write lock
    {
        boost::unique_lock< boost::mutex > lock( m_updateMutex );

        WPosition newOffset( m_knobRotCenter->getPosition() - m_posRotCenter );
        m_knobRot->setPosition( m_knobRot->getPosition() + newOffset );

        WPosition p1 = ( m_posRotCenter - m_posRot );

        m_posRotCenter = m_knobRotCenter->getPosition();
        m_posRot = m_knobRot->getPosition();

        WPosition p2 = ( m_posRotCenter - m_posRot );

        osg::Matrixf rot;
        rot.makeRotate( p2.as< osg::Vec3f >(), p1.as< osg::Vec3f >() );

        // m_transform->rotate( rot, m_posRotCenter );
    } // write lock goes out of scope and is released

    // notify the module of changes to the transformation,
    // so it can do a thread-safe update of the grid
    notifyChanged();
}

void WMDatasetManipulator::adjustManipulatorPositions()
{
    float cx = m_knobx1->getPosition().x() + ( ( m_knobx2->getPosition().x() - m_knobx1->getPosition().x() ) / 2.0 );
    float cy = m_knoby1->getPosition().y() + ( ( m_knoby2->getPosition().y() - m_knoby1->getPosition().y() ) / 2.0 );
    float cz = m_knobz1->getPosition().z() + ( ( m_knobz2->getPosition().z() - m_knobz1->getPosition().z() ) / 2.0 );

    m_knobCenter->setPosition( WPosition( cx, cy, cz ) );
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

void WMDatasetManipulator::notifyChanged()
{
    boost::unique_lock< boost::mutex > lock( m_updateMutex );
    m_updated = true;
    m_transformChangedCondition->notify();
}

void WMDatasetManipulator::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_transformChangedCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        // test if any updates were made to the transformation
        {
            boost::unique_lock< boost::mutex > lock( m_updateMutex );
            if( m_updated )
            {
                m_updated = false;
                boost::shared_ptr< WGrid > newGrid( new WGridRegular3D( m_grid->getNbCoordsX(),
                                                                        m_grid->getNbCoordsY(),
                                                                        m_grid->getNbCoordsZ(),
                                                                        *m_transform ) );
                // NOTE: we NEED to use clone here as we need to keep the dynamic type of the input data set.
                // This means: clone a WDataSetSingle which was constructed as WDataSetVector -> result is a WDataSetSingle which was constructed
                // as WDataSetVector too.
                m_output->updateData( m_dataSet->clone( newGrid ) );
            }

            boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
            bool dataChanged = ( m_dataSet != newDataSet );
            bool dataValid   = ( newDataSet );

            if( dataValid )
            {
                if( dataChanged )
                {
                    {
                        m_dataSet = newDataSet;
                        init();
                        debugLog() << "New dataset on input connector.";
                    }
                }
            }
        }

        if( m_active->changed() || m_showManipulators->changed() )
        {
            if( m_active->get( true ) && m_showManipulators->get( true ) )
            {
                boost::unique_lock< boost::mutex > lock( m_updateMutex );
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

        if( m_rotationMode->changed() )
        {
            boost::unique_lock< boost::mutex > lock( m_updateMutex );
            setManipulatorMode();
        }

        if( m_translationX->changed() || m_translationY->changed() || m_translationZ->changed() )
        {
            {
                boost::unique_lock< boost::mutex > lock( m_updateMutex );
                WPosition pos( m_translationX->get( true ), m_translationY->get( true ), m_translationZ->get( true ) );
                m_transform->translate( pos );
                setManipulatorsFromBoundingBox();
            }
            notifyChanged();
        }
        if( m_stretchX->changed() || m_stretchY->changed() || m_stretchZ->changed() )
        {
            {
                boost::unique_lock< boost::mutex > lock( m_updateMutex );
                WPosition str( m_stretchX->get( true ), m_stretchY->get( true ), m_stretchZ->get( true ) );
                m_transform->scale( str );
                setManipulatorsFromBoundingBox();
            }
            notifyChanged();
        }
        if( m_rotationX->changed() || m_rotationY->changed() || m_rotationZ->changed() )
        {
            //float pi = 3.14159265;
            //float rotx = static_cast<float>( m_rotationX->get( true ) ) / 180. * pi;
            //float roty = static_cast<float>( m_rotationY->get( true ) ) / 180. * pi;
            //float rotz = static_cast<float>( m_rotationZ->get( true ) ) / 180. * pi;

            {
                //boost::unique_lock< boost::mutex > lock( m_updateMutex );
                // m_transform->rotateX( rotx );
                // m_transform->rotateY( rotx );
                // m_transform->rotateZ( rotx );
            }
            notifyChanged();
        }
    }

    // cleanup
    shutdown();
}

