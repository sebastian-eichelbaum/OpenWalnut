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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cmath>

#include "../../common/WStringUtils.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../kernel/WKernel.h"
#include "../../math/WPosition.h"
#include "../../math/WVector3D.h"
#include "../data/WMData.h"
#include "../../dataHandler/io/nifti/nifti1_io.h"
#include "WMWriteNIfTI.h"

WMWriteNIfTI::WMWriteNIfTI():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMWriteNIfTI::~WMWriteNIfTI()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMWriteNIfTI::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteNIfTI() );
}

const char** WMWriteNIfTI::getXPMIcon() const
{
    static const char * disc_xpm[] =
        {
            "16 16 7 1",
            "   c None",
            ".  c #000080",
            "+  c #000000",
            "@  c #FFFF00",
            "#  c #E0E0E0",
            "$  c #FFFFFF",
            "%  c #C0C0C0",
            "..+++++++++++..+",
            "..@@@@@@@@@@@..+",
            "..###########..+",
            "..$$$$$$$$$$$..+",
            "..###########..+",
            "..$$$$$$$$$$$..+",
            "..###########..+",
            "..$$$$$$$$$$$..+",
            "...............+",
            "....%%%%%%%....+",
            "....%..%%%%....+",
            "....%..%%%%....+",
            "....%..%%%%....+",
            "....%..%%%%....+",
            "+...%%%%%%%....+",
            "++++++++++++++++"
        };
    return disc_xpm;
}

const std::string WMWriteNIfTI::getName() const
{
    return "Write NIfTI";
}

const std::string WMWriteNIfTI::getDescription() const
{
    return "Writes a data set to a NIfTI file.";
}

void WMWriteNIfTI::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if ( !m_dataSet )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMWriteNIfTI::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to filter" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMWriteNIfTI::properties()
{
    m_filename = m_properties2->addProperty( "Filename", "Filename where to write the nifty file to.", WKernel::getAppPathObject() );

    // bind a custom callback, triggered whenever the filename changes
    m_filename->getCondition()->subscribeSignal( boost::bind( &WMWriteNIfTI::writeToFile, this ) );
}

void WMWriteNIfTI::writeToFile()
{
    infoLog() << "Writing Data to " << m_filename->get().file_string();
    nifti_image *outField = nifti_simple_init_nim();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    assert( grid && "Seems that grid is of wrong type." );

    boost::shared_ptr< WValueSet< uint8_t > > vals = boost::shared_dynamic_cast< WValueSet< uint8_t > >( ( *m_dataSet ).getValueSet() );
    assert( vals && "Seems that value set is of wrong type." );

    size_t nbValues = vals->size();

    outField->nx = grid->getNbCoordsX();
    outField->ny = grid->getNbCoordsY();
    outField->nz = grid->getNbCoordsZ();
    assert( grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() == nbValues );

    // TODO(wiebel): only able to handle scalars now.
    outField->nt = 1;
    outField->nvox = nbValues;

    outField->dx = grid->getOffsetX();
    outField->dy = grid->getOffsetY();
    outField->dz = grid->getOffsetZ();

    outField->nifti_type = 1;  // 1==NIFTI-1 (1 file)

    outField->freq_dim = 1;
    outField->phase_dim = 2;
    outField->slice_dim = 3;

    outField->qform_code = 1;


    outField->datatype = DT_UNSIGNED_CHAR;
    outField->nbyper = 1;
    uint8_t* data = new uint8_t[nbValues];
    for( unsigned int i = 0; i < nbValues; ++i )
    {
        data[i] = static_cast<uint8_t>( vals->getScalar( i ) );
    }

    outField->data = static_cast<void*>( data );

    if( nifti_set_filenames( outField, m_filename->get().file_string().c_str(), 0, 1 ) )
    {
        throw WException( "NIfTI filename Problem" );
    }

    nifti_image_write( outField );
    nifti_image_free( outField );
    infoLog() << "Writing data completed.";
}
