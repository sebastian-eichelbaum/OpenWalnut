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

#ifndef WGESWITCHCALLBACK_H
#define WGESWITCHCALLBACK_H

#include <osg/Node>
#include <osg/Switch>

/**
 * This callback is able to switch a osg::Switch node using a property. Although this callback is a template, only int and bool props are useful
 * here. If a bool prop is used, the callback can trigger between child 0 and 1. Technically, WPropDouble is also possible.
 */
template < typename PropType >
class WGESwitchCallback: public osg::NodeCallback
{
public:
    /**
     * Creates new instance.
     *
     * \param prop the property which controls switch.
     */
    explicit WGESwitchCallback( PropType prop );

    /**
     * Destructor.
     */
    virtual ~WGESwitchCallback();

     /**
     * This operator gets called by OSG every update cycle.
     *
     * \param node the osg node
     * \param nv the node visitor
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
private:
    /**
     * The prop controlling the node switch
     */
     PropType m_prop;
};

template < typename PropType >
WGESwitchCallback< PropType >::WGESwitchCallback( PropType prop ):
    osg::NodeCallback(),
    m_prop( prop )
{
}

template < typename PropType >
WGESwitchCallback< PropType >::~WGESwitchCallback()
{
    // cleanup
}

template < typename PropType >
void WGESwitchCallback< PropType >::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // is it a switch?
    osg::Switch* s = node->asSwitch();
    if( !s )
    {
        traverse( node, nv );
        return;
    }

    // turn off all the others and enable the current one
    s->setAllChildrenOff();
    s->setSingleChildOn( m_prop->get() );

    traverse( node, nv );
}

#endif  // WGESWITCHCALLBACK_H

