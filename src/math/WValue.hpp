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

#ifndef WVALUE_H
#define WVALUE_H

#include <vector>

/**
 * Base class for all higher level values like tensors, vectors, matrices and so on.
 */
template< typename T > class WValue
{
public:
    /*
     * Create a WValue with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     */
    WValue( size_t nbComponents )
    : m_components( nbComponents )
    {
    }

    /*
     * Create a WValue as copy of the one given as parameter.
     */
    WValue( const WValue& newValue )
    : m_components( newValue.m_components )
    {
    }

    /**
     * Get number of components the value consists of.
     */
    size_t getNbComponents()
    {
        return m_components.size();
    }


protected:
    /**
     * The components the value is composed of. This contains the actual data
     */
    std::vector< T > m_components;
private:
};

#endif  // WVALUE_H
