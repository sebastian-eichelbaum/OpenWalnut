//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WMANIPULATORNORMALIZE_H
#define WMANIPULATORNORMALIZE_H

#include "core/common/WObjectNDIP.h"

#include "WManipulatorInterface.h"

class WDataSet;

/**
 * A manipulator to scale datasets to form a unit cube.
 */
class WManipulatorNormalize : public WObjectNDIP< WManipulatorInterface >
{
public:
    /**
     * Constructor.
     * \param dataSet The data set to be transformed
     */
    explicit WManipulatorNormalize( boost::shared_ptr< WDataSet >* dataSet );

    /**
     * Destructor.
     */
    virtual ~WManipulatorNormalize();

    /**
     * Get the transformation matrix from this manipulator.
     *
     * \return The current transform.
     */
    virtual WMatrixFixed< double, 4, 4 > getTransformationMatrix() const;

    /**
     * Check if the transform has changed, for example because of a change to properties.
     *
     * \return true, iff the transformation has changed.
     */
    virtual bool transformationChanged() const;

    /**
     * Reset the transform.
     */
    virtual void reset();

private:
    //! The normalize parameters as a vector.
    WPropPosition m_normalize;

    //! Pointer to the dataset to be normalized
    boost::shared_ptr< WDataSet >* m_dataSet;
};

#endif  // WMANIPULATORNORMALIZE_H
