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

#ifndef WMANISOTROPICFILTERING_H
#define WMANISOTROPICFILTERING_H

#include <string>
#include <vector>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WValueSet.h"

/**
 * This module smoothes images of a dataset while preserving edges.
 *
 * \ingroup modules
 */
class WMAnisotropicFiltering : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMAnisotropicFiltering();

    /**
     * Destructor.
     */
    virtual ~WMAnisotropicFiltering();

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
     * Calculates the resulting smoothed image.
     *
     * \param iterations The number of diffusion time steps.
     */
    void calcSmoothedImages( int iterations );

    /**
     * Calculates grid indices from voxel coords.
     *
     * \param grid The grid.
     * \param x The x-coord.
     * \param y The y-coord.
     * \param z The z-coord.
     *
     * \return The voxel index.
     */
    std::size_t coordsToIndex( boost::shared_ptr< WGridRegular3D > const& grid,
                               std::size_t x, std::size_t y, std::size_t z );

    /**
     * Copy the datasets image data to a temp array.
     *
     * \param smoothed The temp array to copy to.
     * \param grid The grid, which is not used in this function.
     */
    void copyData( boost::shared_ptr< std::vector< double > >& smoothed,  // NOLINT non-const ref
                   boost::shared_ptr< WGridRegular3D > const& grid );

    /**
     * Calculates an array containing the derivations in x, y and z directions of the image
     * intensity (i.e. contains the intensity gradient).
     *
     * \param deriv The memory used for the gradient values.
     * \param smoothed The intensity data.
     * \param grid The grid.
     * \param image The index of the image to calc the gradient from.
     * \param numImages The number of images in this dataset.
     */
    void calcDeriv( std::vector< double >& deriv, boost::shared_ptr< std::vector< double > > const& smoothed,  // NOLINT non-const ref
                    boost::shared_ptr< WGridRegular3D > const& grid, std::size_t image, std::size_t numImages );

    /**
     * Calculates the diffusion coeff for every voxel.
     *
     * \param coeff The memory used for the coeff data.
     * \param deriv The gradient data.
     * \param grid The grid.
     */
    void calcCoeff( std::vector< double >& coeff, std::vector< double > const& deriv,  // NOLINT non-const ref
                    boost::shared_ptr< WGridRegular3D > const& grid );

    /**
     * Do the diffusion.
     *
     * \param deriv The intensity gradient data.
     * \param coeff The diffusion coeffs.
     * \param smoothed The new smoothed image.
     * \param grid The grid.
     * \param image The index of the image to calc the gradient from.
     * \param numImages The number of images in this dataset.
     */
    void diffusion( std::vector< double > const& deriv, std::vector< double > const& coeff,
                    boost::shared_ptr< std::vector< double > >& smoothed,  // NOLINT non-const ref
                    boost::shared_ptr< WGridRegular3D > const& grid, std::size_t image, std::size_t numImages );

    /**
     * An input connector that accepts multi image datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * An output connector for the output dataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    //! The edge preservation parameter used for diffusion coeff calculation.
    double m_k;

    //! The size of the time steps.
    double m_d;

    //! A property for the number of iterations.
    WPropInt m_iterations;

    //! A property for the edge preservation parameter.
    WPropDouble m_Kcoeff;

    //! A property for the time step size.
    WPropDouble m_delta;
};

#endif  // WMANISOTROPICFILTERING_H
