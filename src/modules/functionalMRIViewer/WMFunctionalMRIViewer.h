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

#ifndef WMFUNCTIONALMRIVIEWER_H
#define WMFUNCTIONALMRIVIEWER_H

#include <string>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetTimeSeries.h"

/**
 * Views a time series at different points in time.
 *
 * \class WMFunctionalMRIViewer
 * \ingroup modules
 */
class WMFunctionalMRIViewer: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMFunctionalMRIViewer();

    /**
     * Standard destructor.
     */
    virtual ~WMFunctionalMRIViewer();

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

private:
    //! The current input.
    boost::shared_ptr< WDataSetTimeSeries > m_dataSet;

    //! The dataset at the current time.
    boost::shared_ptr< WDataSetScalar > m_dataSetAtTime;

    //! The input connector for the time series.
    boost::shared_ptr< WModuleInputData< WDataSetTimeSeries > > m_input;

    //! The output connector for the currently selected time slice.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    //! The current time.
    WPropDouble m_time;

    //! True, iff all textures should be scaled to the same intervall.
    WPropBool m_texScaleNormalized;

    //! A condition for property changes.
    boost::shared_ptr< WCondition > m_propCondition;
};

#endif  // WMFUNCTIONALMRIVIEWER_H
