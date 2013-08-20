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

#include <string>
#include <fstream>

#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"
#include "WMWriteRawData.xpm"

#include "WMWriteRawData.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMWriteRawData )

WMWriteRawData::WMWriteRawData():
    WModule()
{
}

WMWriteRawData::~WMWriteRawData()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMWriteRawData::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteRawData() );
}

const char** WMWriteRawData::getXPMIcon() const
{
    return WMWriteRawData_xpm; // Please put a real icon here.
}
const std::string WMWriteRawData::getName() const
{
    return "Write Raw Data";
}

const std::string WMWriteRawData::getDescription() const
{
    return "Write scalar data defined on uniform lattices"
        "in raw format, i.e., plain three-dimensional arrays of data. Data format is defined by data type of dataset (float, byte, ...).";
}

void WMWriteRawData::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetScalar > >(
            new WModuleInputData< WDataSetScalar >( shared_from_this(), "Data", "The data to write." ) );

    addConnector( m_input );

    WModule::connectors();
}

void WMWriteRawData::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "File", "", WPathHelper::getAppPath(), m_propCondition );

    WModule::properties();
}

void WMWriteRawData::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

/**
 * Visitor for discriminating the type of the first valueset.
 */
class VisitorVSet: public boost::static_visitor<>
{
public:
    /**
     * Create visitor instance and convert it to the given input type
     */
    explicit VisitorVSet( boost::filesystem::path fn ):
        boost::static_visitor<>(),
        m_filename( fn )
    {
    }

    /**
     * Called by boost::varying during static visiting. Creates new, converted valueset
     *
     * \tparam T the real integral type of the first value set.
     * \param vals the first valueset currently visited.
     *
     * \return the result from the operation with this and the second value set
     */
    template < typename T >
    void operator()( const WValueSet< T >* const& vals ) const             // NOLINT
    {
        try
        {
            std::ofstream file( m_filename.string().c_str(), std::ios::out | std::ofstream::binary );
            file.write( reinterpret_cast<const char *>( vals->rawData() ) , sizeof( T ) * vals->rawSize() );
            file.close();
        }
        catch( int e )
        {
        }
    }
private:
    /**
     * Where to write
     */
    boost::filesystem::path m_filename;
};

void WMWriteRawData::moduleMain()
{
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_input->getDataChangedCondition() );
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            debugLog() << "hallo";
            break;
        }

        // acquire data from the input connector
        bool dataSetChanged = !( m_dataSet == m_input->getData() );
        m_dataSet = m_input->getData();
        if( m_dataSet && ( dataSetChanged || m_dataFile->changed() ) )
        {
            debugLog() << "Writing " << m_dataFile->get().string() << ".";
            boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Write File", 2 ) );
            m_progress->addSubProgress( progress );

            // call visitor for this job
            VisitorVSet visitor( m_dataFile->get( true ) );
            m_dataSet->getValueSet()->applyFunction( visitor );

            progress->finish();
        }
    }
}

