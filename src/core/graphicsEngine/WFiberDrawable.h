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

#ifndef WFIBERDRAWABLE_H
#define WFIBERDRAWABLE_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

#include <osg/Drawable>

#include "WExportWGE.h"

/**
 * Class implements an osg::Drawable that paints fiber representations either using lines or tubes
 */
class WGE_EXPORT WFiberDrawable: public osg::Drawable // NOLINT
{
public:
    /**
    * The constructor here does nothing. One thing that may be necessary is
    * disabling display lists. This can be done by calling
    * setSupportsDisplayList (false);
    * Display lists should be disabled for 'Drawable's that can change over
    * time (that is, the vertices drawn change from time to time).
    */
    WFiberDrawable();

    /**
     * I can't say much about the methods below, but OSG seems to expect
     * that we implement them.
     *
     * \param pg
     * \param copyop
     */
    WFiberDrawable( const WFiberDrawable& pg, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * See osg documentation for this.
     *
     * \return the cloned object
     */
    virtual osg::Object* cloneType() const;

    /**
     * clones it
     *
     * \param copyop copy operation. See osg doc for details
     * \return the cloned object
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

    /**
    * Real work is done here. THERE IS A VERY IMPORTANT THING TO NOTE HERE:
    * the \ref drawImplementation method receives an state as
    * parameter. This can be used to change the OpenGL state, but changing
    * the OpenGL state here is something to be avoided as much as possible.
    * Do this *only* if it is *absolutely* necessary to make your rendering
    * algorithm work. The "right" (most efficient and flexible) way to change
    * the OpenGL state in OSG is by attaching 'StateSet's to 'Node's and
    * 'Drawable's.
    * That said, the example below shows how to change the OpenGL state in
    * these rare cases in which it is necessary. But always keep in mind:
    * *Change the OpenGL state only if strictly necessary*.
    *
    * \param renderInfo the render info object. See osg doc for details
    */
    virtual void drawImplementation( osg::RenderInfo& renderInfo ) const; //NOLINT

    /**
     * toggles drawing of tubes
     *
     * \param flag
     */
    void setUseTubes( bool flag );

    using osg::Drawable::setBound;

    /**
     * setter
     * \param bitField selected fibers to draw
     */
    void setBitfield( boost::shared_ptr< std::vector< bool > > bitField );

    /**
     * setter
     * \param idx
     */
    void setStartIndexes( boost::shared_ptr< std::vector< size_t > > idx );

    /**
     * setter
     * \param ppl
     */
    void setPointsPerLine( boost::shared_ptr< std::vector< size_t > > ppl );

    /**
     * setter
     * \param verts
     */
    void setVerts( boost::shared_ptr< std::vector< float > > verts );

    /**
     * setter
     * \param tangents
     */
    void setTangents( boost::shared_ptr< std::vector< float > > tangents );

    /**
     * setter
     * \param color
     */
    void setColor( boost::shared_ptr< std::vector< float > > color );

protected:
private:
    /**
     * Draw fibers as ordinary lines.
     *
     * \param renderInfo
     */
    void drawFibers( osg::RenderInfo& renderInfo ) const; //NOLINT

    /**
     * Draw fibers as fake tubes.
     */
    void drawTubes() const;

    boost::shared_mutex m_recalcLock; //!< lock

    bool m_useTubes; //!< flag

    boost::shared_ptr< std::vector< bool > > m_active; //!< pointer to the bitfield of active fibers

    boost::shared_ptr< std::vector< size_t > > m_startIndexes; //!< pointer to the field of line start indexes
    boost::shared_ptr< std::vector< size_t > > m_pointsPerLine; //!< pointer to the field of points per line
    boost::shared_ptr< std::vector< float > > m_verts; //!< pointer to the field of vertexes
    boost::shared_ptr< std::vector< float > > m_tangents; //!< pointer to the field of line tangents
    boost::shared_ptr< std::vector< float > > m_colors; //!< pointer to the field of colors per vertex
};

inline void WFiberDrawable::setUseTubes( bool flag )
{
    m_useTubes = flag;
}

inline void WFiberDrawable::setBitfield( boost::shared_ptr< std::vector< bool > > bitField )
{
    m_active = bitField;
}

inline void WFiberDrawable::setStartIndexes( boost::shared_ptr< std::vector< size_t > > idx )
{
    m_startIndexes = idx;
}

inline void WFiberDrawable::setPointsPerLine( boost::shared_ptr< std::vector< size_t > > ppl )
{
    m_pointsPerLine = ppl;
}

inline void WFiberDrawable::setVerts( boost::shared_ptr< std::vector< float > > verts )
{
    m_verts = verts;
}

inline void WFiberDrawable::setTangents( boost::shared_ptr< std::vector< float > > tangents )
{
    m_tangents = tangents;
}

inline void WFiberDrawable::setColor( boost::shared_ptr< std::vector< float > > color )
{
    m_colors = color;
}

#endif  // WFIBERDRAWABLE_H
