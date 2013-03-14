//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMANIPULATORTRANSLATION_H
#define WMANIPULATORTRANSLATION_H

#include "core/common/WObjectNDIP.h"

#include "WManipulatorInterface.h"

/**
 * A manipulator for translation of datasets in world coordinates.
 */
class WManipulatorTranslation : public WObjectNDIP< WManipulatorInterface >
{
public:
    /**
     * Constructor.
     */
    WManipulatorTranslation();

    /**
     * Destructor.
     */
    virtual ~WManipulatorTranslation();

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
    //! The translation.
    WPropPosition m_translation;
};

#endif  // WMANIPULATORTRANSLATION_H
