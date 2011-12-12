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

#ifndef WGEMANAGEDGROUPNODE_H
#define WGEMANAGEDGROUPNODE_H

#include <boost/shared_ptr.hpp>

#include "WGEGroupNode.h"
#include "../common/WFlag.h"
#include "WExportWGE.h"

/**
 * This class adds some convenience methods to WGEGroupNode. It can handle bool properties which switch the node on and off.
 *
 * \ingroup GE
 */
class WGE_EXPORT WGEManagedGroupNode: public WGEGroupNode
{
public:
    /**
     * Shared pointer.
     */
    typedef osg::ref_ptr< WGEManagedGroupNode > SPtr;

    /**
     * Const shared pointer.
     */
    typedef osg::ref_ptr< const WGEManagedGroupNode > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param active the flag denoting the node to be activated ( if flag == true ) or turned off.
     */
    explicit WGEManagedGroupNode( boost::shared_ptr< WBoolFlag > active );

protected:
    /**
     * Destructor.
     */
    virtual ~WGEManagedGroupNode();

private:
};

#endif  // WGEMANAGEDGROUPNODE_H

