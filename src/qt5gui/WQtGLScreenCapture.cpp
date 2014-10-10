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

#include <iostream>
#include <sstream>
#include <string>

#include <osgDB/WriteFile>

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QApplication>
#include <QGroupBox>

#include "core/common/WLogger.h"
#include "events/WEventTypes.h"
#include "WMainWindow.h"
#include "WQtGLWidget.h"
#include "WQtGLDockWidget.h"
#include "WQtGui.h"
#include "WQtGLScreenCapture.h"

WQtGLScreenCapture::WQtGLScreenCapture( WQtGLDockWidget* parent ):
    QWidget( parent ),
    m_glDockWidget( parent ),
    m_viewer( m_glDockWidget->getGLWidget()->getViewer() )
{
    // initialize
    setObjectName( "Recorder Dock - " + parent->getDockTitle() );
    setWindowTitle( "Recorder - " + parent->getDockTitle() );

    // create our toolbox and add it to the main layout
    m_toolbox = new QToolBox( this );
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->addWidget( m_toolbox );

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
    m_resolutionCombo->addItem( "1024x768" );
    m_resolutionCombo->addItem( "1280x1024" );
    m_resolutionCombo->addItem( "1280x720 (720p)" );
    m_resolutionCombo->addItem( "1920x1080 (1080p)" );
    m_resolutionCombo->addItem( "Custom" );

    m_customWidth = new QLineEdit( "1980", this );
    m_customHeight = new QLineEdit( "1080", this );
    m_customWidth->setValidator( new QIntValidator( 0, 4096, m_customWidth ) );
    m_customHeight->setValidator( new QIntValidator( 0, 4096, m_customHeight ) );

    QPushButton* resolutionButton = new QPushButton( "Set" );
    resolutionButton->setCheckable( true );
    resolutionGroupLayout->addWidget( m_resolutionCombo, 0, 0, 1, 2 );
    resolutionGroupLayout->addWidget( resolutionButton, 0, 2 );
    resolutionGroupLayout->addWidget( new QLabel( "Custom Resolution" ), 1, 0, 1, 3 );
    resolutionGroupLayout->addWidget( m_customWidth, 2, 0 );
    resolutionGroupLayout->addWidget( m_customHeight, 2, 1 );

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
    std::string defaultFilename = ( QDir::homePath() + QDir::separator() +
                                    "OpenWalnut - " + parent->getDockTitle() + " - Frame %f.png" ).toStdString();
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
    m_screenshotButton->setToolTip( "Take a screenshot of this view" );
    m_screenshotAction = new QAction( WQtGui::getIconManager()->getIcon( "image" ), "Screenshot", this );
    m_screenshotAction->setToolTip( "Take a screenshot of this view" );
    m_screenshotAction->setShortcut( QKeySequence(  Qt::Key_F12 ) );
    m_screenshotAction->setShortcutContext( Qt::ApplicationShortcut );
    connect( m_screenshotAction, SIGNAL(  triggered( bool ) ), this, SLOT( screenShot() ) );
    connect( m_screenshotButton, SIGNAL(  clicked( bool ) ), this, SLOT( screenShot() ) );

    QLabel* screenshotLabel = new QLabel();
    screenshotLabel->setWordWrap( true );
    screenshotLabel->setText( "Take a screenshot of the current scene with the current camera. You need to specify "
                              "a filename in the toolbox item \"Configuration\"." );
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
    movieLabel->setText( "Take a screenshot for every frame with the current camera. You need to specify a filename "
                         "in the toolbox item \"Configuration\". "
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
    // plug it together
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this forces the elements to be initialized properly
    QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( WQT_SCREENCAPTURE_EVENT ) ) );

    // add them
    m_toolbox->insertItem( 0, m_configWidget, WQtGui::getIconManager()->getIcon( "preferences" ), "Configuration" );
    m_toolbox->insertItem( 1, m_screenshotWidget, WQtGui::getIconManager()->getIcon( "image" ), "Screenshot" );
    m_toolbox->insertItem( 2, m_movieWidget, WQtGui::getIconManager()->getIcon( "video" ), "Movie" );

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

void WQtGLScreenCapture::saveSettings()
{
    WQtGui::getSettings().setValue( objectName() + "/resolution", m_resolutionCombo->currentIndex() );
    WQtGui::getSettings().setValue( objectName() + "/customResolutionWidth", m_customWidth->text() );
    WQtGui::getSettings().setValue( objectName() + "/customResolutionHeight", m_customHeight->text() );
    WQtGui::getSettings().setValue( objectName() + "/filename", m_configFileEdit->text() );
}

void WQtGLScreenCapture::restoreSettings()
{
    m_customWidth->setText( WQtGui::getSettings().value( objectName() + "/customResolutionWidth", m_customWidth->text() ).toString() );
    m_customHeight->setText( WQtGui::getSettings().value( objectName() + "/customResolutionHeight", m_customHeight->text() ).toString() );
    m_configFileEdit->setText( WQtGui::getSettings().value( objectName() + "/filename", m_configFileEdit->text() ).toString() );
    m_resolutionCombo->setCurrentIndex(  WQtGui::getSettings().value( objectName() + "/resolution", m_resolutionCombo->currentIndex() ).toInt() );
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
    while( ( pos = filename.find( "%f" ) ) != std::string::npos )
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
        if( framesLeft == 0 )   // recording done:
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

    return QWidget::event( event );
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

void WQtGLScreenCapture::toolBoxChanged( int /* index */ )
{
    // no operation if toolbox changes.
}

void WQtGLScreenCapture::resolutionChange( bool force )
{
    if( force )
    {
        wlog::debug( "WQtGLScreenCapture" ) << "Forcing resolution";

        switch( m_resolutionCombo->currentIndex() )
        {
        case 0:
            m_glDockWidget->forceGLWidgetSize( 640, 480 );
            break;
        case 1:
            m_glDockWidget->forceGLWidgetSize( 800, 600 );
            break;
        case 2:
            m_glDockWidget->forceGLWidgetSize( 1024, 768 );
            break;
        case 3:
            m_glDockWidget->forceGLWidgetSize( 1280, 1024 );
            break;
        case 4:
            m_glDockWidget->forceGLWidgetSize( 1280, 720 );
            break;
        case 5:
            m_glDockWidget->forceGLWidgetSize( 1920, 1080 );
            break;
        case 6: // custom size
            m_glDockWidget->forceGLWidgetSize( m_customWidth->text().toInt(), m_customHeight->text().toInt() );
            break;
        }
    }
    else
    {
        wlog::debug( "WQtGLScreenCapture" ) << "Restoring resolution";
        m_glDockWidget->restoreGLWidgetSize();
    }
}

