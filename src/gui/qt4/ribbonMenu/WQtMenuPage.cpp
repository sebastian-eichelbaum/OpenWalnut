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

#include "WQtMenuPage.h"

WQtMenuPage::WQtMenuPage( QString name, bool persistent )
    : QWidget()
{
    m_name = name;

    m_persistent = persistent;

    m_pageLayout = new QHBoxLayout();

    m_pageLayout->addStretch();

    this->setLayout( m_pageLayout );
}

WQtMenuPage::~WQtMenuPage()
{
}

QString WQtMenuPage::getName()
{
    return m_name;
}

void WQtMenuPage::addButton( WQtPushButton* button )
{
    int count = m_pageLayout->count();
    m_pageLayout->insertWidget( count -1, button );
    this->setLayout( m_pageLayout );
}

bool WQtMenuPage::isPersistent() const
{
    return m_persistent;
}

