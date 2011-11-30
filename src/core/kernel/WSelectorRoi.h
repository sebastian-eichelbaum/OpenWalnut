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

#ifndef WSELECTORROI_H
#define WSELECTORROI_H

#include <vector>

#include "../dataHandler/WDataSetFibers.h"

#include "../graphicsEngine/WROI.h"

//#include "WKdTree.h"
class WKdTree;

/**
 * class implements the updating of a bitfield for a roi
 */
class WSelectorRoi
{
public:
    /**
     * constructor
     * \param roi the roi representation
     * \param fibers the fiber dataset to work on
     * \param kdTree kd tree for fast intersection checks
     */
    WSelectorRoi( osg::ref_ptr< WROI > roi, boost::shared_ptr< const WDataSetFibers > fibers, boost::shared_ptr< WKdTree> kdTree );

    /**
     * destructor
     */
    ~WSelectorRoi();

    /**
     * getter
     * \return the bitfield for this ROI
     */
    boost::shared_ptr< std::vector<bool> >getBitField();

    /**
     * getter
     * access to the ROI representation, mainly for delete and update functions
     *
     * \return Pointer to the ROI representation
     */
    osg::ref_ptr< WROI > getRoi();

    /**
     * setter
     * sets the dirty flag
     */
    void setDirty();

protected:
private:
    /**
     * updates the output bitfiel when something with the rois has changed
     */
    void recalculate();

    /**
     * recursive function to check for intersections with the roi
     * \param left
     * \param right
     * \param axis
     */
    void boxTest( int left, int right, int axis );

    /**
     * getter
     * \param point point to check
     * \return the index of the line the point is part of
     *
     */
    size_t getLineForPoint( size_t point );

    /**
     * pointer to the roi
     */
    osg::ref_ptr< WROI > m_roi;

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_fibers;

    /**
     * Stores a pointer to the kdTree used for fiber selection
     */
    boost::shared_ptr< WKdTree > m_kdTree;

    /**
     * size of the fiber dataset, stored for convinience
     */
    size_t m_size;

    /**
     * dirty flag
     */
    bool m_dirty;

    /**
     * the bitfield that is given to the outside world
     */
    boost::shared_ptr< std::vector<bool> >m_bitField;

    /**
     * the bitfield we work on
     */
    boost::shared_ptr< std::vector<bool> >m_workerBitfield;

    /**
     * pointer to the array that is used for updating
     * this is used for the recurse update function, to reduce the amount of function parameters
     */
    boost::shared_ptr< std::vector< float > > m_currentArray;

    /**
     * pointer to the reverse array that is used for updating
     * this is used for the recurse update function, to reduce the amount of function parameters
     */
    boost::shared_ptr< std::vector< size_t > > m_currentReverse;

    std::vector<float> m_boxMin; //!< lower boundary of the box, used for boxtest
    std::vector<float> m_boxMax; //!< upper boundary of the box, used for boxtest

    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the selector ROI
};

inline boost::shared_ptr< std::vector<bool> > WSelectorRoi::getBitField()
{
    if( m_dirty )
    {
        recalculate();
    }
    return m_bitField;
}

inline size_t WSelectorRoi::getLineForPoint( size_t point )
{
    return ( *m_currentReverse )[point];
}

inline osg::ref_ptr< WROI > WSelectorRoi::getRoi()
{
    return m_roi;
}

#endif  // WSELECTORROI_H
