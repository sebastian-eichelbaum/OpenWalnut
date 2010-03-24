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

#ifndef WMDETERMINISTICFTMORI_H
#define WMDETERMINISTICFTMORI_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../kernel/WModuleInputData.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../common/math/WVector3D.h"
#include "../../common/math/WMatrix.h"

/**
 * This module implements the simple fiber tracking algorithm by Mori et al.
 * 
 * S. Mori, B. Crain, V. Chacko, and P. van Zijl, 
 * "Three-dimensional tracking of axonal projections in the brain by magnetic resonance imaging",
 * Annals of Neurology 45, pp. 265-269, 1999
 * 
 * \class WMDeterministicFTMori
 */
class WMDeterministicFTMori: public WModule
{
public:

    /**
     * Standard Constructor.
     */
    WMDeterministicFTMori();

    /**
     * Destructor.
     */
    virtual ~WMDeterministicFTMori();

    /**
     * Returns a new instance of this module.
     * \return A new instance of this module.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Return the name of this module.
     * \return The name of this module.
     */
    virtual const std::string getName() const;

    /**
     * Return the description of this module.
     * \return This module's description.
     */
    virtual const std::string getDescription() const;

protected:

    /**
     * The worker function, runs in its own thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the module's connectors.
     */
    virtual void connectors();

    /**
     * Initialize the module's properties.
     */
    virtual void properties();

    /**
     * Callback. This function gets called whenever the modules "active" state changes.
     */
    virtual void activate();

private:

    /**
     * Computes the largest eigenvector as well as the fractional anisotropy (FA)
     * for every position in the input dataset. The calculation is spread over
     * multiple threads.
     * 
     * \see WEigenThread
     */
    void doEigen();

    /**
     * Calculate fibers using the fiber tracking algorithm by Mori et al.
     * The calculation is spread over multiple threads.
     * 
     * \see WMoriThread
     * 
     * \param minFA The fractional anisotropy threshold.
     * \param minPoints Minimum number of points per fiber.
     * \param minCos The minimum cosine of the angle between two adjacent segments of a fiber.
     */
    void doMori( double const minFA, unsigned int const minPoints, double minCos );

    /**
     * A condition for property changes.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A pointer to the input tensor dataset.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * The output dataset. Stores all fibers extracted from the input tensor field.
     */
    boost::shared_ptr< WDataSetFibers > m_fiberSet;

    /**
     * The output Connector.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output;

    /**
     * The input Connector.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * Stores the eigenvectors extracted from the input tensor field.
     */
    boost::shared_ptr< std::vector< wmath::WVector3D > > m_eigenVectors;

    /**
     * Stores the fractional anisotropy of the input tensor field.
     */
    boost::shared_ptr< std::vector< double > > m_FA;

    /**
     * The minimum FA property.
     */
    WPropDouble m_minFA;

    /**
     * The minimum number of points property.
     */
    WPropInt m_minPoints;

    /**
     * The minimum cosine property.
     */
    WPropDouble m_minCos;

    /**
     * Run the algorithm.
     */
    WPropBool m_run;
};

#endif  // WMDETERMINISTICFTMORI_H
