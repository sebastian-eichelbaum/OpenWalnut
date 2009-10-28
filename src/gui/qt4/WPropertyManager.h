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

#include <QtCore/QObject>

#include "../../common/WProperties.h"
/**
 * Class that implements a list for pointers to properties objects from modules
 * Gui elemements will connect to its slots and write values to coresponding properties
 */
class WPropertyManager : public QObject
{
    Q_OBJECT

public:
    /**
     * standard constructor
     */
    WPropertyManager();

    /**
     * destructor
     */
    virtual ~WPropertyManager();

    /**
     * adds a property pointer to the list
     */
    void connectProperties( WProperties* properties );

public slots:
    /**
     * slot function that writes a boolean value to all properties objects that contain an entry
     * with that name
     */
    void slotBoolChanged( QString name, bool value );
    /**
     * slot function that writes a integer value to all properties objects that contain an entry
     * with that name
     */
    void slotIntChanged( QString name, int value );
    /**
     * slot function that writes a float value to all properties objects that contain an entry
     * with that name
     */
    void slotFloatChanged( QString name, float value );
    /**
     * slot function that writes a string value to all properties objects that contain an entry
     * with that name
     */
    void slotStringChanged( QString name, QString value );


protected:
private:
    std::vector<WProperties*>m_connectedProperties;
};

#endif  // WPROPERTYMANAGER_H
