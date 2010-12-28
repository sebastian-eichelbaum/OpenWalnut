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

#ifndef WGEFUNCTORCALLBACK_H
#define WGEFUNCTORCALLBACK_H

#include <boost/signals2.hpp>

#include <osg/Node>
#include <osg/NodeCallback>

#include "../WExportWGE.h"

/**
 * This class is needed as OSG does not define a uniform callback type.
 */
template < typename Type >
class WGEFunctorCallbackTraits
{
public:
    /**
     * The real callback type. Some specific osg classes have specific callbacks. Specialize this template in this case.
     */
    typedef typename Type::Callback CallbackType;

    /**
     * Call traversal method if existing for the specific callback type.
     *
     * \param inst the instance to use
     * \param handled the instance of the handled object
     * \param nv the node visitor
     */
    static void traverse( CallbackType* inst, Type* handled, osg::NodeVisitor* nv );
};

template < typename Type >
void WGEFunctorCallbackTraits< Type >::traverse( CallbackType* /*inst*/, Type* /*handled*/, osg::NodeVisitor* /*nv*/ )
{
    // the generic case: no nested callbacks -> no traversal
}

/**
 * Nodes have their own callback type and provide a traverse method (as they can be nested).
 */
template <>
class WGEFunctorCallbackTraits< osg::Node >
{
public:

    /**
     * The real callback type. Some specific OSG classes have specific callbacks. Specialize this template in this case.
     */
    typedef osg::NodeCallback CallbackType;

    /**
     * Call traversal method if existing for the specific callback type. This calls osg::NodeCallback::traverse.
     *
     * \param inst the instance to use
     * \param handled the instance of the handled object
     * \param nv the node visitor
     */
    static void traverse( CallbackType* inst, osg::Node* handled, osg::NodeVisitor* nv )
    {
        inst->traverse( handled, nv );
    }
};

/**
 * This callback allows you a simple usage of callbacks in your module. The callback uses function pointers and calls them every update cycle.
 * This is especially useful if you want to use a callback in a module without the need of writing subclasses providing a shared_ptr to the
 * parent module.
 *
 * \tparam Type the callback type. You can specify every class that has a nested class called "Callback".
 */
template < typename Type = osg::Node >
class WGEFunctorCallback: public WGEFunctorCallbackTraits< Type >::CallbackType
{
public:

    /**
     * The type of functor supported in this callback.
     */
    typedef boost::function< void ( Type* )> FunctorType;

    /**
     * Default constructor. Creates the callback and sets the specified functor instance.
     *
     * \param functor the function pointer.
     */
    explicit WGEFunctorCallback( FunctorType functor );

    /**
     * Destructor.
     */
    virtual ~WGEFunctorCallback();

    /**
     * This operator gets called by OSG every update cycle. It calls the specified functor.
     *
     * \param handled the osg node, stateset or whatever
     * \param nv the node visitor
     */
    virtual void operator()( Type* handled, osg::NodeVisitor* nv );

protected:
private:

    /**
     * The functor getting called each callback.
     */
    FunctorType m_functor;
};

template < typename Type >
WGEFunctorCallback< Type >::WGEFunctorCallback( FunctorType functor ):
    WGEFunctorCallbackTraits< Type >::CallbackType(),
    m_functor( functor )
{
    // initialize members
}

template < typename Type >
WGEFunctorCallback< Type >::~WGEFunctorCallback()
{
    // cleanup
}

template < typename Type >
void WGEFunctorCallback< Type >::operator()( Type* handled, osg::NodeVisitor* nv )
{
    // call functor
    m_functor( handled );
    WGEFunctorCallbackTraits< Type >::traverse( this, handled, nv );
}

#endif  // WGEFUNCTORCALLBACK_H

