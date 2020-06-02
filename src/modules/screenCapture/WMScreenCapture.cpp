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


#include <string>

#include <osg/Image>
#include <osgDB/WriteFile>

#include "core/common/WAssert.h"
#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WKernel.h"
#include "WMScreenCapture.h"
#include "WMScreenCapture.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMScreenCapture )

WMScreenCapture::WMScreenCapture():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMScreenCapture::~WMScreenCapture()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMScreenCapture::factory() const
{
    return boost::shared_ptr< WModule >( new WMScreenCapture() );
}

const char** WMScreenCapture::getXPMIcon() const
{
    return WMScreenCapture_xpm;
}

const std::string WMScreenCapture::getName() const
{
    return "Screen Capture";
}

const std::string WMScreenCapture::getDescription() const
{
    return "Record the screen on input update.";
}

void WMScreenCapture::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    m_imageWrittenCondition = boost::shared_ptr< WConditionOneShot >( new WConditionOneShot() );
    m_projLoadedCondition = boost::shared_ptr< WConditionOneShot >( new WConditionOneShot() );
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";
        m_moduleState.wait();

        boost::shared_ptr< WTransferable > dataSet = m_input->getData();
        bool dataValid = ( dataSet != NULL );
        bool dataChanged = ( dataSet != m_dataSetOld );
        m_dataSetOld = dataSet;

        if( !dataValid )
        {
            continue;
        }

        // do nothing if these are still the same data
        if( !dataChanged )
        {
            continue;
        }

        if( m_unattendedOnly->get() && !WKernel::getRunningKernel()->isUnattendedMode() )
        {
            debugLog() << "Data received but not in unattended mode as expected. Not doing anything.";
            continue;
        }

        if( m_skip->get() )
        {
            debugLog() << "Skip " << m_imageFile->get().string();

            // How to set the data to the output and how to notify other modules about it?
            m_output->updateData( boost::shared_ptr<WDataSetSingle>( new WDataSetSingle() ) );

            handleExit(m_exitDelay->get());
            continue;
        }

        boost::shared_ptr< WProgress > progress( new WProgress( "Doing work", 6 ) );
        m_progress->addSubProgress( progress );

        debugLog() << "Data received.";

        if( m_loadProjDelay->get() >= 0 )
        {
            debugLog() << "Load project in " << m_loadProjDelay->get();
            msleep( 1000 * m_loadProjDelay->get() );
            debugLog() << "Load project " << m_projFile->get().string();
            WProjectFile::SPtr proj( new WProjectFile( m_projFile->get(), boost::bind( &WMScreenCapture::projLoadFinished, this, _1, _2, _3 ) ) );
            proj->load();
            m_projLoadedCondition->wait();
            debugLog() << "Load project done.";
        }
        ++*progress;

        debugLog() << "Start capture in " << m_captureDelay->get();
        if( m_captureDelay->get() < 0 )
        {
            progress->finish();
            continue;
        }
        ++*progress;

        msleep( 1000 * m_captureDelay->get() );
        ++*progress;
        debugLog() << "Start capture. Waiting for image";
        m_imageConnection = viewer->getScreenCapture()->subscribeSignal( boost::bind( &WMScreenCapture::handleImage, this, _1, _2, _3 ) );
        viewer->getScreenCapture()->screenshot();
        ++*progress;
        m_imageWrittenCondition->wait();
        m_imageConnection.disconnect();
        ++*progress;

        debugLog() << "Captured. Exit in " << m_exitDelay->get();

        // How to set the data to the output and how to notify other modules about it?
        m_output->updateData( boost::shared_ptr<WDataSetSingle>( new WDataSetSingle() ) );

        ++*progress;
        handleExit( m_exitDelay->get() );
        progress->finish();
    }
}

void WMScreenCapture::handleExit(int delay)
{
    if( delay < 0 )
    {
        return;
    }

    msleep( 1000 * delay );
    WKernel::getRunningKernel()->quit( true );
}

void WMScreenCapture::handleImage( size_t /* framesLeft */, size_t /* totalFrames */,
                                   osg::ref_ptr< osg::Image > image ) const
{
    debugLog() << "Received capture. Writing to " << m_imageFile->get().string();
    osgDB::writeImageFile( *image, m_imageFile->get().string() );
    m_imageWrittenCondition->notify();
}

void WMScreenCapture::projLoadFinished( boost::filesystem::path /* file */, std::vector< std::string > /* errors */,
                                        std::vector< std::string > /* warnings */ )
{
    m_projLoadedCondition->notify();
}

void WMScreenCapture::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WTransferable  > >(
        new WModuleInputData< WTransferable >( shared_from_this(),
                                                               "in", "The dataset to show" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    m_output = WModuleOutputData < WDataSetSingle  >::createAndAdd( shared_from_this(), "out", "Dummy data to notify about finished calculations." );

    // call WModules initialization
    WModule::connectors();
}

void WMScreenCapture::properties()
{
    m_unattendedOnly = m_properties->addProperty( "Unattended Only", "If true, actions take place in unattended mode only. Refer to commandline params.", true );
    m_imageFile = m_properties->addProperty( "File", "Where to write the image to", WPathHelper::getAppPath() / "screenCapture.png" );

    m_captureDelay = m_properties->addProperty( "Capture Delay [ms]", "Time to wait till capture after input update. Negative for never.", -1 );
    m_captureDelay->setMin( -1 );
    m_captureDelay->setMax( 10000 );

    m_exitDelay = m_properties->addProperty( "Exit Delay [ms]", "Time to wait till exit after capture. Negative for never.", -1 );
    m_exitDelay->setMin( -1 );
    m_exitDelay->setMax( 10000 );

    m_loadProjDelay = m_properties->addProperty( "Load Project Delay [ms]", "Time to wait till project load. Negative for never.", -1 );
    m_loadProjDelay->setMin( -1 );
    m_loadProjDelay->setMax( 10000 );
    m_projFile = m_properties->addProperty( "Project File", "Load this file before capture.", WPathHelper::getAppPath() / "camera.owp" );

    m_skip = m_properties->addProperty( "Skip", "Allows to skip this capture.", false );

    WModule::properties();
}

