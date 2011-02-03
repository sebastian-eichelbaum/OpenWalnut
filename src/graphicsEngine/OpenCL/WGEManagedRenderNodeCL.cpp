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

#include "WGEManagedRenderNodeCL.h"

WGEManagedRenderNodeCL::WGEManagedRenderNodeCL( boost::shared_ptr< WBoolFlag > active, bool deactivated ):
    WGERenderNodeCL( deactivated ),
    m_flag( active )
{
    // initialize
}

WGEManagedRenderNodeCL::WGEManagedRenderNodeCL( const WGEManagedRenderNodeCL& node, const osg::CopyOp& copyop ):
    WGERenderNodeCL( node, copyop ),
    m_flag( node.m_flag )
{
    // initialize
}

WGEManagedRenderNodeCL::~WGEManagedRenderNodeCL()
{
    // initialize
}

osg::Object* WGEManagedRenderNodeCL::cloneType() const
{
    return new WGEManagedRenderNodeCL( *this );
}

osg::Object* WGEManagedRenderNodeCL::clone( const osg::CopyOp& copyop ) const
{
    return new WGEManagedRenderNodeCL( *this, copyop );
}

bool WGEManagedRenderNodeCL::isSameKindAs( const osg::Object* object ) const
{
    return ( dynamic_cast< const WGEManagedRenderNodeCL* >( object ) != 0 );
}

const char* WGEManagedRenderNodeCL::className() const
{
    return "WGEManagedRenderNodeCL";
}

void WGEManagedRenderNodeCL::accept( osg::NodeVisitor& nv ) // NOLINT  // non-const reference
{
    if ( m_flag->get() )
    {
        WGERenderNodeCL::accept( nv );
    }
}

