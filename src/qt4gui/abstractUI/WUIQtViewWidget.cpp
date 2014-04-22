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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"

#include "../guiElements/WQtPropertyTriggerAction.h"

#include "WUIQtViewWidget.h"

WUIQtViewWidget::WUIQtViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent ):
    WUIViewWidget( title ),
    WUIQtWidgetBase( mainWindow, parent ),
    m_projectionMode( projectionMode ),
    m_widgetDock( NULL )
{
    // initialize members
}

WUIQtViewWidget::~WUIQtViewWidget()
{
    // cleanup
}

QString WUIQtViewWidget::getTitleQString() const
{
    return QString::fromStdString( getTitle() );
}

osg::ref_ptr< WGEGroupNode > WUIQtViewWidget::getScene() const
{
    return m_scene;
}

boost::shared_ptr< WGEViewer > WUIQtViewWidget::getViewer() const
{
    // should be safe to call from outside the GUI thread !?
    return m_widgetDock->getGLWidget()->getViewer();
}

size_t WUIQtViewWidget::height() const
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        return static_cast< size_t >( m_widgetDock->getGLWidget()->getViewer()->getCamera()->getViewport()->height() );
    }
    else
    {
        return 0;
    }
}

size_t WUIQtViewWidget::width() const
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        return static_cast< size_t >( m_widgetDock->getGLWidget()->getViewer()->getCamera()->getViewport()->width() );
    }
    else
    {
        return 0;
    }
}

void WUIQtViewWidget::addEventHandler( osgGA::GUIEventHandler* handler )
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        m_widgetDock->getGLWidget()->getViewer()->getView()->addEventHandler( handler );
    }
}

void WUIQtViewWidget::show()
{
    WUIQtWidgetBase::show();
}

void WUIQtViewWidget::setVisible( bool visible )
{
    WUIQtWidgetBase::setVisible( visible );
}

bool WUIQtViewWidget::isVisible() const
{
    return WUIQtWidgetBase::isVisible();
}

void WUIQtViewWidget::cleanUpGT()
{
    // nothing to do
}

void WUIQtViewWidget::close()
{
    // use WUIViewWidget to handle this
    WUIViewWidget::close();
}

void WUIQtViewWidget::closeImpl()
{
    WUIQtWidgetBase::closeImpl();
}

void WUIQtViewWidget::realizeImpl()
{
    // WGEViewers need to have unique names. Create a uuid for this new viewer
    std::string uuid = boost::lexical_cast<std::string>( boost::uuids::random_generator()() );

    // this is called from withing the GUI thread -> we can safely create QT widgets here
    m_widgetDock = new WQtGLDockWidget( QString::fromStdString( getTitle() + uuid ),
                                        QString::fromStdString( getTitle() ), getCompellingQParent(), m_projectionMode );
    m_widgetDock->setObjectName( QString( "Custom Dock Window " ) + QString::fromStdString( getTitle() ) );

    // custom widgets need to take care of this! Besides this, if this widget is nested, it will not restore no matter what you set here.
    m_widgetDock->setSaveViewerSettings( false );

    // define some scene
    m_scene = new WGEGroupNode();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    m_widgetDock->getGLWidget()->getViewer()->setScene( m_scene );

    // remove the close button for embedded views
    if( getQtParent() )
    {
        m_widgetDock->disableCloseButton();
    }

    embedContent( m_widgetDock );
}

void WUIQtViewWidget::clearCameraPresets()
{
    WQt4Gui::execInGUIThread( boost::bind( &WUIQtViewWidget::clearCameraPresetsGT, this ) );
}

void WUIQtViewWidget::addCameraPreset( std::string name, WPropTrigger preset, WGEImage::SPtr icon )
{
    WQt4Gui::execInGUIThread( boost::bind( &WUIQtViewWidget::addCameraPresetGT, this, name, preset, icon ) );
}

void WUIQtViewWidget::clearCameraPresetsGT()
{
    if( m_widgetDock )
    {
        m_widgetDock->getCameraPresetMenu()->clear();
    }
}

void WUIQtViewWidget::addCameraPresetGT( std::string name, WPropTrigger preset, WGEImage::SPtr icon )
{
    if( m_widgetDock )
    {
        QMenu* m = m_widgetDock->getCameraPresetMenu();

        WQtPropertyTriggerAction* propAction = new WQtPropertyTriggerAction( preset, m_widgetDock );
        if( !icon )
        {
            propAction->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
        }
        else
        {
            propAction->setIcon( WIconManager::convertToIcon( icon ) );
        }
        m->addAction( propAction );
    }
}
