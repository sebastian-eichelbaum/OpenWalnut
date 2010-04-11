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
#include <iostream>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>

#include "../../graphicsEngine/WGEViewer.h"
#include "../../graphicsEngine/WGEScene.h"
#include "events/WPropertyChangedEvent.h"
#include "events/WEventTypes.h"

#include "WQtNavGLWidget.h"

WQtNavGLWidget::WQtNavGLWidget( QString title, QWidget* parent, int maxValue, std::string sliderTitle )
    : QDockWidget( title, parent )
{
    m_sliderTitle = QString( sliderTitle.c_str() );

    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    m_slider = new QSlider( Qt::Horizontal );
    m_slider->setMaximum( maxValue );
    m_slider->setValue( maxValue / 2 );
    m_slider->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    QWidget* panel = new QWidget( this );

    QVBoxLayout* layout = new QVBoxLayout();

    m_glWidget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( title.toStdString(), panel, WGECamera::ORTHOGRAPHIC ) );
    m_glWidget->setFixedSize( 150, 150 );
    m_glWidget->initialize();

    setMinimumSize( 160, 200 );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );

    m_scene = new WGEScene();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    m_glWidget->getViewer()->setScene( m_scene );

    if( title ==  QString( "axial" ) )
    {
        osg::Matrix m;
        m.makeRotate( 90.0, 1.0, 0.0, 0.0 );
        osg::Matrix tm;
        //tm.makeTranslate( osg::Vec3( 20.0, 0.0, 60.0 ) );
        m *= tm;
        osg::Matrix sm;
        sm.makeScale( 1.2, 1.2, 1.2 );
        m *= sm;
        m_scene->setMatrix( m );
    }
    if( title == QString(  "coronal" ) )
    {
        osg::Matrix m;
        m.makeIdentity();
        osg::Matrix tm;
        //tm.makeTranslate( osg::Vec3( 20.0, 0.0, 30.0 ) );
        m *= tm;
        osg::Matrix sm;
        sm.makeScale( 1.2, 1.2, 1.2 );
        m *= sm;
        m_scene->setMatrix( m );
    }
    if( title == QString( "sagittal" ) )
    {
        osg::Matrix m;
        m.makeRotate( 90.0, 0.0, 0.0, 1.0 );
        osg::Matrix tm;
        tm.makeTranslate( osg::Vec3( 200.0, 0.0, 0.0 ) );
        m *= tm;
        osg::Matrix sm;
        sm.makeScale( 1.2, 1.2, 1.2 );
        m *= sm;
        m_scene->setMatrix( m );
    }

    layout->addWidget( m_glWidget.get() );
    layout->addWidget( m_slider );

    panel->setLayout( layout );

    setWidget( panel );

    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderValueChanged( int ) ) );
}

WQtNavGLWidget::~WQtNavGLWidget()
{
    if( m_slider )
    {
        delete m_slider;
        m_slider = 0;
    }
}

void WQtNavGLWidget::closeEvent( QCloseEvent* event )
{
    // forward events
    m_glWidget->close();

    event->accept();
}

void WQtNavGLWidget::setSliderTitle( std::string title )
{
    m_sliderTitle = QString( title.c_str() );
}

boost::shared_ptr<WQtGLWidget>WQtNavGLWidget::getGLWidget()
{
    return m_glWidget;
}

void WQtNavGLWidget::sliderValueChanged( int value )
{
    if ( m_sliderProp )
    {
        m_sliderProp->set( value );
    }
    emit navSliderValueChanged( m_sliderTitle, value );
}

void WQtNavGLWidget::setSliderProperty( WPropInt prop )
{
    m_sliderProp = prop;
    m_sliderProp->getCondition()->subscribeSignal( boost::bind( &WQtNavGLWidget::handleChangedPropertyValue, this ) );
}

bool WQtNavGLWidget::event( QEvent* event )
{
    // a property changed
    if ( event->type() == WQT_PROPERTY_CHANGED_EVENT )
    {
        if( m_slider )
        {
            m_slider->setValue( m_sliderProp->get() );
        }
        return true;
    }

    return QWidget::event( event );
}

void WQtNavGLWidget::handleChangedPropertyValue()
{
    // post an event to handle it in the gui thread
    QCoreApplication::postEvent( this, new WPropertyChangedEvent() );
}

