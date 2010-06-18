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

#include <boost/shared_ptr.hpp>

class WModule;
class WApplyCombiner;

/**
 * A list of all combiners in a group.
 */
typedef std::vector< boost::shared_ptr< WApplyCombiner > > WCompatibleCombiners;

/**
 * A group of compatible connections to and from a specified module, which is additionally stored in the first element of the pair. This first
 * element will never be the module to which the compatible connections have been searched for.
 */
typedef std::pair< boost::shared_ptr< WModule >, WCompatibleCombiners > WCompatiblesGroup;

/**
 * This is a list of compatible connection groups, which has been created for a specific module.
 */
typedef std::vector< WCompatiblesGroup > WCompatiblesList;

#endif  // WMODULECOMBINERTYPES_H

