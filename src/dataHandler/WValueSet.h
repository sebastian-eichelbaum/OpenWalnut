//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WVALUESET_H
#define WVALUESET_H

/**
 * Base Class for all value set types.
 */
class WValueSet
{
/**
 * Only UnitTests are allowed to be friends
 */
friend class WValueSetTest;

public:
    /**
     * Constructs an empty WValueSet instance
     */
    WValueSet();

    /**
     * \return The number of values stored in this set.
     */
    unsigned int getNumberOfValues() const;

protected:

private:
    /**
     * Stores the number of values in this set
     */
    unsigned int m_numberOfValues;
};

#endif  // WVALUESET_H
