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

#ifndef WGECALLBACKTRAITS_H
#define WGECALLBACKTRAITS_H

#include <osg/Node>
#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osg/Drawable>

/**
 * This class is needed as OSG does not define a uniform callback type.
 */
template < typename Type >
class WGECallbackTraits
{
public:
    /**
     * The real callback type. Some specific osg classes have specific callbacks. Specialize this template in this case.
     */
    typedef typename Type::Callback CallbackType;

    /**
     * The type of the element used as parameter in the () operator.
     */
    typedef Type HandledType;

    /**
     * Call traversal method if existing for the specific callback type.
     *
     * \param inst the instance to use
     * \param handled the instance of the handled object
     * \param nv the node visitor
     */
    static void traverse( CallbackType* inst, HandledType* handled, osg::NodeVisitor* nv );
};

template < typename Type >
void WGECallbackTraits< Type >::traverse( CallbackType* /*inst*/, Type* /*handled*/, osg::NodeVisitor* /*nv*/ )
{
    // the generic case: no nested callbacks -> no traversal
}

/**
 * Nodes have their own callback type and provide a traverse method (as they can be nested).
 */
template <>
class WGECallbackTraits< osg::Node >
{
public:

    /**
     * The real callback type. Some specific OSG classes have specific callbacks. Specialize this template in this case.
     */
    typedef osg::NodeCallback CallbackType;

    /**
     * The type of the element used as parameter in the () operator.
     */
    typedef osg::Node HandledType;

    /**
     * Call traversal method if existing for the specific callback type. This calls osg::NodeCallback::traverse.
     *
     * \param inst the instance to use
     * \param handled the instance of the handled object
     * \param nv the node visitor
     */
    static void traverse( CallbackType* inst, HandledType* handled, osg::NodeVisitor* nv )
    {
        inst->traverse( handled, nv );
    }
};

/**
 * StateAttributes have their own callback type and do NOT provide a traverse method.
 */
template <>
class WGECallbackTraits< osg::StateAttribute >
{
public:

    /**
     * The real callback type. Some specific OSG classes have specific callbacks. Specialize this template in this case.
     */
    typedef osg::StateAttribute::Callback CallbackType;

    /**
     * The type of the element used as parameter in the () operator.
     */
    typedef osg::StateAttribute HandledType;

    /**
     * Call traversal method if existing for the specific callback type.
     */
    static void traverse( CallbackType* /*inst*/, HandledType* /*handled*/, osg::NodeVisitor* /*nv*/ )
    {
        // no traverse allowed
    }
};

/**
 * StateSets have their own callback type and do NOT provide a traverse method.
 */
template <>
class WGECallbackTraits< osg::StateSet >
{
public:

    /**
     * The real callback type. Some specific OSG classes have specific callbacks. Specialize this template in this case.
     */
    typedef osg::StateSet::Callback CallbackType;

    /**
     * The type of the element used as parameter in the () operator.
     */
    typedef osg::StateSet HandledType;

    /**
     * Call traversal method if existing for the specific callback type.
     */
    static void traverse( CallbackType* /*inst*/, HandledType* /*handled*/, osg::NodeVisitor* /*nv*/ )
    {
        // no traverse allowed
    }
};

/**
 * Drawables have their own callback type and do NOT provide a traverse method.
 */
template <>
class WGECallbackTraits< osg::Drawable >
{
public:

    /**
     * The real callback type. Some specific OSG classes have specific callbacks. Specialize this template in this case.
     */
    typedef osg::Drawable::UpdateCallback CallbackType;

    /**
     * The type of the element used as parameter in the () operator.
     */
    typedef osg::Drawable HandledType;

    /**
     * Call traversal method if existing for the specific callback type.
     */
    static void traverse( CallbackType* /*inst*/, HandledType* /*handled*/, osg::NodeVisitor* /*nv*/ )
    {
        // no traverse allowed
    }
};


#endif  // WGECALLBACKTRAITS_H

