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

// C headers
// ...

// C++ headers
#include <fstream>
#include <iostream>
#include <string>

// External lib headers
#include <boost/shared_ptr.hpp>

// OW core headers
#include "core/common/WAssert.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/WPathHelper.h"
#include "core/common/WStringUtils.h"

// own, local headers
#include "WMWriteTransferFunction.xpm"
#include "WMWriteTransferFunction.h"

W_LOADABLE_MODULE( WMWriteTransferFunction )

WMWriteTransferFunction::WMWriteTransferFunction():
    WModule()
{
}

WMWriteTransferFunction::~WMWriteTransferFunction()
{
    // cleanup
}

boost::shared_ptr< WModule > WMWriteTransferFunction::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMWriteTransferFunction() );
}

const char** WMWriteTransferFunction::getXPMIcon() const
{
    return WMWriteTransferFunction_xpm;
}

const std::string WMWriteTransferFunction::getName() const
{
    return "Write Transfer Function";
}

const std::string WMWriteTransferFunction::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Allows to export transfer functions in several formats.";
}

void WMWriteTransferFunction::connectors()
{
    // an output connector for the transfer function created
    m_input = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(), "transferFunction1D", "The transfer function" );

    // call WModule's initialization
    WModule::connectors();
}

void WMWriteTransferFunction::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_savePath = m_properties->addProperty( "Save Location", "Set the path to the file.", WPathHelper::getAppPath() / "tf.txt" );
    m_saveAsRaw = m_properties->addProperty( "Save binary", "Save file in RAW binary format.", false, m_propCondition );
    m_saveTrigger = m_properties->addProperty( "Save", "Save to file.", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    // Call parent method
    WModule::properties();
}

void WMWriteTransferFunction::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    // lets go
    while( !m_shutdownFlag() )
    {
        m_saveTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_saveTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WDataSetSingle::SPtr d = m_input->getData();
            // valid data?
            if( !d )
            {
                warnLog() << "No data available to save.";
                continue;
            }

            // get value set and write it
            boost::shared_ptr< WValueSetBase > vsb = d->getValueSet();
            if( ( vsb->order() != 1 ) || ( vsb->dimension() != 4 ) || ( vsb->getDataType() != W_DT_UNSIGNED_CHAR ) )
            {
                errorLog() << "This is not a proper TF. Abort.";
                continue;
            }

            // we ensured that the valuesetbase is this valueset:
            boost::shared_ptr< WValueSet< unsigned char > > vs = boost::dynamic_pointer_cast< WValueSet< unsigned char > >( vsb );

            // valid path?
            boost::filesystem::path p = m_savePath->get( true );

            debugLog() << "Save TF to \"" + p.string() + "\".";

            if( !m_saveAsRaw->get( true ) )
            {
                std::ofstream f;
                f.open( p.string().c_str() );
                if( !f.good() )
                {
                    errorLog() << "Failed to open file. Abort.";
                    continue;
                }

                f << "# Exported using OpenWalnut. http://www.openwalnut.org" << std::endl;
                f << "# TF export format:" << std::endl <<
                     "# Comments begin with #" << std::endl <<
                     "# 1st line: width height" << std::endl <<
                     "# Then, RGBA quadruples; line-wise; values space separated; in x direction first." << std::endl;

                // later, we might support 2d TFs. Now, write 1 as second dimension
                f << string_utils::toString( vs->size() ) << " " << "1" << std::endl;
                // go through each RGBA vector
                for( size_t i = 0; i < vs->size(); ++i )
                {
                    f << string_utils::toString( vs->getScalar( i * 4 + 0 ) ) << " "
                      << string_utils::toString( vs->getScalar( i * 4 + 1 ) ) << " "
                      << string_utils::toString( vs->getScalar( i * 4 + 2 ) ) << " "
                      << string_utils::toString( vs->getScalar( i * 4 + 3 ) ) << std::endl;
                }

                // done.
                f.close();
            }
            else
            {
                // write header file separately
                std::ofstream fhead;
                fhead.open( ( p.string() + ".header" ).c_str() );
                if( !fhead.good() )
                {
                    errorLog() << "Failed to write header file " <<  ( p.string() + ".header" ).c_str() << ". Abort.";
                    continue;
                }

                fhead << "# Exported using OpenWalnut. http://www.openwalnut.org" << std::endl;
                fhead << "# TF export format:" << std::endl <<
                         "# Comments begin with #" << std::endl <<
                         "# 1st line: width height" << std::endl <<
                         "# 2nd line: filename to binary file, relative to header" << std::endl <<
                         "# The RGBA quadruples are stored in binary format as four unsigned char values per quadruples." << std::endl;

                // later, we might support 2d TFs. Now, write 1 as second dimension
                fhead << string_utils::toString( vs->size() ) << " " << "1" << std::endl;
                fhead << p.filename().string().c_str() << std::endl;
                // go through each RGBA vector

                fhead.close();

                // write data file.
                std::ofstream f;
                f.open( p.string().c_str(), std::ios::out | std::ios::binary );
                if( !f.good() )
                {
                    errorLog() << "Failed to open file. Abort.";
                    continue;
                }

                f.write( reinterpret_cast< const char*>( vs->rawData() ), vs->size() );

                // done.
                f.close();
            }
            // done
            m_saveTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        }
    }
}

