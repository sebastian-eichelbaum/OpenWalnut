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

#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/io/WWriterFiberVTK.h"
#include "WMFiberTransform.h"
#include "WMFiberTransform.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMFiberTransform )

WMFiberTransform::WMFiberTransform()
    : WModule(),
      m_recompute( new WCondition() )
{
}

WMFiberTransform::~WMFiberTransform()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMFiberTransform::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMFiberTransform() );
}

const char** WMFiberTransform::getXPMIcon() const
{
    return fiberTransform_xpm;
}

void WMFiberTransform::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    typedef WModuleInputData< WDataSetFibers > FiberInputData;  // just an alias
    m_fiberInput = boost::shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    typedef WModuleOutputData< WDataSetFibers > FiberOutputData;  // just an alias
    m_output = boost::shared_ptr< FiberOutputData >( new FiberOutputData( shared_from_this(), "fiberOutput", "The transformed fibers." ) );

    addConnector( m_fiberInput );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMFiberTransform::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    m_save     = m_properties->addProperty( "Save result", "If true the transformed fibers are saved to a file", false );
    m_savePath = m_properties->addProperty( "Save path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    m_run      = m_properties->addProperty( "Run", "Do the transformation", WPVBaseTypes::PV_TRIGGER_READY, m_recompute );
    m_run->get( true ); // reset so no initial run occurs
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );

    m_translationProp = m_properties->addProperty( "Translation",
                                                   "Translation part of the transformation. You need to press enter to make the values effective.",
                                                   WPosition( 0.0, 0.0, 0.0 ) );
    m_matrix0Prop = m_properties->addProperty( "M Row 0",
                                               "Row 0 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               WPosition( 1.0, 0.0, 0.0 ) );
    m_matrix1Prop = m_properties->addProperty( "M Row 1",
                                               "Row 1 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               WPosition( 0.0, 1.0, 0.0 ) );
    m_matrix2Prop = m_properties->addProperty( "M Row 2",
                                               "Row 2 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               WPosition( 0.0, 0.0, 1.0 ) );
    m_matrix3Prop = m_properties->addProperty( "M Row 3",
                                               "Row 3 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               WPosition( 0.0, 0.0, 0.0 ) );

    WModule::properties();
}

void WMFiberTransform::moduleMain()
{
    // When conditions are firing while wait() is not reached: wait terminates
    // and behaves as if the appropriate conditions have had fired. But it is
    // not detectable how many times a condition has fired.
    m_moduleState.setResetable();
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_recompute );



    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if( !m_fiberInput->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }
        if( m_rawDataset != m_fiberInput->getData() ) // in case data has changed
        {
            m_rawDataset = m_fiberInput->getData();
            WAssert( m_rawDataset, "Couldn't load dataset" );
        }

        WAssert( m_savePath, "No save path property" );
        if( m_savePath->get().string() == "/no/such/file" )
        {
            m_savePath->set( saveFileName( m_rawDataset->getFileName() ) );
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            update();
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        m_moduleState.wait();
    }
}

void WMFiberTransform::update()
{
    // get parameter from GUI (properties)
    bool   save = m_save->get();
    boost::filesystem::path savePath = m_savePath->get();

    // set the transformation matrix
    WMatrix< double > transformationMatrix( 4, 4 ); //!< matrix which is multiplied with each point to linear transform it.
    transformationMatrix( 0, 0 ) = m_matrix0Prop->get()[0];
    transformationMatrix( 0, 1 ) = m_matrix0Prop->get()[1];
    transformationMatrix( 0, 2 ) = m_matrix0Prop->get()[2];
    transformationMatrix( 0, 3 ) = m_translationProp->get()[0];

    transformationMatrix( 1, 0 ) = m_matrix1Prop->get()[0];
    transformationMatrix( 1, 1 ) = m_matrix1Prop->get()[1];
    transformationMatrix( 1, 2 ) = m_matrix1Prop->get()[2];
    transformationMatrix( 1, 3 ) = m_translationProp->get()[1];

    transformationMatrix( 2, 0 ) = m_matrix2Prop->get()[0];
    transformationMatrix( 2, 1 ) = m_matrix2Prop->get()[1];
    transformationMatrix( 2, 2 ) = m_matrix2Prop->get()[2];
    transformationMatrix( 2, 3 ) = m_translationProp->get()[2];

    transformationMatrix( 3, 0 ) = m_matrix3Prop->get()[0];
    transformationMatrix( 3, 1 ) = m_matrix3Prop->get()[1];
    transformationMatrix( 3, 2 ) = m_matrix3Prop->get()[2];
    transformationMatrix( 3, 3 ) = 1.0;

    boost::shared_ptr< WProgress > progress( new WProgress( "Transforming", 4 + save ) );
    m_progress->addSubProgress( progress );

    infoLog() << "Start: WDataSetFibers => WDataSetFiberVector";
    boost::shared_ptr< WDataSetFiberVector > dataset( new WDataSetFiberVector( m_rawDataset ) );
    infoLog() << "Stop:  WDataSetFibers => WDataSetFiberVector";
    ++*progress;

    //transform
    WValue< double > vec( 4 );
    WValue< double > vec_transformed( 4 );
    for( std::size_t fiberID = 0; fiberID < dataset->size(); ++fiberID )
    {
        WFiber& fiber = (*dataset)[fiberID];
        for( std::size_t positionID = 0; positionID < fiber.size(); ++positionID )
        {
            vec[0] = fiber[positionID][0];
            vec[1] = fiber[positionID][1];
            vec[2] = fiber[positionID][2];
            vec[3] = 1.0;
            vec_transformed = transformationMatrix * vec;
            vec_transformed = ( 1.0 / vec_transformed[3] ) * vec_transformed;
            fiber[positionID] = WPosition( vec_transformed[0], vec_transformed[1], vec_transformed[2] );
        }
    }
    ++*progress;

    infoLog() << "Start: WDataSetFibers <= WDataSetFiberVector";
    m_output->updateData( dataset->toWDataSetFibers() );
    infoLog() << "Stop:  WDataSetFibers <= WDataSetFiberVector";
    ++*progress;

    if( save )
    {
        WWriterFiberVTK w( savePath, true );
        w.writeFibs( dataset );
        ++*progress;
    }

    progress->finish();
}

boost::filesystem::path WMFiberTransform::saveFileName( std::string dataFileName ) const
{
    boost::filesystem::path fibFileName( dataFileName );
    return fibFileName.replace_extension( ".transformed.fib" );
}
inline const std::string WMFiberTransform::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return std::string( "Fiber Transform" );
}

inline const std::string WMFiberTransform::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return std::string( "Transforms a fiber dataset" );
}

