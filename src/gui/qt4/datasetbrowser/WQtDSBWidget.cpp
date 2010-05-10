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

#include <QtGui/QGroupBox>
#include <QtGui/QScrollArea>

#include "WQtDSBWidget.h"

WQtDSBWidget::WQtDSBWidget( std::string name, QWidget* parent  )
    : QWidget( parent ),
    m_name( name.c_str() ),
//    m_controlLayout(),
//    m_pageLayout(),
    m_numberOfWidgets( 0 )
{
    // note: never do layouts as none pointers
    // on destruction of a widget it will try to delete them which will cause crashes
    m_pageLayout = new QVBoxLayout();
    m_controlLayout = new QGridLayout();
    m_pageLayout->addLayout( m_controlLayout );
}

WQtDSBWidget::~WQtDSBWidget()
{
}

WPropertyBoolWidget* WQtDSBWidget::addProp( WPropBool property )
{
    ++m_numberOfWidgets;

    return new WPropertyBoolWidget( property, m_controlLayout, this );
}

WPropertyIntWidget* WQtDSBWidget::addProp( WPropInt property )
{
    ++m_numberOfWidgets;

    return new WPropertyIntWidget( property, m_controlLayout, this );
}

WPropertyDoubleWidget* WQtDSBWidget::addProp( WPropDouble property )
{
    ++m_numberOfWidgets;

    return new WPropertyDoubleWidget( property, m_controlLayout, this );
}

WPropertyStringWidget* WQtDSBWidget::addProp( WPropString property )
{
    ++m_numberOfWidgets;

    return new WPropertyStringWidget( property, m_controlLayout, this );
}

WPropertyColorWidget* WQtDSBWidget::addProp( WPropColor property )
{
    ++m_numberOfWidgets;

    return new WPropertyColorWidget( property, m_controlLayout, this );
}

WPropertyFilenameWidget* WQtDSBWidget::addProp( WPropFilename property )
{
    ++m_numberOfWidgets;

    return new WPropertyFilenameWidget( property, m_controlLayout, this );
}

WPropertyTriggerWidget* WQtDSBWidget::addProp( WPropTrigger property )
{
    ++m_numberOfWidgets;

    return new WPropertyTriggerWidget( property, m_controlLayout, this );
}

WPropertySelectionWidget* WQtDSBWidget::addProp( WPropSelection property )
{
    ++m_numberOfWidgets;

    return new WPropertySelectionWidget( property, m_controlLayout, this );
}

void WQtDSBWidget::addGroup( WQtDSBWidget* widget, bool asScrollArea )
{
    ++m_numberOfWidgets;

    // TODO(ebaum): extend it to collapse the group
    QGroupBox* group = new QGroupBox( widget->getName() , this );

    QScrollArea *scrollArea;
    QGridLayout *grid = new QGridLayout();
    grid->addWidget( widget, 0, 0 );

    group->setLayout( grid );
    if ( asScrollArea )
    {
        scrollArea = new QScrollArea();
        scrollArea->setWidget( group );
        group->show();
    }

    // create a new group box
    int row = m_controlLayout->rowCount();

    if ( asScrollArea )
    {
        m_controlLayout->addWidget( scrollArea, row, 0, 1, 2 );
    }
    else
    {
        m_controlLayout->addWidget( group, row, 0, 1, 2 );
    }
}

void WQtDSBWidget::addSpacer()
{
    m_pageLayout->addStretch();
    setLayout( m_pageLayout );
}

QString WQtDSBWidget::getName()
{
    return m_name;
}

void WQtDSBWidget::setName( QString name )
{
    m_name = name;
}

bool WQtDSBWidget::isEmpty() const
{
    return !m_numberOfWidgets;
}
