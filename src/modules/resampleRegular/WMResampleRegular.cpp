//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#include <string>

#include "core/kernel/WKernel.h"

#include "WMResampleRegular.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMResampleRegular )

WMResampleRegular::WMResampleRegular():
    WModule(),
    resampleStepSize( 2 )
{
}

WMResampleRegular::~WMResampleRegular()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMResampleRegular::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMResampleRegular() );
}

const std::string WMResampleRegular::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Resample Regular";
}

const std::string WMResampleRegular::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "No documentation yet. "
    "The best person to ask for documenation is probably the modules's creator, i.e. the user with the name \"wiebel\".";
}

void WMResampleRegular::connectors()
{
    m_original = boost::shared_ptr<WModuleInputData<WDataSetScalar> >(
        new WModuleInputData<WDataSetScalar> ( shared_from_this(), "Original",
                                               "The dataset to resample." ) );
    addConnector( m_original );

    m_resampled = boost::shared_ptr<WModuleOutputData<WDataSetScalar> >(
        new WModuleOutputData<WDataSetScalar> ( shared_from_this(), "Resampled",
                                                "The resampled data set." ) );
    addConnector( m_resampled );

    WModule::connectors();
}

void WMResampleRegular::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    WModule::properties();
}

void WMResampleRegular::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMResampleRegular::moduleMain()
{

    m_moduleState.add( m_original->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {

        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetScalar > originalData = m_original->getData();

        // If no data found go into waiting state again.
        if( !originalData )
        {
            continue;
        }


        boost::shared_ptr<WGridRegular3D> grid = boost::dynamic_pointer_cast< WGridRegular3D >( originalData->getGrid() );

        size_t nX = grid->getNbCoordsX();
        size_t nY = grid->getNbCoordsY();
        size_t nZ = grid->getNbCoordsZ();

        boost::shared_ptr< WGrid > resampledGrid;
        resampledGrid = boost::shared_ptr< WGridRegular3D >(
            new WGridRegular3D( nY/resampleStepSize, nY/resampleStepSize, nZ/resampleStepSize ) );
        // TODO( wiebel ): adapt transformation above

        boost::shared_ptr<WValueSetBase> vals;
        vals = boost::dynamic_pointer_cast<WValueSetBase >( originalData->getValueSet() );

        boost::shared_ptr< std::vector< float > > theValues;
        theValues =  boost::shared_ptr< std::vector< float > >( new std::vector<float>() );

        for( size_t idZ = 1; idZ < nZ; idZ += resampleStepSize )
        {
            for( size_t idY = 1; idY < nY; idY += resampleStepSize )
            {
                for( size_t idX = 1; idX < nX; idX += resampleStepSize )
                {
                    theValues->push_back( static_cast<float>(originalData->getValueAt( idX, idY, idZ ) ) );
                }
            }
        }

        boost::shared_ptr< WValueSet< float > >  newValueSet;
        newValueSet = boost::shared_ptr< WValueSet< float > >( new WValueSet<float>( vals->order(), vals->dimension(), theValues ) );

        m_resampled->updateData( boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, resampledGrid ) ) );
    }
}
