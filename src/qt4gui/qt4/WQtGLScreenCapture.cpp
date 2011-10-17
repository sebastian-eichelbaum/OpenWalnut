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

#include <sstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <osgDB/WriteFile>

#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QGroupBox>

#include "WMainWindow.h"
#include "events/WEventTypes.h"
#include "core/common/WLogger.h"

#include "WQtGLScreenCapture.h"
#include "WQtGLScreenCapture.moc"

WQtGLScreenCapture::WQtGLScreenCapture( WGEViewer::SPtr viewer, WMainWindow* parent ):
    QDockWidget( "Animation", parent ),
    m_mainWindow( parent ),
    m_viewer( viewer ),
    m_iconManager( parent->getIconManager() )
{
    // initialize
    setObjectName( "Animation Dock" );

    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::AllDockWidgetFeatures );

    m_toolbox = new QToolBox( this );
    setWidget( m_toolbox );
    connect( m_toolbox, SIGNAL( currentChanged( int ) ), this, SLOT( toolBoxChanged( int ) ) );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // common config tools
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_configWidget = new QWidget();
    QVBoxLayout* configLayout = new QVBoxLayout();
    m_configWidget->setLayout( configLayout );

    // frame related stuff
    QGroupBox* frameCounterGroup = new QGroupBox( "Frame" );
    QVBoxLayout* frameCounterGroupLayout = new QVBoxLayout();
    frameCounterGroup->setLayout( frameCounterGroupLayout );
    m_configFrameLabel = new QLabel();
    m_configFrameLabel->setText( "Recorded Frames: 0" );
    m_configFrameResetButton = new QPushButton( "Reset" );
    connect( m_configFrameResetButton, SIGNAL( clicked( bool ) ), this, SLOT( resetFrames() ) );
    frameCounterGroupLayout->addWidget( m_configFrameLabel );
    frameCounterGroupLayout->addWidget( m_configFrameResetButton );

    QGroupBox* resolutionGroup = new QGroupBox( "Resolution" );
    QGridLayout* resolutionGroupLayout = new QGridLayout();
    resolutionGroup->setLayout( resolutionGroupLayout );

    m_resolutionCombo = new QComboBox();
    m_resolutionCombo->addItem( "640x480" );
    m_resolutionCombo->addItem( "800x600" );
    m_resolutionCombo->addItem( "850x650" );
    m_resolutionCombo->addItem( "1024x768" );
    m_resolutionCombo->addItem( "1280x1024" );
    m_resolutionCombo->addItem( "1330x1074" );
    m_resolutionCombo->addItem( "1280x720 (720p)" );
    m_resolutionCombo->addItem( "1920x1080 (1080p)" );

    QPushButton* resolutionButton = new QPushButton( "Set" );
    resolutionButton->setCheckable( true );
    resolutionGroupLayout->addWidget( m_resolutionCombo, 0, 0 );
    resolutionGroupLayout->addWidget( resolutionButton, 0, 1 );
    connect( resolutionButton, SIGNAL( toggled( bool ) ), this, SLOT( resolutionChange( bool ) ) );

    // filename config
    QGroupBox* fileGroup = new QGroupBox( "Output Files" );
    QVBoxLayout* fileGroupLayout = new QVBoxLayout();
    fileGroup->setLayout( fileGroupLayout );

    QLabel* configFileHint = new QLabel();
    configFileHint->setWordWrap( true );
    configFileHint->setText(
        "The filename used for all captured frames. This filename can contain some special tags which get replaced:"
                              "<ul>"
                              "<li> %f - the frame number"
                              "</ul>"
    );

    m_configFileEdit = new QLineEdit();
    std::string defaultFilename = ( QDir::homePath() + QDir::separator() + "OpenWalnut_Frame_%f.jpg" ).toStdString();
    m_configFileEdit->setText( QString::fromStdString( defaultFilename ) );

    fileGroupLayout->addWidget( configFileHint );
    fileGroupLayout->addWidget( m_configFileEdit );

    // add the groups
    configLayout->addWidget( resolutionGroup );
    configLayout->addWidget( frameCounterGroup );
    configLayout->addWidget( fileGroup );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // screenshot tools
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_screenshotWidget = new QWidget();
    QVBoxLayout* screenshotLayout = new QVBoxLayout();
    m_screenshotWidget->setLayout( screenshotLayout );

    m_screenshotButton = new QPushButton( "Screenshot" );
    m_screenshotButton->setToolTip( "Take a screenshot of the 3D view." );
    m_screenshotAction = new QAction( parent->getIconManager()->getIcon( "image" ), "Screenshot", this );
    m_screenshotAction->setToolTip( "Take a screenshot of the 3D view." );
    m_screenshotAction->setShortcut( QKeySequence(  Qt::Key_F12 ) );
    m_screenshotAction->setShortcutContext( Qt::ApplicationShortcut );
    connect( m_screenshotAction, SIGNAL(  triggered( bool ) ), this, SLOT( screenShot() ) );
    connect( m_screenshotButton, SIGNAL(  clicked( bool ) ), this, SLOT( screenShot() ) );

    QLabel* screenshotLabel = new QLabel();
    screenshotLabel->setWordWrap( true );
    screenshotLabel->setText( "Take a screenshot of the current scene with the current camera. You need to specify a filename." );
    screenshotLayout->addWidget( screenshotLabel );
    screenshotLayout->addWidget( m_screenshotButton );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // movie tools
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_movieWidget = new QWidget();
    QVBoxLayout* movieLayout = new QVBoxLayout();
    m_movieWidget->setLayout( movieLayout );

    QLabel* movieLabel = new QLabel();
    movieLabel->setWordWrap( true );
    movieLabel->setText( "Take a screenshot for every frame with the current camera. You need to specify a filename. "
                              "You should always add %f to your filename to differentiate between each frame. "
                              "This kind of recording can be very slow and produce a high IO load on your machine. Consider recording movies with "
                              "the animation tool which plays back a previously recorded scene and snapshot it frame-wise. "
                              "To create a movie with these images, you can use free encoders like ffmpeg, transcode or mencoder."
                              );
    movieLabel->setTextInteractionFlags( Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse );

    m_movieTimeLabel = new QLabel();
    m_movieTimeLabel->setText( "Recorded Movie-Time: 0s" );

    m_movieRecButton = new QPushButton( "Record" );
    connect( m_movieRecButton, SIGNAL(  clicked( bool ) ), this, SLOT( startRec() ) );

    m_movieStopButton = new QPushButton( "Stop" );
    m_movieStopButton->setDisabled( true );
    connect( m_movieStopButton, SIGNAL(  clicked( bool ) ), this, SLOT( stopRec() ) );

    movieLayout->addWidget( movieLabel );
    movieLayout->addWidget( m_movieTimeLabel );
    movieLayout->addWidget( m_movieRecButton );
    movieLayout->addWidget( m_movieStopButton );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // animation tools
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_animationWidget = new QWidget();
    QVBoxLayout* animationLayout = new QVBoxLayout();
    m_animationWidget->setLayout( animationLayout );


    QGroupBox* animationControlGroup = new QGroupBox( "Animation Control" );
    QGridLayout* animationControlGroupLayout = new QGridLayout();
    animationControlGroup->setLayout( animationControlGroupLayout );

    m_animationPlayButton = new QPushButton( "Play" );
    connect( m_animationPlayButton, SIGNAL(  clicked( bool ) ), this, SLOT( playAnim() ) );

    m_animationRecButton = new QPushButton( "Record" );
    connect( m_animationRecButton, SIGNAL(  clicked( bool ) ), this, SLOT( recAnim() ) );

    m_animationStopButton = new QPushButton( "Stop" );
    connect( m_animationStopButton, SIGNAL(  clicked( bool ) ), this, SLOT( stopAnim() ) );

    animationControlGroupLayout->addWidget( m_animationPlayButton, 1, 0 );
    animationControlGroupLayout->addWidget( m_animationRecButton, 1, 1 );
    animationControlGroupLayout->addWidget( m_animationStopButton, 1, 2 );

    QGroupBox* animationFileGroup = new QGroupBox( "Animation Script" );
    QVBoxLayout* animationFileGroupLayout = new QVBoxLayout();
    animationFileGroup->setLayout( animationFileGroupLayout );
    QLabel* animationFileLabel = new QLabel();
    animationFileLabel->setText( "Animation Script:" );
    m_animationFileEdit = new QLineEdit();
    m_animationFileEdit->setText( QDir::homePath() + QDir::separator() + "Animation.owanim" );

    animationFileGroupLayout->addWidget( animationFileLabel );
    animationFileGroupLayout->addWidget( m_animationFileEdit );

    // plug it into the layout
    animationLayout->addWidget( animationFileGroup );
    animationLayout->addWidget( animationControlGroup );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // plug it together
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this forces the elements to be initialized properly
    QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( WQT_SCREENCAPTURE_EVENT ) ) );

    // add them
    m_toolbox->insertItem( 0, m_configWidget, m_iconManager->getIcon( "preferences" ), "Configuration" );
    m_toolbox->insertItem( 1, m_screenshotWidget, m_iconManager->getIcon( "image" ), "Screenshot" );
    m_toolbox->insertItem( 2, m_movieWidget, m_iconManager->getIcon( "video" ), "Movie" );
    m_toolbox->insertItem( 3, m_animationWidget, m_iconManager->getIcon( "video" ), "Animation" );

    // we need to be notified about the screen grabbers state
    m_recordConnection = m_viewer->getScreenCapture()->getRecordCondition()->subscribeSignal( boost::bind( &WQtGLScreenCapture::recCallback, this ) );
    m_imageConnection = m_viewer->getScreenCapture()->subscribeSignal( boost::bind( &WQtGLScreenCapture::handleImage, this, _1, _2, _3 ) );
}

WQtGLScreenCapture::~WQtGLScreenCapture()
{
    // cleanup
    m_recordConnection.disconnect();
    m_imageConnection.disconnect();
}

QAction* WQtGLScreenCapture::getScreenshotTrigger() const
{
    return m_screenshotAction;
}

void WQtGLScreenCapture::handleImage( size_t /* framesLeft */, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const
{
    std::string filename = m_configFileEdit->text().toStdString();
    std::ostringstream ss;

    // this allows 999999 frames -> over 11h of movie material -> should be enough
    ss.setf( std::ios::right, std::ios::adjustfield );
    ss.setf( std::ios::fixed, std::ios::floatfield );
    ss.precision( 6 );
    ss.width( 6 );
    ss.fill( '0' );
    ss << totalFrames;

    size_t pos;
    while ( ( pos = filename.find( "%f" ) ) != std::string::npos )
    {
        filename.replace( pos, 2, ss.str() );
    }

    wlog::info( "WQtGLScreenCapture" ) << "Writing frame " << totalFrames << " to " << filename;
    osgDB::writeImageFile( *image, filename );
}

bool WQtGLScreenCapture::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if( event->type() == WQT_SCREENCAPTURE_EVENT )
    {
        // grab the needed info
        WGEScreenCapture::SharedRecordingInformation::ReadTicket r = m_viewer->getScreenCapture()->getRecordingInformation();
        size_t frame = r->get().m_frames;
        size_t framesLeft = r->get().m_framesLeft;
        r.reset();

        // generate some label texts
        std::ostringstream time;
        time.precision( 2 );
        time << "Recorded Movie-Time: " << static_cast< float >( frame ) / 24.0f;
        m_movieTimeLabel->setText( QString::fromStdString( time.str() ) );

        std::ostringstream frames;
        frames << "Recorded Frames: " << frame;
        m_configFrameLabel->setText( QString::fromStdString( frames.str() ) );

        // disable some elements if in recording mode
        if ( framesLeft == 0 )   // recording done:
        {
            m_screenshotWidget->setDisabled( false );
            m_configWidget->setDisabled( false );
            m_movieStopButton->setDisabled( true );
            m_movieRecButton->setDisabled( false );
        }
        else    // still recording
        {
            m_screenshotWidget->setDisabled( true );
            m_configWidget->setDisabled( true );
            m_movieStopButton->setDisabled( false );
            m_movieRecButton->setDisabled( true );
        }
    }

    return QDockWidget::event( event );
}

void WQtGLScreenCapture::screenShot()
{
    m_viewer->getScreenCapture()->screenshot();
}

void WQtGLScreenCapture::startRec()
{
    wlog::info( "WQtGLScreenCapture" ) << "Started recording.";
    m_viewer->getScreenCapture()->recordStart();
}

void WQtGLScreenCapture::stopRec()
{
    wlog::info( "WQtGLScreenCapture" ) << "Stopped recording.";
    m_viewer->getScreenCapture()->recordStop();
}

void WQtGLScreenCapture::resetFrames()
{
    wlog::debug( "WQtGLScreenCapture" ) << "Resetting frame-counter.";
    m_viewer->getScreenCapture()->resetFrameCounter();
}

void WQtGLScreenCapture::recCallback()
{
    QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( WQT_SCREENCAPTURE_EVENT ) ) );
}

void WQtGLScreenCapture::playAnim()
{
    wlog::debug( "WQtGLScreenCapture" ) << "Starting animation playback.";
}

void WQtGLScreenCapture::stopAnim()
{
    wlog::debug( "WQtGLScreenCapture" ) << "Stoping animation playback.";
    m_viewer->getScreenCapture()->recordStop();
    WGEAnimationManipulator::RefPtr anim = m_viewer->animationMode();
    anim->home( 0 );
}

void WQtGLScreenCapture::recAnim()
{
    wlog::info( "WQtGLScreenCapture" ) << "Starting animation record.";

    // NOTE: this needs some tuning. This is not really thread-safe
    WGEAnimationManipulator::RefPtr anim = m_viewer->animationMode();
    anim->setTimer( m_viewer->getScreenCapture()->getFrameTimer() );
    anim->home( 0 );
    m_viewer->getScreenCapture()->recordStart();
}

void WQtGLScreenCapture::toolBoxChanged( int index )
{
    if ( index != 3 )
    {
        wlog::debug( "WQtGLScreenCapture" ) << "Deactivating animation mode.";
        m_viewer->animationMode( false );
        return;
    }

    wlog::debug( "WQtGLScreenCapture" ) << "Activating animation mode.";
    m_viewer->animationMode();
}

void WQtGLScreenCapture::resolutionChange( bool force )
{
    if ( force )
    {
        wlog::debug( "WQtGLScreenCapture" ) << "Forcing resolution";

        // TODO(ebaum): this magic number stuff is ugly.
        switch( m_resolutionCombo->currentIndex() )
        {
        case 0:
            m_mainWindow->forceMainGLWidgetSize( 640, 480 );
            break;
        case 1:
            m_mainWindow->forceMainGLWidgetSize( 800, 600 );
            break;
        case 2: // TODO(ebaum): add offset edits
            m_mainWindow->forceMainGLWidgetSize( 800 + 50, 600 + 50 );
            break;
        case 3:
            m_mainWindow->forceMainGLWidgetSize( 1024, 768 );
            break;
        case 4:
            m_mainWindow->forceMainGLWidgetSize( 1280, 1024 );
            break;
        case 5: // TODO(ebaum): add offset edits
            m_mainWindow->forceMainGLWidgetSize( 1280 + 50, 1024 + 50 );
            break;
        case 6:
            m_mainWindow->forceMainGLWidgetSize( 1280, 720 );
            break;
        case 7:
            m_mainWindow->forceMainGLWidgetSize( 1920, 1080 );
            break;
        }
    }
    else
    {
        wlog::debug( "WQtGLScreenCapture" ) << "Restoring resolution";
        m_mainWindow->restoreMainGLWidgetSize();
    }
}

