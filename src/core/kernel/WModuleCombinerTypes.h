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

#ifndef WMODULECOMBINERTYPES_H
#define WMODULECOMBINERTYPES_H

#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>



class WModule;
class WModuleOneToOneCombiner;

namespace WCombinerTypes
{
    /**
     * A list of all combiners in a group.
     */
    typedef std::vector< boost::shared_ptr< WModuleOneToOneCombiner > > WOneToOneCombiners;

    /**
     * A group of compatible connections to and from a specified module, which is additionally stored in the first element of the pair. This first
     * element will never be the module to which the compatible connections have been searched for.
     */
    typedef std::pair< boost::shared_ptr< WModule >, WOneToOneCombiners > WCompatiblesGroup;

    /**
     * This is a list of compatible connection groups, which has been created for a specific module.
     */
    typedef std::vector< WCompatiblesGroup > WCompatiblesList;

    /**
     * A  pair of a connector and WCompatibleCombiners for each connection from/to the connector specified in the first element of the pair.
     */
    typedef std::pair< std::string, WOneToOneCombiners > WDisconnectGroup;

    /**
     * A list of all connectors and their possible disconnect- combiner.
     */
    typedef std::vector< WDisconnectGroup > WDisconnectList;

    /**
     * Sorting function for sorting the compatibles list. It uses the alphabetical order of the names.
     *
     * \param lhs the first combiner
     * \param rhs the second combiner
     *
     * \return true if lhs < rhs
     */
    bool  compatiblesSort( WCompatiblesGroup lhs, WCompatiblesGroup rhs );
}

#endif  // WMODULECOMBINERTYPES_H

