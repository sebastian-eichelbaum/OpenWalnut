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

#ifndef WGEPROPERTYTRANSFORMATIONCALLBACK_H
#define WGEPROPERTYTRANSFORMATIONCALLBACK_H

#include <osg/Node>
#include <osg/TexMat>
#include <osg/StateAttribute>
#include <osg/MatrixTransform>

#include "WGECallbackTraits.h"
#include "../../common/WProperties.h"
#include "../WExportWGE.h"

/**
 * TODO(ebaum): write.
 */
template < typename ParentType = osg::Node, typename TargetType = osg::MatrixTransform >
class WGEPropertyTransformationCallback: public WGECallbackTraits< ParentType >::CallbackType
{
public:
    /**
     * Default constructor.
     *
     * \param prop the property holding the matrix to use.
     */
    explicit WGEPropertyTransformationCallback( WPropMatrix4X4 prop );

    /**
     * Destructor.
     */
    virtual ~WGEPropertyTransformationCallback();

    /**
     * This operator gets called by OSG every update cycle. It calls the specified functor.
     *
     * \param handled the osg node, stateset or whatever
     * \param nv the node visitor
     */
    virtual void operator()( typename WGECallbackTraits< ParentType >::HandledType* handled, osg::NodeVisitor* nv );

protected:
private:
    /**
     * The property controlling the callback
     */
    WPropMatrix4X4 m_prop;
};

template < typename ParentType, typename TargetType >
WGEPropertyTransformationCallback< ParentType, TargetType >::WGEPropertyTransformationCallback( WPropMatrix4X4 prop ):
    WGECallbackTraits< ParentType >::CallbackType(),
    m_prop( prop )
{
    // initialize members
}

template < typename ParentType, typename TargetType >
WGEPropertyTransformationCallback< ParentType, TargetType >::~WGEPropertyTransformationCallback()
{
    // cleanup
}

template < typename ParentType, typename TargetType >
void WGEPropertyTransformationCallback< ParentType, TargetType >::operator()( typename WGECallbackTraits< ParentType >::HandledType* handled,
                                                                              osg::NodeVisitor* nv )
{
    TargetType* m = dynamic_cast< TargetType* >( handled );
    if( m )
    {
        m->setMatrix( m_prop->get() );
    }

    WGECallbackTraits< ParentType >::traverse( this, handled, nv );
}

#endif  // WGEPROPERTYTRANSFORMATIONCALLBACK_H

