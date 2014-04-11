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

#include <QtGui/QGridLayout>

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"

#include "../guiElements/WQtDockWidget.h"

#include "WUIQtPropertyGroupWidget.h"

WUIQtPropertyGroupWidget::WUIQtPropertyGroupWidget(
            std::string title,
            WPropGroup properties,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent ):
    WUIPropertyGroupWidget( title ),
    WUIQtWidgetBase( mainWindow, parent ),
    m_propWidget( NULL ),
    m_properties( properties )
{
    // initialize members
}

WUIQtPropertyGroupWidget::~WUIQtPropertyGroupWidget()
{
    // cleanup
}

WPropGroup WUIQtPropertyGroupWidget::getPropertyGroup() const
{
    return m_properties;
}

QString WUIQtPropertyGroupWidget::getTitleQString() const
{
    return QString::fromStdString( getTitle() );
}

void WUIQtPropertyGroupWidget::show()
{
    WUIQtWidgetBase::show();
}

void WUIQtPropertyGroupWidget::setVisible( bool visible )
{
    WUIQtWidgetBase::setVisible( visible );
}

bool WUIQtPropertyGroupWidget::isVisible() const
{
    return WUIQtWidgetBase::isVisible();
}

void WUIQtPropertyGroupWidget::cleanUpGT()
{
    // nothing to do
}

void WUIQtPropertyGroupWidget::close()
{
    // use WUIPropertyGroupWidget to handle this
    WUIPropertyGroupWidget::close();
}

void WUIQtPropertyGroupWidget::closeImpl()
{
    // notify child widgets
    WUIQtWidgetBase::closeImpl();
}

void WUIQtPropertyGroupWidget::realizeImpl()
{
    // this is called from withing the GUI thread -> we can safely create QT widgets here

    // this widget does what we want:
    m_propWidget = WQtPropertyGroupWidget::createPropertyGroupBox( m_properties, true, getTitleQString(), 0, getCompellingQParent() );

    // let WUIQtWidgetBase do the rest
    embedContent( m_propWidget );
}

