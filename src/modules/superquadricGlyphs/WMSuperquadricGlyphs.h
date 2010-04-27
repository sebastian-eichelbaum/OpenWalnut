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

#ifndef WMSUPERQUADRICGLYPHS_H
#define WMSUPERQUADRICGLYPHS_H

#include <string>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../dataHandler/WDataSetSingle.h"

#include "../../graphicsEngine/WGEGroupNode.h"

/**
 * Rendering of GPU bases Superquadric Glyphs. These glyphs are completely raytraced on the GPU.
 *
 * \ingroup modules
 */
class WMSuperquadricGlyphs: public WModule
{
public:

    /**
     * Constructor.
     */
    WMSuperquadricGlyphs();

    /**
     * Destructor.
     */
    virtual ~WMSuperquadricGlyphs();

    /**
     * Gives back the name of this module.
     *
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     *
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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * The Geode containing all the glyphs. In fact it only contains a quad per glyph on which the raytracing is done.
     */
    osg::ref_ptr< osg::Geode > m_outputGeode;

    /**
     * The input dataset. It contains the second order tensor data needed here.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * The current tensor dataset.
     */
    boost::shared_ptr< const WDataSetSingle > m_dataSet;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z
};

#endif  // WMSUPERQUADRICGLYPHS_H
