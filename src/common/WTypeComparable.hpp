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

#ifndef WCOMPARABLE_H
#define WCOMPARABLE_H

/**
 * Interface class for the concept of type comparable (runtime) classes, The functionality provided by this class can and will be
 * achieved by facilities provided by the upcoming C++0X standard as they get available.
 */
template < typename T >
class WTypeComparable
{
public:

    /**
     * Default constructor. Creates a instance of the class. This not necessarily mean that the instance is fully usable. This is
     * required for type checking and inheritance checking.
     */
    WTypeComparable();

    /**
     * Destructor.
     */
    virtual ~WTypeComparable();

    /**
     * Checks whether the specified type is castable to the true type of this instance.
     *
     * \return true if castable using dynamic_cast.
     */
    template < typename U > bool isCompatibleType()
    {
        T* a = T::factory();
        U* b = U::factory();
    }

protected:

private:
};

template < typename T >
WTypeComparable< T >::WTypeComparable()
{
    // init
}

template < typename T >
WTypeComparable< T >::~WTypeComparable()
{
    // clean up
}

#endif  // WTYPECOMPARABLE_H

