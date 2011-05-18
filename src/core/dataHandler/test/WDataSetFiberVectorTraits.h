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

#ifndef WDATASETFIBERVECTORTRAITS_H
#define WDATASETFIBERVECTORTRAITS_H

#include <string>
#include <sstream>

#include "../WDataSetFiberVector.h"
#include "../../common/test/WTraitsBase.h"
#include "../../common/WStringUtils.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with a mixin vector, so
 * you see immedeatly what is failing.
 */
class ValueTraits< WDataSetFiberVector > : public WTraitsBase
{
public:
    /**
     * Constructs a new ValueTrait of a mixin vector for better test output
     */
    explicit ValueTraits( const WDataSetFiberVector & v )
    {
        std::stringstream ss;
        using string_utils::operator<<;
        size_t count = 0;
        ss << std::endl;
        for( WDataSetFiberVector::const_iterator cit = v.begin(); cit != v.end(); ++cit, ++count )
        {
            std::stringstream prefix;
            prefix << "fib: " << count << "[ ";
            std::string indent( prefix.str().size(), ' ' );
            for( WFiber::const_iterator dit = cit->begin(); dit != cit->end(); ++dit )
            {
                if( *dit == *cit->begin() )
                {
                    ss << prefix.str() << *dit << std::endl;
                }
                else
                {
                    ss << indent << *dit << std::endl;
                }
            }
            ss << std::string( indent.size() - 2, ' ' ) << "]" << std::endl;
        }
        m_s = ss.str();
    }
};
}
#endif  // CXXTEST_RUNNING
#endif  // WDATASETFIBERVECTORTRAITS_H
