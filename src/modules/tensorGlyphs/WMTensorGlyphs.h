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

#ifndef WMTENSORGLYPHS_H
#define WMTENSORGLYPHS_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../common/WCondition.h"
#include "../../common/WItemSelection.h"
#include "../../common/WPropertyTypes.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

/**
 * Module for rendering of high order tensor glyphs.
 *
 * \ingroup modules
 */
class WMTensorGlyphs: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMTensorGlyphs();

    /**
     * Destructor.
     */
    virtual ~WMTensorGlyphs();

    /**
     * Overrides WModule::getName().
     *
     * \return The module's name.
     */
    virtual const std::string getName() const;

    /**
     * Overrides WModule::getDescription().
     *
     * \return The module's description.
     */
    virtual const std::string getDescription() const;

    /**
     * Overrides WModule::factory().
     *
     * \return An instance of this module.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Overrides WModule::getXPMIcon().
     *
     * \return The module's icon.
     */
    virtual const char** getXPMIcon() const;

protected:

    /**
     * Overrides WModule::moduleMain().
     */
    virtual void moduleMain();

    /**
     * Overrides WModule::connectors().
     */
    virtual void connectors();

    /**
     * Overrides WModule::properties().
     */
    virtual void properties();

private:

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_changed;

    /**
     * The input dataset containing the tensor data.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle> > m_input;

    /**
     * List containing the coloring modes.
     */
    boost::shared_ptr< WItemSelection > m_coloringList;

    /**
     * The coloring mode.
     */
    WPropSelection m_coloring;

    /**
     * Slice positions.
     */
    WPropInt m_slicePosition[ 3 ];

    /**
     * Slice visibility.
     */
    WPropBool m_sliceVisibility[ 3 ];
};

#endif  // WMTENSORGLYPHS_H
