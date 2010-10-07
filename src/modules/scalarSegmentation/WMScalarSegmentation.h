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

#ifndef WMSCALARSEGMENTATION_H
#define WMSCALARSEGMENTATION_H

#include <string>
#include <vector>

#include "../../common/WItemSelection.h"
#include "../../common/WItemSelector.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../dataHandler/WDataSetScalar.h"

/**
 * First version of a module that implements 3D-image segmentation algorithms.
 *
 * \ingroup modules
 */
class WMScalarSegmentation: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMScalarSegmentation();

    /**
     * Destructor.
     */
    virtual ~WMScalarSegmentation();

    /**
     * Return the name of this module.
     *
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Return a description of this module.
     *
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Return a new instance of this module.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return the icon.
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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * Do a segmentation depending on the current module property values.
     *
     * \return the resulting segmented dataset.
     */
    void doSegmentation();

    /**
     * An input connector used to get datasets from other modules.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * This is a pointer to the segmented dataset.
     */
    boost::shared_ptr< WDataSetScalar > m_result;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The number of the currently selected segmentation method.
     */
    std::size_t m_algoIndex;

    /**
     * A list of possible segmentation algorithms.
     */
    boost::shared_ptr< WItemSelection > m_algos;

    /**
     * A selection box for segmentation algorithms.
     */
    WPropSelection m_algoType;

    /**
     * The threshold property for the simple threshold segmentation.
     */
    WPropDouble m_threshold;

    //! A function object that implements a simple threshold segmentation.
    class SimpleSegmentation : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param t The threshold for segmentation.
         */
        SimpleSegmentation( double t ) // NOLINT
            : m_threshold( t )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \param valueset The valueset to segment.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* valueset ) const;

    private:
        //! the threshold
        double m_threshold;
    };
};

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::SimpleSegmentation::operator() ( WValueSet< T > const* valueset ) const
{
    std::vector< T > values( valueset->size() );

    for( std::size_t k = 0; k < valueset->size(); ++k )
    {
        values[ k ] = ( static_cast< double >( valueset->getScalar( k ) ) <
                        m_threshold * ( valueset->getMaximumValue() - valueset->getMinimumValue() ) / 100.0 + valueset->getMinimumValue()
                        ? static_cast< T >( 0 ) : static_cast< T >( 1 ) );
    }
    return boost::shared_ptr< WValueSet< T > >( new WValueSet< T >( 0, 1, values, DataType< T >::type ) );
}

#endif  // WMSCALARSEGMENTATION_H
