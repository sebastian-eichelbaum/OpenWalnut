//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community, Hochschule Worms
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

#ifndef WSINGLEPOSITION_H
#define WSINGLEPOSITION_H

#include <string>

#include "../../common/math/linearAlgebra/WPosition.h"
#include "../../common/WTransferable.h"

/**
 * Represents a single position that is transferable between modules by connectors.
 */
class WSinglePosition: public WTransferable, public WPosition // NOLINT
{
public:
    /**
     * Shared pointer abbreviation.
     */
    typedef boost::shared_ptr< WSinglePosition > SPtr;

    /**
     * Const shared pointer abbreviation.
     */
    typedef boost::shared_ptr< const WSinglePosition > ConstSPtr;

    /**
     * Copies the specified \ref WPosition instance.
     *
     * \param position the instance to clone.
     */
    explicit WSinglePosition( const WPosition& position );

    /**
     * Constructs an empty position.
     */
    WSinglePosition();

    /**
     * Destructs.
     */
    virtual ~WSinglePosition();

    /**
     * The name of this transferable. This is useful information for the users.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     *
     * The description of this transferable. This is useful information for the users.
     *
     * \return A description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:
    /**
     * Prototype for this dataset
     */
    static boost::shared_ptr< WPrototyped > m_prototype;
};

inline const std::string WSinglePosition::getName() const
{
    return "SinglePosition";
}

inline const std::string WSinglePosition::getDescription() const
{
    return "A single 3D position that can be used for module connectors.";
}

#endif  // WSINGLEPOSITION_H
