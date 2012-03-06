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

#include "WGECallbackTraits.h"


/**
 * This callback allows you a simple usage of callbacks in your module. The callback uses function pointers and calls them every update cycle.
 * This is especially useful if you want to use a callback in a module without the need of writing subclasses providing a shared_ptr to the
 * parent module.
 *
 * \tparam Type the callback type. You can specify every class that has a nested class called "Callback".
 */
template < typename Type = osg::Node >
class WGEFunctorCallback: public WGECallbackTraits< Type >::CallbackType
{
public:
    /**
     * Shared pointer.
     */
    typedef osg::ref_ptr< WGEFunctorCallback > SPtr;

    /**
     * Const shared pointer.
     */
    typedef osg::ref_ptr< const WGEFunctorCallback > ConstSPtr;

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

    /**
     * This gets called by OSG every update cycle. It calls the specified functor.
     * \note we provide several versions here as the OSG does not uniformly use operator().
     *
     * \param handled the osg node, stateset or whatever
     * \param nv the node visitor
     */
    virtual void update( osg::NodeVisitor* nv, Type* handled );

protected:
private:
    /**
     * The functor getting called each callback.
     */
    FunctorType m_functor;
};

template < typename Type >
WGEFunctorCallback< Type >::WGEFunctorCallback( FunctorType functor ):
    WGECallbackTraits< Type >::CallbackType(),
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
    WGECallbackTraits< Type >::traverse( this, handled, nv );
}

template < typename Type >
void WGEFunctorCallback< Type >::update( osg::NodeVisitor* nv, Type* handled )
{
    operator()( handled, nv );
}

#endif  // WGEFUNCTORCALLBACK_H

