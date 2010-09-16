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

#ifndef WCOLOREDVERTICES_H
#define WCOLOREDVERTICES_H

#include <map>
#include <string>

#include "../WTransferable.h"
#include "../WColor.h"

#include "../WExportCommon.h"

/**
 * Represents a std::map where for each vertex ID a color is stored.
 */
class OWCOMMON_EXPORT WColoredVertices : public WTransferable // NOLINT
{
public:
    /**
     * Default constructor.
     */
    WColoredVertices();

    /**
     * Initialize this with the given map.
     *
     * \param data The map
     */
    explicit WColoredVertices( const std::map< size_t, WColor >& data );

    /**
     * Cleans up this instance.
     */
    virtual ~WColoredVertices();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Reference to the data.
     *
     * \return Reference to the map of ids and colors.
     */
    const std::map< size_t, WColor >& getData() const;

    /**
     * Replace (copies) the internal data with the given one.
     *
     * \param data The ID-Color map
     */
    void setData( const std::map< size_t, WColor >& data );

protected:
    static boost::shared_ptr< WPrototyped > m_prototype; //!< The prototype as singleton.

private:
    std::map< size_t, WColor > m_data; //!< stores the vertex ids and colors
};

inline const std::string WColoredVertices::getName() const
{
    return "WColoredVertices";
}

inline const std::string WColoredVertices::getDescription() const
{
    return "Represents a std::map where for each vertex ID a color is stored.";
}

inline const std::map< size_t, WColor >& WColoredVertices::getData() const
{
    return m_data;
}

inline void WColoredVertices::setData( const std::map< size_t, WColor >& data )
{
    m_data = data;
}

#endif  // WCOLOREDVERTICES_H
