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

#ifndef WMHARDITOSPHERICALHARMONICS_H
#define WMHARDITOSPHERICALHARMONICS_H

#include <map>
#include <string>
#include <vector>


#include "core/common/WThreadedRunner.h"
#include "core/common/math/WMatrix.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WGridRegular3D.h"

#include "WSphericalHarmonicsCoefficientsThread.h"


class WSphericalHarmonicsCoefficientsThread;
/**
 * Module for the creation of a spherical harmonic data set from raw HARDI data.
 * \ingroup modules
 */
class WMHARDIToSphericalHarmonics : public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMHARDIToSphericalHarmonicsTest;

public:
    /**
     * Standard constructor.
     */
    WMHARDIToSphericalHarmonics();

    /**
     * Destructor.
     */
    ~WMHARDIToSphericalHarmonics();

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
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    // TODO(philips): can we move this method to private or at least protected
    /**
     * Stop the threads.
     */
    void stopThreads();

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

    WPropInt m_order; //!< Property holding the order of the spherical harmonics

    WPropDouble m_regularisationFactorLambda; //!< Property holding the regularisation factor lambda

    WPropBool m_doFunkRadonTransformation; //!< Property indicating whether to do the Funk-Radon-transformation with calculated spherical harmonics.

    WPropBool m_doErrorCalculation; //!< Property indicating whether a reprojection error of the spherical harmonics is calculated.

    /**
     * Property indicating whether the reprojection error (measurement relative to spherical harmonic calculation) is stored into a seperate dataset.
     */
    WPropBool m_doResidualCalculation;

    /**
     * Property indicating whether the spherical harmonics calculation is done multithreaded.
     */
    WPropBool m_multiThreaded;

    /**
     * Property indicating whether the measurements are normalized from 0 to 1 for each voxel.
     */
    WPropBool m_doNormalisation;

    /**
     * Pointer to the spherical harmonics calculation threads.
     */
    std::vector< WSphericalHarmonicsCoefficientsThread* > m_threads;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetRawHARDI > m_dataSet;

    /**
     * Input connector required by this module. The HARDI measurements.
     */
    boost::shared_ptr< WModuleInputData< WDataSetRawHARDI > > m_input;

    /**
     * Output connector provided by this module. The calculated spherical harmonics.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSphericalHarmonics > > m_output;

    /**
     * The reprojection error for each measurement.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetRawHARDI > > m_outputResiduals;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * This class is derived from PropertyConstraint and ensures that only even values are valid.
     */
    class evenInt: public WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint
    {
        /**
         * You need to overwrite this method. It decides whether the specified new value should be accepted or not.
         *
         * \param property the property thats going to be changed.
         * \param value the new value
         *
         * \return true if the new value is OK.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >  property, WPVBaseTypes::PV_INT value );

        /**
         * Method to clone the constraint and create a new one with the correct dynamic type.
         *
         * \return the constraint.
         */
        virtual boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint > clone();
    };
};


#endif  // WMHARDITOSPHERICALHARMONICS_H
