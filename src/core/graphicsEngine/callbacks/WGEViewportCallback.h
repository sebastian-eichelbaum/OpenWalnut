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

#ifndef WGEVIEWPORTCALLBACK_H
#define WGEVIEWPORTCALLBACK_H

#include <osg/Camera>
#include <osg/Node>

#include "../WGECamera.h"

/**
 * This callback is useful to update viewport information on several nodes supporting it. The specified type must support an setViewport method.
 * This is especially useful to keep offscreen render cameras in sync with the scene cam or to update HUD viewport information. Note that the
 * order of execution of callbacks for a node can cause problems as the new viewport might get set after it is needed.
 *
 * \tparam T the type supporting setViewport
 * \tparam Source the type from who the viewport should be acquired by using osg::Viewport* getViewport()
 */
template < typename T, typename Source = WGECamera >
class WGEViewportCallback: public osg::NodeCallback
{
public:
    /**
     * Creates new instance of viewport callback and sets the viewport size to the reference camera size
     *
     * \param reference set the viewport to the one of the reference camera.
     */
    explicit WGEViewportCallback( osg::ref_ptr< Source > reference );

    /**
     * Creates new instance of viewport callback and sets the viewport size to the specified size
     *
     * \param width viewport width
     * \param height viewport height
     */
    WGEViewportCallback( size_t width, size_t height );

    /**
     * Destructor.
     */
    virtual ~WGEViewportCallback();

     /**
     * This operator gets called by OSG every update cycle. It applies the viewport.
     *
     * \param node the osg node
     * \param nv the node visitor
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
private:
    /**
     * The reference camera to use.
     */
    osg::ref_ptr< Source > m_reference;

    /**
     * Forced viewport width
     */
    size_t m_width;

    /**
     * Forced viewport height
     */
    size_t m_height;
};

template < typename T, typename Source >
WGEViewportCallback< T, Source >::WGEViewportCallback( osg::ref_ptr< Source > reference ):
    osg::NodeCallback(),
    m_reference( reference ),
    m_width( 0 ),
    m_height( 0 )
{
    // initialize members
}

template < typename T, typename Source >
WGEViewportCallback< T, Source >::WGEViewportCallback( size_t width, size_t height ):
    osg::NodeCallback(),
    m_reference( NULL ),
    m_width( width ),
    m_height( height )
{
    // initialize members
}

template < typename T, typename Source >
WGEViewportCallback< T, Source >::~WGEViewportCallback()
{
    // cleanup
}

template < typename T, typename Source >
void WGEViewportCallback< T, Source >::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::ref_ptr< T > t = dynamic_cast< T* >( node );
    if( t )
    {
        if( m_reference )
        {
            t->setViewport( m_reference->getViewport() );
        }
        else
        {
            t->setViewport( new osg::Viewport( 0, 0, m_width, m_height ) );
        }
    }
    traverse( node, nv );
}

#endif  // WGEVIEWPORTCALLBACK_H

