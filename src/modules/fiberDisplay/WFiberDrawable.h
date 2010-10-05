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
#include <osg/ShapeDrawable>
#include <osg/Group>

#include "WFiberSelector.h"


class WDataSetFibers;
/**
 * TODO(schurade): Document this!
 */
class WFiberDrawable: public osg::Drawable
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
     * no clue why it's here and wehat it does
     */
    virtual osg::Object* cloneType() const;

    /**
     * clones it
     *
     * \param copyop
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

    /**
    * Real work is done here. THERE IS A VERY IMPORTANT THING TO NOTE HERE:
    * the 'drawImplementation()' method receives an 'osg::State' as
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
    * \param renderInfo
    */
    virtual void drawImplementation( osg::RenderInfo& renderInfo ) const; //NOLINT

    /**
     * sets the dataset pointer which provides access to the necessary array
     *
     * \param dataset
     */
    void setDataset( boost::shared_ptr< const WDataSetFibers > dataset );

    /**
     * toggles drawing of tubes
     *
     * \param flag
     */
    void setUseTubes( bool flag );

    /**
     * Set the coloring modes of the fibers.
     *
     * \param globalColoring If true is given global coloring is applied ( default ), else local coloring
     */
    void setColoringMode( bool globalColoring );

    /**
     * Set the use of custom colors
     *
     * \param custom
     */
    void setCustomColoring( bool custom );

    /**
     * In which mode coloring for the fibers is applied.
     *
     * \return True for global, false for local coloring
     */
    bool getColoringMode() const;

    /**
     * Set the bounding box of all fibers.
     * \param bb The new bounding box.
     */
    void setBoundingBox( const osg::BoundingBox & bb );

    /**
     * setter
     * \param bitField selected fibers to draw
     */
    void setBitfield( boost::shared_ptr< std::vector< bool > > bitField );

    /**
     * setter
     * \param selector the selector object which provides the bitfield for fiber selection
     */
    void setSelector( boost::shared_ptr< WFiberSelector > selector );

    /**
     * setter
     * \param colors
     */
    void setCustomColors( boost::shared_ptr< std::vector< float > > colors );

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

    boost::shared_ptr< const WDataSetFibers > m_dataset; //!< stores pointer to dataset

    boost::shared_mutex m_recalcLock; //!< lock

    bool m_useTubes; //!< flag

    bool m_globalColoring; //!< True indicates global, false local coloring

    bool m_customColoring; //!< True indicates use of custom colors


    boost::shared_ptr< std::vector< bool > > m_active; //!< pointer to the bitfield of active fibers

    boost::shared_ptr< WFiberSelector > m_fiberSelector; //!< the selector object which provides the bitfield for fiber selection
};

inline void WFiberDrawable::setDataset( boost::shared_ptr< const WDataSetFibers > dataset )
{
    m_dataset = dataset;
}

inline void WFiberDrawable::setUseTubes( bool flag )
{
    m_useTubes = flag;
}

inline void WFiberDrawable::setColoringMode( bool globalColoring )
{
    m_globalColoring = globalColoring;
}

inline bool WFiberDrawable::getColoringMode() const
{
    return m_globalColoring;
}

inline void WFiberDrawable::setCustomColoring( bool custom )
{
    m_customColoring = custom;
}

inline void WFiberDrawable::setBoundingBox( const osg::BoundingBox & bb )
{
    setBound( bb );
}

inline void WFiberDrawable::setBitfield( boost::shared_ptr< std::vector< bool > > bitField )
{
    m_active = bitField;
}

inline void WFiberDrawable::setSelector( boost::shared_ptr< WFiberSelector > selector )
{
    m_fiberSelector = selector;
}

#endif  // WFIBERDRAWABLE_H
