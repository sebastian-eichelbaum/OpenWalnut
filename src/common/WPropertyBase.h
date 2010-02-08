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

#ifndef WPROPERTYBASE_H
#define WPROPERTYBASE_H

#include <string>

#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "WPropertyTypes.h"

/**
 * Abstract base class for all properties. Simply provides name and type information.
 */
class WPropertyBase: public boost::enable_shared_from_this< WPropertyBase >
{
public:

    /**
     * Create an empty named property.
     *
     * \param name  the name of the property
     * \param description the description of the property
     */
    WPropertyBase( std::string name, std::string description );

    /**
     * Destructor.
     */
    virtual ~WPropertyBase();

    /**
     * Gets the name of the class.
     *
     * \return the name.
     */
    std::string getName() const;

    /**
     * Gets the description of the property.
     *
     * \return the description
     */
    std::string getDescription() const;

    /**
     * Determines whether the property is hidden or not.
     *
     * \return true if hidden
     */
    bool isHidden() const;

    /**
     * Sets the property hidden. This flag is especially used by the GUI.
     *
     * \param hidden true if it should be hidden.
     */
    void setHidden( bool hidden );

    /**
     * Gets the real WPropertyVariable type of this instance.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Helpers for easy conversion to the possible types
    /////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as integer property
     */
    WPropInt toPropInt();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as double property
     */
    WPropDouble toPropDouble();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as bool property
     */
    WPropBool toPropBool();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as string property
     */
    WPropString toPropString();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as path property
     */
    WPropFilename toPropPath();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as list property
     */
    WPropList toPropList();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as color property
     */
    WPropColor toPropColor();

    /**
     * Helper converts this instance to its native type.
     *
     * \return the property as position property
     */
    WPropPosition toPropPosition();

    /**
     * Signal signature emitted during set operations
     */
    typedef boost::function<void ( boost::shared_ptr< WPropertyBase > )> PropertyChangeNotifierType;

protected:

    /**
     * Name of the property.
     */
    std::string m_name;

    /**
     * Description of the property.
     */
    std::string m_description;

    /**
     * Flag denoting whether the property is hidden or not.
     */
    bool m_hidden;

    /**
     * Type of the PropertyVariable instance
     */
    PROPERTY_TYPE m_type;

    /**
     * Calculates the type of the property. This has to be done by the implementing class.
     */
    virtual void updateType();

    /**
     * Signal used for firing change signals
     */
    typedef boost::signals2::signal<void ( boost::shared_ptr< WPropertyBase >  )>  PropertyChangeSignalType;

    /**
     * Signal getting fired whenever the property changes.
     */
    PropertyChangeSignalType signal_PropertyChange;

private:
};

#endif  // WPROPERTYBASE_H

