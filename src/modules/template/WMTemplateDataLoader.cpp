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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a tutorial on how to create custom data loading modules.
//
//  You will need the knowledge of these tutorials before you can go on:
//  * WMTemplate
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "core/kernel/WDataModuleInputFile.h"
#include "core/kernel/WDataModuleInputFilterFile.h"
#include "core/kernel/WKernel.h"

#include "WMTemplateDataLoader.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ... You already know this from WMTemplate.
//
// BUT: there is one difference: you derive from WDataModule instead of WModule. Accordingly, you have to change the constructor initializer list a
// bit. WDataModule itself is derived from WModule, so you basically have a module with some additional functionality to load data. Also the
// life-cycle of WDataModule is the same. But it is important to understand the way OpenWalnut starts WDataModules. So we assume, the user wants to
// load something. He either drags a file into the OpenWalnut window, uses the file-open dialog or simply starts your WDataModule:
//
// 1) OpenWalnut uses the method getInputFilter of the prototype of your WDataModule. The module is NOT instanceiated at this point.
//  -> return a list of filters that define the allowed types of input
// 2) OpenWalnut creates an instance of your WDataModule whenever one of the input filters match
//  -> this happens exactly as you know it from WModule
//  -> your module starts to run
// 3) The final step is that OpenWalnut sets the WDataModuleInput, which then triggers your handleInputChange method
//  -> this is done from another thread, not your module-thread!
//   --> so this happens sooner or later.
//  -> use this to notify your module thread to load something
//  -> this might be done multiple times (the user changes the file for the module)
//   --> write proper code to support loading multiple times!
//
// Thats it. To summarize: OW asks your module for a list of input filters. These filters are used to find a proper WDataModuleInput. OW creates
// your module, adds it to the module root container, starts it and sets the WDataModuleInput sooner or later.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateDataLoader::WMTemplateDataLoader()
    : WDataModule() // NOTE: thats new.
{
}

WMTemplateDataLoader::~WMTemplateDataLoader()
{
}

boost::shared_ptr< WModule > WMTemplateDataLoader::factory() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return boost::shared_ptr< WModule >( new WMTemplateDataLoader() );
}

const std::string WMTemplateDataLoader::getName() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Template Data Loader";
}

const std::string WMTemplateDataLoader::getDescription() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Load a dummy file from disk.";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateDataLoader::connectors()
{
    // As WDataModule is also a WModule, you can create output connectors as you are used to.
    // Data modules of course should have at least one output connectors. This is not enforced, which might come in handy
    // when loading data directly to the colormapper without providing it to other modules.

    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    m_output = WModuleOutputData < WDataSetScalar  >::createAndAdd( shared_from_this(), "out", "The loaded dataset" );
    WDataModule::connectors();
}

void WMTemplateDataLoader::properties()
{
    // Again, as WDataModule is a standard module, you can define arbitrary properties here. They can modify the loaded data
    // or something similar. But be aware that these properties are not shown to the user before the data module was added and
    // got its filename/path/url/whatever to load. So it is not useful for configure the loader before loading. There is another
    // way to define properties which are used before creating and starting the WDataModule. Read on!

    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    // Add your properties as you are used to:
    // m_enableFeature    = m_properties->addProperty( "Enable feature", "Description.", true );

    // Never forget to call this:
    WDataModule::properties();
}

// You will need to implement this function. It defines a list of filters of inputs your module supports. This method is called
// before instancing the module (called for your module prototype instance).
std::vector< WDataModuleInputFilter::ConstSPtr > WMTemplateDataLoader::getInputFilter() const
{
    // The list of filters:
    std::vector< WDataModuleInputFilter::ConstSPtr > filters;

    // Now, let us add some filters. OpenWalnut already provides a file-based filter for convenience.
    filters.push_back( WDataModuleInputFilter::ConstSPtr(
        new WDataModuleInputFilterFile(
            "owfun",                    // the extension of the file, without "."
            "OpenWalnut Demo File Type" // a description shown in OpenWalnut for this kind of files
    ) ) );

    // thats it. OpenWalnut will use this filter whenever the user tries to load some data. If it matches, your module
    // will be created and started.

    // But wait! There is more. How about writing your own input filters? This can be interesting if you only allow
    // a certain subset of files with a given extension. Maybe you are only able to load scalar fields from files that support
    // a wide range of internal formats? Then writing your own input filter, derived from WDataModuleInputFilterFile is the
    // best choice.

    // IMPORTANT: right now (OpenWalnut 1.4), there are some limitations that will be resolved soon:
    //
    // * there is NO support for other input filters. We are working hard to add this.
    // * two modules which match the same file type: OpenWalnut chooses the last loaded module -> yes, ugly.
    // * no support for properties in input filters
    //
    // Please stay tuned. It requires some work in the GUI to support generic input filter types. You can follow the progress on
    // feature #32 in the OpenWalnut bug tracker.

    return filters;
}

// So let us assume, the user has chosen a file/path/url/... to load. OpenWalnut creates and runs an instance of this data module
// and sets the new WDataModuleInput. This causes this method to be called. It is NOT called from within a certain thread, like the module thread.
// You should strictly avoid actually loading data in here. This might freeze other threads. Instead, use conditions, flags, properties or
// more to wake up your module thread and load the data there.
//
// IMPORTANT: please keep in mind that this can be called multiple times, whenever the user changes the WDataModuleInput! A typical use-case
// is when the user changes the file in an already existing data module instance OR the user triggers a reload button.  So take care that your
// data module is able to load multiple times.
void WMTemplateDataLoader::handleInputChange()
{
    // A very simple way of achieving this is to define a flag. We call it m_reload. This loads the stuff defined in the WDataModuleInput again. No
    // matter whether this is a new input or not.

    // Set flag.
    m_reload = true;

    // wakeup the thread
    m_moduleState.notify();
}

void WMTemplateDataLoader::moduleMain()
{
    // NOTE: an easy way to wake up the thread, even if it is not waiting: use resetable condition sets:
    m_moduleState.setResetable( true, true );

    // Now, we can mark the module ready.
    ready();

    // Wait until the project file loader finished.
    waitRestored();

    // Now the remaining module code. In our case, this is empty.
    while( !m_shutdownFlag() )
    {
        // Usually, this waits. But IF there was a call to handleInputChange already, this immediately wakes up again and does the first load.
        // This behaviour can be changed using the setResetable method of the condition set.
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }

        // Are we forced to load something?
        if( m_reload )
        {
            load();
        }
    }

    // Never miss to clean up.
    debugLog() << "Shutting down ...";
}

// Finally, this is our load routine. This is called from within the moduleMain method. Never call this from handleInputChange!
void WMTemplateDataLoader::load()
{
    // Reset the flag.
    m_reload = false;

    // Query the new input. We defined a file input filter, thus we get a WDataModuleInputFile. If you implement your own input filters that create
    // custom WDataModuleInput instances, you can query them in the same fashion.
    WDataModuleInputFile::SPtr inputFile = getInputAs< WDataModuleInputFile >();
    // Be aware, that the input might also be NULL or cannot be cast.
    if( !inputFile )
    {
        // No input? Reset output too. You should do this. The user might want to remove the input and make the output empty again.
        m_output->updateData( WDataSetScalar::SPtr() );
        return;
    }

    // Now implement your loading .......
    boost::filesystem::path p = inputFile->getFilename();
    infoLog() << "Start Loading from \"" << p.string() << "\"";

    // also add some progress to give feedback:
    boost::shared_ptr< WProgress > progress( new WProgress( "Loading" ) );
    m_progress->addSubProgress( progress );

    // LOAD LOAD LOAD LOAD

    progress->finish();
    m_progress->removeSubProgress( progress );
    infoLog() << "Done";
}

