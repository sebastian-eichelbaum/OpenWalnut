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

#ifndef WBOUNDARYFRAGMENTS_H
#define WBOUNDARYFRAGMENTS_H

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>

#include <osg/ref_ptr>

#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "WBoundaryBuilder.h"

// forward declarations
class WDataSetScalar;

/**
 * Constructs the boundary surface curves of the WM and GM via a shader approach.
 */
class WBoundaryFragments : public WBoundaryBuilder
{
public:
     /**
     * Constructs an builder instance.
     *
     * \param texture The underlying scalar dataset to compute those boundaries for
     * \param properties Properties, like slice positions, thresholds etc.
     * \param slices Slice geodes which are controlled (hide/unhide) by the module.
     * \param localPath Path where to search for the shaders.
     */
    WBoundaryFragments( boost::shared_ptr< const WDataSetScalar > texture,
                        boost::shared_ptr< const WProperties > properties,
                        boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices,
                        boost::filesystem::path localPath );

    /**
     * Starts rendering and finally insert result in output.
     *
     * \param output Where to put/insert the results.
     * \param sliceNum If given -1 all slice will perform an update, otherwise only the slice with the given slice number.
     */
    virtual void run( osg::ref_ptr< WGEManagedGroupNode > output, const char sliceNum = -1 );

protected:
private:
};

#endif  // WBOUNDARYFRAGMENTS_H
