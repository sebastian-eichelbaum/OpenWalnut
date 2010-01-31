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

#ifndef WMARKHANDLER_H
#define WMARKHANDLER_H

#include <osgGA/GUIEventHandler>

#include "../common/WFlag.h"


/**
 * Class to handle events which mark a time position in an EEG or MEG recording.
 */
class WMarkHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructor
     */
    WMarkHandler();

    /**
     * Handle events.
     *
     * \param ea event class for storing keyboard, mouse and window events
     * \param aa the action adapter
     * \return true if handled, false otherwise
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    /**
     * Get the flag which contains the marked position
     *
     * \return the flag as pointer
     */
    WFlag< double >* getPositionFlag();

protected:
    /**
     * Protected destructor because this class is derived from an OSG class
     */
    virtual ~WMarkHandler();

private:
    /**
     * Flag which contains the marked position
     */
    WFlag< double > m_positionFlag;

    /**
     * Calculate the new marked position
     *
     * \param ea event class for storing keyboard, mouse and window events
     * \param aa the action adapter
     * \return true if handled, false otherwise
     */
    bool calculateNewPosition( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
};

#endif  // WMARKHANDLER_H
