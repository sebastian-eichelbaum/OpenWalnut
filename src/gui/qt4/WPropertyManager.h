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

#ifndef WPROPERTYMANAGER_H
#define WPROPERTYMANAGER_H

#include <string>
#include <vector>

#include <boost/thread/mutex.hpp>

#include <QtCore/QObject>

#include "../../common/WProperties2.h"
/**
 * Class that implements a list for pointers to properties objects from modules
 * Gui elemements will connect to its slots and write values to coresponding properties
 */
class WPropertyManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Standard constructor. Creates an empty property manager.
     */
    WPropertyManager();

    /**
     * Destructor.
     */
    virtual ~WPropertyManager();

    /**
     * Add a property to the list of managed properties.
     *
     * \param properties the property to add.
     */
    void connectProperties( boost::shared_ptr< WProperties2 > properties );

public slots:

    /**
     * Slot function that writes a boolean value to all properties objects that contain an entry
     * with that name.
     *
     * \param name the name of the property to set the bool for.
     * \param value the bool value to set.
     */
    void slotBoolChanged( QString name, bool value );

    /**
     * Slot function that writes a integer value to all properties objects that contain an entry
     * with that name
     *
     * \param name the name of the property to set the value for.
     * \param value the value to set.
     */
    void slotIntChanged( QString name, int value );

    /**
     * slot function that writes a float value to all properties objects that contain an entry
     * with that name
     *
     * \param name the name of the property to set the value for.
     * \param value the value to set.
     */
    void slotFloatChanged( QString name, float value );

    /**
     * slot function that writes a string value to all properties objects that contain an entry
     * with that name
     *
     * \param name the name of the property to set the value for.
     * \param value the value to set.
     */
    void slotStringChanged( QString name, QString value );

protected:
private:
    /**
    * Mutex used to lock the properties.
    */
    boost::mutex m_PropertiesLock;

    /**
     * List of all properties actually connected with this manager instance.
     */
    std::vector< boost::shared_ptr< WProperties2 > > m_connectedProperties;
};

#endif  // WPROPERTYMANAGER_H
