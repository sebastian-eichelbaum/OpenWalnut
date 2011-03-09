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

#ifndef WBOUNDARYBUILDER_H
#define WBOUNDARYBUILDER_H

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "../../graphicsEngine/WGEManagedGroupNode.h"

// forward declarations
class WGEManagedGroupNode;
class WDataSetScalar;
class WProperties;

class WBoundaryBuilder
{
public:
    WBoundaryBuilder( boost::shared_ptr< const WDataSetScalar > texture,
                      boost::shared_ptr< const WProperties > properties,
                      boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices );

    virtual ~WBoundaryBuilder();

    /**
     * Starts rendering and finally insert result in output.
     *
     * \param output Where to put/insert the results.
     */
    virtual void run( osg::ref_ptr< WGEManagedGroupNode > output ) = 0;

protected:
    boost::shared_ptr< const WDataSetScalar > m_texture;

    boost::array< WPropInt, 3 > m_slicePos;

    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > m_slices;

    boost::shared_ptr< const WPVDouble > m_grayMatter;

    boost::shared_ptr< const WPVDouble > m_whiteMatter;

private:
};

#endif  // WBOUNDARYBUILDER_H
