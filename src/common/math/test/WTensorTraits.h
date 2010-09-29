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

#ifndef WTENSORTRAITS_H
#define WTENSORTRAITS_H

#include <sstream>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../../test/WTraitsBase.h"
#include "../WTensorSym.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
/**
 * Enables better UnitTest OutPut if something fails with symmetric tensors of order 2 and , so you see
 * immedeatly what is failing.
 */
template< template< size_t, size_t, typename > class TensorBase_T, size_t order, size_t dim, typename Data_T > // NOLINT incomplete declaration (multiline?)
class ValueTraits< TensorBase_T< order, dim, Data_T> > : public WTraitsBase
{
public:
    /**
     * Constructor for class allowing usable output of WMatrix in tests
     *
     * \param m the WMatrix to print
     */
    explicit ValueTraits( const TensorBase_T< order, dim, Data_T > &t )
    {
        std::stringstream tmp;
        tmp.precision( 5 );
        tmp << std::endl;
        tmp << t << std::endl;
        m_s = tmp.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WTENSORTRAITS_H
