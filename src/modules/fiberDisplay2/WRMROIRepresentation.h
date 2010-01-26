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

#ifndef WRMROIREPRESENTATION_H
#define WRMROIREPRESENTATION_H

#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

#include "WKdTree.h"
#include "../../graphicsEngine/WROI.h"
class WRMBranch;
/**
 * TODO(schurade): Document this!
 */
class WRMROIRepresentation
{
public:
    /**
     * TODO(schurade): Document this!
     */
    WRMROIRepresentation( boost::shared_ptr< WROI > roi, boost::shared_ptr< WRMBranch > branch );

    /**
     * TODO(schurade): Document this!
     */
    ~WRMROIRepresentation();

    /**
     * getter
     */
    boost::shared_ptr< WROI > getROI();

    /**
     *
     */
    boost::shared_ptr< std::vector< bool > > getBitField( unsigned int index );

    /**
     *
     */
    void addBitField( size_t size );

    /**
     *
     */
    void recalculate();

    /**
     *
     */
    void boxTest( int left, int right, int axis );

    /**
     *
     */
    unsigned int getLineForPoint( unsigned int point );

    /**
     * getter for dirty flag
     */
    bool isDirty();

    /**
     * sets dirty flag true and notifies the branch
     */
    void setDirty();

protected:
private:
    bool m_dirty;

    boost::shared_ptr< WROI > m_roi;

    boost::shared_ptr< WRMBranch > m_branch;

    std::list< boost::shared_ptr< std::vector<bool> > >m_bitFields;

    boost::shared_ptr< std::vector<bool> >m_currentBitfield;

    boost::shared_ptr< std::vector< float > > m_currentArray;

    boost::shared_ptr< std::vector< unsigned int > > m_currentReverse;

    boost::shared_ptr< WKdTree >m_kdTree;

    std::vector<float> m_boxMin;
    std::vector<float> m_boxMax;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;
};

#endif  // WRMROIREPRESENTATION_H
