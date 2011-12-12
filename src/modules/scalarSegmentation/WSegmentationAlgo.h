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

#ifndef WSEGMENTATIONALGO_H
#define WSEGMENTATIONALGO_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include "core/common/WPropertyTypes.h"
#include "core/common/WCondition.h"

#include "core/dataHandler/WDataSetScalar.h"

/**
 * A base class for segmentation alorithms.
 *
 * \class WSegmentationAlgo
 */
class WSegmentationAlgo : public boost::static_visitor< boost::shared_ptr< WDataSetScalar > >
{
public:
    //! A conveniant typedef.
    typedef boost::shared_ptr< WDataSetScalar > DataSetPtr;

    /**
     * Standard constructor.
     */
    WSegmentationAlgo();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgo();

    /**
     * Return a condition that indicates changes to the properties.
     *
     * \return The condition.
     */
    boost::shared_ptr< WCondition > getCondition();

    /**
     * Return the name of this algorithm.
     * \return The name.
     */
    virtual std::string getName() = 0;

    /**
     * Return a description of this algorithm.
     * \return A description.
     */
    virtual std::string getDescription() = 0;

    /**
     * Initialize all properties for this algorithm. This will call
     * the respective properties() function.
     *
     * \param group The property group all new properties should be added to.
     */
    void initProperties( WPropGroup group );

    /**
     * This is called to trigger the actual segmentation in the segmentation module.
     *
     * \param dataset The input dataset for segmentation.
     * \return The segmentation result.
     */
    DataSetPtr segment( DataSetPtr dataset );

    /**
     * Checks if any properties were changed.
     * \return True, iff any properties were changed.
     */
    virtual bool propChanged() = 0;

    /**
     * Tell the property group to hide itself.
     *
     * \param hide If true, the group will be hidden.
     */
    void hideProperties( bool hide );

protected:
    /**
     * A virtual function that calls the correct segmentation operation.
     * \return The resulting dataset.
     */
    virtual DataSetPtr applyOperation() = 0;

    /**
     * Initialize your algorithms properties here.
     */
    virtual void properties() = 0;

    //! The property group of this segmentation algorithm.
    WPropGroup m_properties;

    //! The condition indicating changed to the properties.
    boost::shared_ptr< WCondition > m_propCondition;

    //! A pointer to the currently processed dataset.
    DataSetPtr m_dataSet;
};

#endif  // WSEGMENTATIONALGO_H
