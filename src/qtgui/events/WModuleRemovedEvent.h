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

#ifndef WMODULEREMOVEDEVENT_H
#define WMODULEREMOVEDEVENT_H

#include <boost/shared_ptr.hpp>

#include <QtCore/QEvent>

#include "core/kernel/WModule.h"

/**
 * Event signalling a module was removed from the kernel root container.
 */
class WModuleRemovedEvent: public QEvent
{
public:
    /**
     * Creates a new event instance denoting that the specified module got removed from the root container.
     *
     * \param module the module
     */
    explicit WModuleRemovedEvent( boost::shared_ptr< WModule > module );

    /**
     * Destructor.
     */
    virtual ~WModuleRemovedEvent();

    /**
     * Getter for the module that got removed.
     *
     * \return the module.
     */
    boost::shared_ptr< WModule > getModule();

protected:
    /**
     * The module that got removed.
     */
    boost::shared_ptr< WModule > m_module;

private:
};

#endif  // WMODULEREMOVEDEVENT_H

