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

#include <osg/Geometry>
#include <osg/LineStipple>

#include "../../common/exceptions/WTypeMismatch.h"
#include "../../common/math/WPosition.h"
#include "../../common/WLogger.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "WSPSliceBuilderVectors.h"

WSPSliceBuilderVectors::WSPSliceBuilderVectors( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
        boost::shared_ptr< const WDataSetVector > /* vector */ ) // todo(math): uncomment parameter if used
    : WSPSliceBuilder( probTracts, sliceGroup, colorMap )
{
}

void WSPSliceBuilderVectors::preprocess()
{
    // no preprocessing needed, but we have to implement this to be not abstract anymore
}

// todo(math): uncomment parameter if used
osg::ref_ptr< WGEGroupNode > WSPSliceBuilderVectors::generateSlice( const unsigned char /* sliceNum */ ) const
{
    return osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
}
