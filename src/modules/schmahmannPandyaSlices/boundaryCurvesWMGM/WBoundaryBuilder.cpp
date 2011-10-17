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

#include "core/common/WProperties.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "WBoundaryBuilder.h"

WBoundaryBuilder::WBoundaryBuilder( boost::shared_ptr< const WDataSetScalar > texture, boost::shared_ptr< const WProperties > properties,
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices )
    : m_texture( texture ),
      m_slices( *slices ) // copy the pointers
{
    m_slicePos[0] = properties->findProperty( "Slices" )->toPropGroup()->findProperty( "Sagittal Position" )->toPropDouble();
    m_slicePos[1] = properties->findProperty( "Slices" )->toPropGroup()->findProperty( "Coronal Position" )->toPropDouble();
    m_slicePos[2] = properties->findProperty( "Slices" )->toPropGroup()->findProperty( "Axial Position" )->toPropDouble();

    m_grayMatter = properties->findProperty( "Gray Matter" )->toPropDouble();
    m_whiteMatter = properties->findProperty( "White Matter" )->toPropDouble();

    m_gmColor = properties->findProperty( "Gray Matter Color" )->toPropColor();
    m_wmColor = properties->findProperty( "White Matter Color" )->toPropColor();
}

WBoundaryBuilder::~WBoundaryBuilder()
{
}
