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

#include <nifti1_io.h>

#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cmath>

#include "core/common/WAssert.h"
#include "core/common/WPathHelper.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/kernel/WKernel.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/kernel/WModule.h"

#include "WMWriteNIfTI.h"

WMWriteNIfTI::WMWriteNIfTI() :
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
                    "16 16 7 1", "   c None", ".  c #000080", "+  c #000000", "@  c #FFFF00", "#  c #E0E0E0",
                    "$  c #FFFFFF", "%  c #C0C0C0", "..+++++++++++..+", "..@@@@@@@@@@@..+", "..###########..+",
                    "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+",
                    "...............+", "....%%%%%%%....+", "....%..%%%%....+", "....%..%%%%....+", "....%..%%%%....+",
                    "....%..%%%%....+", "+...%%%%%%%....+", "++++++++++++++++"
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
    while( !m_shutdownFlag() )
    {
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if( !m_dataSet )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        writeToFile();
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMWriteNIfTI::connectors()
{
    // this module works for scalar data sets only so far
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle > (
                    shared_from_this(), "in", "The dataset to filter" ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMWriteNIfTI::properties()
{
    m_filename = m_properties->addProperty( "Filename", "Filename where to write the NIfTI file to.",
                                             WPathHelper::getAppPath() );
    m_saveTriggerProp = m_properties->addProperty( "Do save",  "Press!",
                                                  WPVBaseTypes::PV_TRIGGER_READY );
    m_saveTriggerProp->getCondition()->subscribeSignal( boost::bind( &WMWriteNIfTI::writeToFile, this ) );

    WModule::properties();
}

template< typename T > void WMWriteNIfTI::castData( void*& returnData )
{
    // cast valueset
    boost::shared_ptr< WValueSetBase > valsB = ( *m_dataSet ).getValueSet();
    boost::shared_ptr< WValueSet< T > > vals = boost::dynamic_pointer_cast< WValueSet< T > >( ( *m_dataSet ).getValueSet() );
    WAssert( vals, "Seems that value set type is not yet supported." );
    const size_t vDim = vals->dimension();
    // cast grid
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    const size_t countVoxels = grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ();
    WAssert( grid, "Seems that grid is of wrong type." );

    // copy data
    T* data = new T[vals->rawSize()];
    for( size_t i = 0; i < countVoxels; ++i )
    {
        for( size_t j = 0; j < vDim; ++j )
        {
            data[( j * countVoxels ) + i] = static_cast< T > ( vals->getScalar( i * vDim + j ) );
        }
    }
    returnData = static_cast< void* > ( data );
}

void WMWriteNIfTI::writeToFile()
{
    m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

    infoLog() << "Writing Data to " << m_filename->get().string();
    nifti_image *outField = nifti_simple_init_nim();

    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "Seems that grid is of wrong type." );

    size_t nbValues = ( *m_dataSet ).getValueSet()->size();

    outField->nx = grid->getNbCoordsX();
    outField->ny = grid->getNbCoordsY();
    outField->nz = grid->getNbCoordsZ();
    WAssert( grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() == nbValues,
             "Overall size incompatible with size in axis directions." );

    outField->nvox = nbValues*m_dataSet->getValueSet()->dimension();

    outField->dx = grid->getOffsetX();
    outField->dy = grid->getOffsetY();
    outField->dz = grid->getOffsetZ();

    outField->nifti_type = 1; // 1==NIFTI-1 (1 file)

    outField->freq_dim = 1;
    outField->phase_dim = 2;
    outField->slice_dim = 3;

    outField->qform_code = 1;
    outField->sform_code = 1;

    // TODO(philips): solve ticket 334
    // set time dimension to 1
    // wrong, according to nifti specs
    outField->nt = m_dataSet->getValueSet()->dimension();
    outField->dim[0] = 4;
    // right, according to nifti specs

    // wrong, according to nifti specs
    outField->ndim = 4;
    // right, according to nifti specs
    std::string description = m_dataSet->getName();
    // a description max. 80 char
    description.copy( outField->descrip, 80 );

    WMatrix< double > matrix = grid->getTransformationMatrix();
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            outField->qto_xyz.m[i][j] = matrix( i, j );
            outField->sto_xyz.m[i][j] = matrix( i, j );
        }
    }

    {
        float dx, dy, dz;
        nifti_mat44_to_quatern( outField->qto_xyz, &( outField->quatern_b ),
                                &( outField->quatern_c ), &( outField->quatern_d ),
                                &( outField->qoffset_x ), &( outField->qoffset_y ),
                                &( outField->qoffset_z ),
                                &dx, &dy, &dz,
                                &( outField->qfac ) );
    }

    outField->qto_ijk = nifti_mat44_inverse( outField->qto_xyz );
    outField->sto_ijk = nifti_mat44_inverse( outField->sto_xyz );

    void* data = 0;
    switch( ( *m_dataSet ).getValueSet()->getDataType() )
    {
        case W_DT_DOUBLE:
            outField->datatype = DT_DOUBLE;
            castData< double > ( data );
            outField->nbyper = 8;
            break;
        case W_DT_FLOAT:
            outField->datatype = DT_FLOAT;
            castData< float > ( data );
            outField->nbyper = 4;
            break;
        case W_DT_UNSIGNED_CHAR:
            outField->datatype = DT_UNSIGNED_CHAR;
            castData< uint8_t > ( data );
            outField->nbyper = 1;
            break;
        case W_DT_INT8:
            outField->datatype = DT_INT8;
            castData< int8_t > ( data );
            outField->nbyper = 1;
            break;
        case W_DT_UINT16:
            outField->datatype = DT_UINT16;
            castData< uint16_t > ( data );
            outField->nbyper = 2;
            break;
        case W_DT_INT16:
            outField->datatype = DT_INT16;
            castData< int16_t > ( data );
            outField->nbyper = 2;
            break;
        case W_DT_UINT32:
            outField->datatype = DT_UINT32;
            castData< uint32_t > ( data );
            outField->nbyper = 4;
            break;
        case W_DT_SIGNED_INT:
            outField->datatype = DT_SIGNED_INT;
            castData< int32_t > ( data );
            outField->nbyper = 4;
            break;
        default:
            WAssert( false, "Data set type not yet supported." );
    }
    outField->data = data;

    std::string s = m_filename->get().string();
    if( nifti_set_filenames( outField, s.c_str(), 0, 1 ) )
    {
        throw WException( std::string( "NIfTI filename Problem" ) );
    }

    nifti_image_write( outField );

    boost::shared_ptr< WDataSetRawHARDI > h = boost::dynamic_pointer_cast< WDataSetRawHARDI >( m_dataSet );
    if( h )
    {
        std::fstream f( ( s + ".bvec" ).c_str(), std::ios_base::out );
        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t k = 0; k < h->getNumberOfMeasurements(); ++k )
            {
                f << h->getGradient( k )[ i ] << " ";
            }
            f << std::endl;
        }
        f.close();
    }

    nifti_image_free( outField );
    infoLog() << "Writing data completed.";
}
