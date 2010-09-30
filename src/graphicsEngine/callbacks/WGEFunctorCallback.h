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

#include "../WExportWGE.h"

/**
 * This callback allows you a simple usage of callbacks in your module. The callback uses function pointers and calls them every update cycle.
 * This is especially useful if you want to use a callback in a module without the need of writing subclasses providing a shared_ptr to the
 * parent module.
 */
class WGE_EXPORT WGEFunctorCallback: public osg::NodeCallback
{
public:

    /**
     * The type of functor supported in this callback.
     */
    typedef boost::function< void ( osg::Node* )> FunctorType;

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
     * \param node the osg node
     * \param nv the node visitor
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:

private:

    /**
     * The functor getting called each callback.
     */
    FunctorType m_functor;
};

#endif  // WGEFUNCTORCALLBACK_H

