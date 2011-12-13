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

#ifndef WMIMAGEEXTRACTOR_H
#define WMIMAGEEXTRACTOR_H

#include <string>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WValueSet.h"

/**
 * This module extracts single images from a dataset.
 *
 * \ingroup modules
 */
class WMImageExtractor: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMImageExtractor();

    /**
     * Destructor.
     */
    virtual ~WMImageExtractor();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Return an icon for this module.
     *
     * \return the icon
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Callback for m_active.
     */
    virtual void activate();

private:
    /**
     * Extract the ith image from the dataset. If there is no
     * ith image, an invalid pointer will be returned. For scalar datasets, only the 0th
     * image can be extracted and so on.
     *
     * \param i The number of the image to be extracted.
     * \return A pointer to extracted image as a scalar dataset or an invalid pointer, if no ith image exists.
     */
    boost::shared_ptr< WDataSetScalar > extract( std::size_t i ) const;

    /**
     * Create a name for the extracted image.
     *
     * \param i The number of the image.
     * \return The name.
     */
    const std::string makeImageName( std::size_t i );

    /**
     * Write property values to output dataset.
     */
    void setOutputProps();

    /**
     * A property that allows selecting the number of the desired image.
     */
    WPropInt m_selectedImage;

    //! The data value percent that maps to 0.0 in the texture.
    WPropDouble m_minValuePct;

    //! The data value percent that maps to 1.0 in the texture.
    WPropDouble m_maxValuePct;

    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
    // the following 5 members are taken from WMData
    /**
     * Interpolation?
     */
    WPropBool m_interpolation;

    /**
     * A list of color map selection types
     */
    boost::shared_ptr< WItemSelection > m_colorMapSelectionsList;

    /**
     * Selection property for color map
     */
    WPropSelection m_colorMapSelection;

    /**
     * Threshold value for this data.
     */
    WPropDouble m_threshold;

    /**
     * Opacity value for this data.
     */
    WPropInt m_opacity;
    // }

    /**
     * An input connector that accepts order 1 datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * An output connector for the output scalar dataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * This is a pointer to the current output.
     */
    boost::shared_ptr< WDataSetScalar > m_outData;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;
};

#endif  // WMIMAGEEXTRACTOR_H
