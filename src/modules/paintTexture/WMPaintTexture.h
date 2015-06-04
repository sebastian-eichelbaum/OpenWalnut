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

#ifndef WMPAINTTEXTURE_H
#define WMPAINTTEXTURE_H

#include <queue>
#include <string>
#include <vector>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WValueSet.h"

#include "core/graphicsEngine/WGETexture.h"

/**
 * This module allows mark areas in a 3D texture by "painting"
 * on pickable surfaces.
 *
 * \ingroup modules
 */
class WMPaintTexture: public WModule
{
public:
    /**
     *
     */
    WMPaintTexture();

    /**
     *
     */
    virtual ~WMPaintTexture();

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
     * Get the icon for this module in XPM format.
     * \return The icon.
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
     * this function listens to the pick handler, if the paint flag is true it will write into the out texture
     */
    void doPaint();

    /**
     * this function listens to the pick handler, if the paint flag is true it will add the paint position to the
     * paint queue
     *
     * \param pickInfo the pickInfo object fromt he current pick
     */
    void queuePaint( WPickInfo pickInfo );

    /**
     * creates a new texture
     */
    void createTexture();

    /**
     * updates the output connector
     */
    void updateOutDataset();

    /**
     * copies data from the input dataset into the paint texture
     */
    void copyFromInput();

    /**
     * creates a ROI from the currently selected paint value
     */
    void createROI();

    /**
     * get the paint index at the picked voexel fromt he out texture and sets m_paintIndex accordingly
     *
     * \param pickInfo the pickInfo object for the pick
     */
    void setColorFromPick( WPickInfo pickInfo );

    /**
     * Interpolation?
     */
    WPropBool m_painting;

    /**
     * A list of pencil sizes and shapes
     */
    boost::shared_ptr< WItemSelection > m_pencilSelectionsList;

    /**
     * Selection property for pencil size and shape
     */
    WPropSelection m_pencilSelection;

    /**
     * specifies the value we paint into the output texture
     */
    WPropInt m_paintIndex;

    /**
     * A list of color map selection types
     */
    boost::shared_ptr< WItemSelection > m_colorMapSelectionsList;

    /**
     * Selection property for color map
     */
    WPropSelection m_colorMapSelection;

    /**
     * true when a new paint coordinate is added to the queue
     */
    WPropBool m_queueAdded;

    /**
     * new paint coordinates get added here
     */
    std::queue< WPickInfo >m_paintQueue;

    /**
     * An input connector that accepts order 1 datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * An output connector for the output scalar dsataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * Point to the out dataset once it is invalid. Used to deregister from the datahandler
     */
    boost::shared_ptr< WDataSetScalar > m_outDataOld;

    /**
     * This is a pointer to the current output.
     */
    boost::shared_ptr< WDataSetScalar > m_outData;

    /**
     * stores a pointer to the texture we paint in
     */
    osg::ref_ptr< WGETexture3D > m_texture;

    /**
     * stores a pointer to the grid we use;
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * copies the input dataset into the paint texture, this allows one to continue work
     */
    WPropTrigger m_buttonCopyFromInput;

    /**
     * updates the output connector on demand, as we don't want to do this every paint command
     */
    WPropTrigger m_buttonUpdateOutput;

    /**
     * updates the output connector on demand, as we don't want to do this every paint command
     */
    WPropTrigger m_buttonCreateRoi;
};

#endif  // WMPAINTTEXTURE_H
