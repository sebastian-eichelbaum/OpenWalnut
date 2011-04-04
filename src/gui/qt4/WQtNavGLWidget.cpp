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

WQtNavGLWidget::WQtNavGLWidget( QString title, QWidget* parent, std::string sliderTitle, const QGLWidget * shareWidget )
    : QDockWidget( title, parent )
{
    setObjectName( QString( "Navigation Window " ) + title );
    propertyWidgetMap.clear();
    m_sliderTitle = QString( sliderTitle.c_str() );

    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();

#ifndef _MSC_VER
    m_glWidget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( title.toStdString(), panel, WGECamera::ORTHOGRAPHIC, shareWidget ) );
#else
    m_glWidget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( title.toStdString(), panel, WGECamera::ORTHOGRAPHIC ) );
#endif

    setMinimumSize( 160, 240 );
    //setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );

    m_scene = new WGEScene();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    m_glWidget->getViewer()->getView()->requestContinuousUpdate( false );
    m_glWidget->getViewer()->setScene( m_scene );

    m_layout->addWidget( m_glWidget.get() );

    panel->setLayout( m_layout );

    setWidget( panel );

    // we need to know whether the dock is visible or not
    connect( this, SIGNAL( visibilityChanged( bool ) ), this, SLOT( handleVisibilityChange( bool ) ) );
}

WQtNavGLWidget::~WQtNavGLWidget()
{
    for( std::map< boost::shared_ptr< WPropertyBase >, WPropertyIntWidget* >::iterator it = propertyWidgetMap.begin();
         it != propertyWidgetMap.end();
         ++it )
    {
        if( it->second != 0 )
        {
            delete it->second;
        }
    }
}

void WQtNavGLWidget::setSliderTitle( std::string title )
{
    m_sliderTitle = QString( title.c_str() );
}

boost::shared_ptr<WQtGLWidget>WQtNavGLWidget::getGLWidget()
{
    return m_glWidget;
}

void WQtNavGLWidget::setSliderProperty( boost::shared_ptr< WPropertyBase > prop )
{
    WPropertyIntWidget* propWidget;
    propWidget = new WPropertyIntWidget( prop->toPropInt(), NULL, parentWidget() );
    propertyWidgetMap[prop] = propWidget;
    m_layout->addWidget( propWidget );
    m_layout->setStretchFactor( m_glWidget.get(), 1 );
    m_layout->setStretchFactor( propWidget, 0 );
}

void WQtNavGLWidget::removeSliderProperty( boost::shared_ptr< WPropertyBase > prop )
{
    WPropertyIntWidget* propWidget = propertyWidgetMap[prop];
    m_layout->removeWidget( propWidget );
    delete propWidget;
    propertyWidgetMap.erase( prop );
}

void WQtNavGLWidget::handleVisibilityChange( bool visible )
{
    // this canhelp to reduce CPU load. Especially if multithreading viewers are used with cull thread per context.
    m_glWidget->getViewer()->getView()->getScene()->getSceneData()->setNodeMask( visible * 0xFFFFFFFF );
}

