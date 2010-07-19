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

#ifndef WRMBRANCH_H
#define WRMBRANCH_H

#include <list>
#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "./../../../common/WProperties.h"
#include "WRMROIRepresentation.h"

class WROIManagerFibers;

/**
 * implements a branch in the tree like structure for rois
 */
class WRMBranch : public boost::enable_shared_from_this< WRMBranch >
{
public:
    /**
     * construtor
     * \param roiManager
     */
    explicit WRMBranch( boost::shared_ptr< WROIManagerFibers > roiManager );

    /**
     * destructor
     */
    ~WRMBranch();

    /**
     * adds a roi to the branch
     *
     * \param roi
     */
    void addRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * removes a roi from the branch
     *
     * \param roi
     */
    void removeRoi( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * removes all rois from the branch
     *
     */
    void removeAllRois();



    /**
     * getter for the bitfield
     *
     * \return the bitfield
     */
    boost::shared_ptr< std::vector< bool > > getBitField();

    /**
     * creates and adds a bitfield to the list
     *
     * \param size of the bitfield
     */
    void addBitField( size_t size );

    /**
     * updates the branch bitfield for this branch
     */
    void recalculate();

    /**
     * getter for dirty flag
     *
     * \return the dirty flag
     */
    bool isDirty();

    /**
     * sets dirty flag true and notifies the branch
     */
    void setDirty();

    /**
     * returns whether the branch is empty
     */
    bool isEmpty();

    /**
     * returns a pointer to the first roi in the branch
     *
     * \return the roi
     */
    boost::shared_ptr< WRMROIRepresentation >getFirstRoi();

    /**
     * getter for roi manager pointer
     *
     * \return the roi manager
     */
    boost::shared_ptr< WROIManagerFibers > getRoiManager();

    /**
     * returns the properties object
     */
    boost::shared_ptr< WProperties > getProperties();


protected:
    /**
     * slot gets called when a property has changed
     *
     */
    void slotToggleNot();

    /**
     * slot gets called when a property has changed
     *
     */
    void slotChangeBundleColor();


private:
    bool m_dirty; //!< dirty flag to indicate the bit field must be recalculated

    boost::shared_ptr< WROIManagerFibers > m_roiManager; //!< stores a pointer to the roi manager

    boost::shared_ptr< std::vector<bool> >m_bitField; //!< list of bit fields for each fiber dataset

    std::list< boost::shared_ptr< WRMROIRepresentation> > m_rois; //!< list of rois in this this branch,
                                                                  // first in the list is the master roi
    /**
     * lock to prevent concurrent threads trying to update the branch
     */
    boost::shared_mutex m_updateLock;

    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * indicates if the branch is negated
     */
    WPropBool m_isNot;

    /**
     * The color used when in isosurface mode for blending.
     */
    WPropColor m_bundleColor;
};

#endif  // WRMBRANCH_H
