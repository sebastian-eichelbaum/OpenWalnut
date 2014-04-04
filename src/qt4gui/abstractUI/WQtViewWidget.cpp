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

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"

#include "WQtViewWidget.h"

WQtViewWidget::WQtViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            WMainWindow* mainWindow ):
    WUIViewWidget( title ),
    WQtWidgetBase( mainWindow )
{
    // initialize members
}

WQtViewWidget::~WQtViewWidget()
{
    // cleanup
}

osg::ref_ptr< WGEGroupNode > WQtViewWidget::getScene() const
{
}

boost::shared_ptr< WGEViewer > WQtViewWidget::getViewer() const
{
}

size_t WQtViewWidget::height() const
{
}

size_t WQtViewWidget::width() const
{
}

void WQtViewWidget::addEventHandler( osgGA::GUIEventHandler* handler )
{
}

void WQtViewWidget::show()
{
}

void WQtViewWidget::setVisible( bool visible )
{
}

bool WQtViewWidget::isVisible() const
{
}

void WQtViewWidget::close()
{
}

void WQtViewWidget::realizeImpl()
{

}

