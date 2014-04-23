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

#include "core/common/WConditionOneShot.h"
#include "core/common/WLogger.h"

#include "WUIWidgetBase.h"

WUIWidgetBase::WUIWidgetBase( std::string title ):
    m_title( title ),
    m_closeCondition( new WConditionOneShot() ),
    m_closed( false )
{
    // initialize members
}

WUIWidgetBase::~WUIWidgetBase()
{
    // cleanup
}

std::string WUIWidgetBase::getTitle() const
{
    return m_title;
}

void WUIWidgetBase::close()
{
    // already closed?
    if( isClosed() )
    {
        return;
    }

    m_closed = true;

    // close all children
    closeChildren();

    // clear the list of children. Their shared_ptr now get de-references.
    m_childs.clear();

    // notify about close
    m_closeCondition->notify();

    // let the implementor do the rest
    closeImpl();
}

WCondition::SPtr WUIWidgetBase::getCloseCondition() const
{
    return m_closeCondition;
}

void WUIWidgetBase::setParent( WUIWidgetBase::SPtr parent )
{
    m_parent = parent;
}

WUIWidgetBase::SPtr WUIWidgetBase::getParent() const
{
    return m_parent;
}

void WUIWidgetBase::registerChild( WUIWidgetBase::SPtr child )
{
    // it is a WSharedSequenceContainer which handles all the nasty locking stuff
    m_childs.unique_push_back( child );
}

bool WUIWidgetBase::allowNesting() const
{
    // not allowed by default.
    return false;
}

WUIWidgetBase::ChildContainer& WUIWidgetBase::getChildren()
{
    return m_childs;
}

const WUIWidgetBase::ChildContainer& WUIWidgetBase::getChildren() const
{
    return m_childs;
}

/**
 * Close the specified widget.
 *
 * \param widget the widget
 */
void closeFunctor( WUIWidgetBase::SPtr widget )
{
    widget->close();
}

void WUIWidgetBase::closeChildren()
{
    ChildContainer::WriteTicket w = m_childs.getWriteTicket();
    std::for_each( w->get().begin(), w->get().end(), closeFunctor );
}

bool WUIWidgetBase::isClosed() const
{
    return m_closed;
}

