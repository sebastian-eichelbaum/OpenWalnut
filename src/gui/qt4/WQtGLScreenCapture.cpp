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

#include "WMainWindow.h"
#include "events/WEventTypes.h"
#include "../../common/WLogger.h"

#include "WQtGLScreenCapture.h"

WQtGLScreenCapture::WQtGLScreenCapture( WMainWindow* parent ):
    QDockWidget( "Animation", parent ),
    WGEScreenCapture(),
    m_iconManager( parent->getIconManager() )
{
    // initialize
    setObjectName( "Animation Dock" );

    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* widget = new QWidget( this );
    QVBoxLayout* layout = new QVBoxLayout();
    widget->setLayout( layout );
    m_toolbox = new QToolBox( this );

    QLabel* fileHint = new QLabel();
    fileHint->setWordWrap( true );
    fileHint->setText(
        "The filename used for all captured frames. This filename can contain some special tags which get replaced:"
                              "<ul>"
                              "<li> %# - the frame number"
                              "</ul>"
    );

    m_fileEdit = new QLineEdit();
    m_fileEdit->setText( QDir::homePath() + QDir::separator() + "OpenWalnut_Frame_%#.jpg" );


    layout->addWidget( fileHint );
    layout->addWidget( m_fileEdit );
    layout->addWidget( m_toolbox );
    setWidget( widget );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // screenshot tools
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_screenshotWidget = new QWidget();
    QVBoxLayout* screenshotLayout = new QVBoxLayout();
    m_screenshotWidget->setLayout( screenshotLayout );

    m_screenshotButton = new QPushButton( "Screenshot" );
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
                              "You should always add %# to your filename to differentiate between each frame. "
                              "This kind of recording can be very slow and produce a high IO load on your machine. Consider recording movies with "
                              "the animation tool which plays back a previously recorded scene and snapshot it frame-wise. "
                              "To create a movie with these images, you can use free encoders like ffmpeg, transcode or mencoder."
                              );
    movieLabel->setTextInteractionFlags( Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse );

    m_movieRecButton = new QPushButton( "Record" );
    connect( m_movieRecButton, SIGNAL(  clicked( bool ) ), this, SLOT( startRec() ) );

    m_movieStopButton = new QPushButton( "Stop" );
    m_movieStopButton->setDisabled( true );
    connect( m_movieStopButton, SIGNAL(  clicked( bool ) ), this, SLOT( stopRec() ) );

    movieLayout->addWidget( movieLabel );
    movieLayout->addWidget( m_movieRecButton );
    movieLayout->addWidget( m_movieStopButton );

    // add them
    m_toolbox->insertItem( 0, m_screenshotWidget, m_iconManager->getIcon( "image" ), "Screenshot" );
    m_toolbox->insertItem( 1, m_movieWidget, m_iconManager->getIcon( "video" ), "Movie" );
    m_toolbox->insertItem( 2, NULL, m_iconManager->getIcon( "video" ), "Animation" );

    // we need to be notified about the screen grabbers state
    m_startConnection = getRecordStartCondition()->subscribeSignal( boost::bind( &WQtGLScreenCapture::recStartCallback, this ) );
    m_stopConnection = getRecordStopCondition()->subscribeSignal( boost::bind( &WQtGLScreenCapture::recStopCallback, this ) );
}

WQtGLScreenCapture::~WQtGLScreenCapture()
{
    // cleanup
    m_startConnection.disconnect();
    m_stopConnection.disconnect();
}

void WQtGLScreenCapture::handleImage( size_t /* framesLeft */, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const
{
    std::string filename = m_fileEdit->text().toStdString();
    size_t pos;
    while ( ( pos = filename.find( "%#" ) ) != std::string::npos )
    {
        filename.replace( pos, 2, boost::lexical_cast< std::string >( totalFrames ) );
    }
    // wlog::info( "WQtGLScreenCapture" ) << "Writing frame " << totalFrames << " to " << filename;
    osgDB::writeImageFile( *image, filename );
}

bool WQtGLScreenCapture::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if( event->type() == WQT_SCREENCAPTURE_START_EVENT )
    {
        wlog::info( "WQtGLScreenCapture" ) << "Started recording.";
        m_screenshotWidget->setDisabled( true );
        m_movieStopButton->setDisabled( false );
        m_movieRecButton->setDisabled( true );
        m_fileEdit->setDisabled( true );
    }

    if( event->type() == WQT_SCREENCAPTURE_STOP_EVENT )
    {
        wlog::info( "WQtGLScreenCapture" ) << "Stopped recording.";
        m_screenshotWidget->setDisabled( false );
        m_movieStopButton->setDisabled( true );
        m_movieRecButton->setDisabled( false );
        m_fileEdit->setDisabled( false );
    }

    return QDockWidget::event( event );
}

void WQtGLScreenCapture::screenShot()
{
    screenshot();
}

void WQtGLScreenCapture::startRec()
{
    recordStart();
}

void WQtGLScreenCapture::stopRec()
{
    recordStop();
}

void WQtGLScreenCapture::recStartCallback()
{
    QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( WQT_SCREENCAPTURE_START_EVENT ) ) );
}

void WQtGLScreenCapture::recStopCallback()
{
    QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( WQT_SCREENCAPTURE_STOP_EVENT ) ) );
}

