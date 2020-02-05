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

#ifndef WMSCREENCAPTURE_H
#define WMSCREENCAPTURE_H

#include <string>

#include <osg/Image>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/dataHandler/WDataSet.h"
#include "core/common/WConditionOneShot.h"

/**
 * Write a screenshot
 * \ingroup modules
 */
class WMScreenCapture : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMScreenCapture();

    /**
     * Destructor.
     */
    ~WMScreenCapture();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

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

private:
    boost::shared_ptr< WModuleInputData< WDataSet > > m_input;  //!< Input connector required by this module.

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    WPropFilename m_imageFile; //!< The file to save to.

    WPropInt m_captureDelay; //!< the time to wait between input update and screen grab

    WPropInt m_exitDelay; //!< the delay after capture to quit

    WPropBool m_unattendedOnly; //!< If true, only in unattended mode, the actions take place

    WPropFilename m_projFile; //!< The proj file to load before capture.
    WPropInt m_loadProjDelay;    //!< Load a file before capture?

    boost::signals2::connection m_imageConnection; //!< Screen capture callback subscription

    /**
     * A condition used to notify about the image being written.
     */
    boost::shared_ptr< WConditionOneShot > m_imageWrittenCondition;

    /**
     * Project loaded.
     */
    boost::shared_ptr< WConditionOneShot > m_projLoadedCondition;

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    /**
     * The currently handled data.
     */
    WDataSet::SPtr m_dataSetOld;

    /**
     * \ref WGEScreenCapture callback.
     */
    void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const;

    /**
     * Project got loaded.
     */
    void projLoadFinished( boost::filesystem::path file, std::vector< std::string > errors, std::vector< std::string > warnings );

};
#endif  // WMSCREENCAPTURE_H
