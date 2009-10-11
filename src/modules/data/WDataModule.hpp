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

#ifndef WDATAMODULE_H
#define WDATAMODULE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleOutputData.hpp"

#include "../../dataHandler/WDataSet.h"

/**
 * Module for encapsulating WDataSets. It can encapsulate almost everything, but is intended to be used with WDataSets and its
 * inherited classes. This class builds a "source".
 */
template < typename T >
class WDataModule: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WDataModule();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WDataModule();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

protected:

    /**
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

    /** 
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /** 
     * Receive DATA_CHANGE notifications.
     * 
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

private:

    /**
     * The only output of this data module.
     */
    boost::shared_ptr<WModuleOutputData<T> > m_output;
};


template < typename T >
WDataModule<T>::WDataModule():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
}

template < typename T >
WDataModule<T>::~WDataModule()
{
    // cleanup
}

template < typename T >
const std::string WDataModule<T>::getName() const
{
    return "Data Module";
}

template < typename T >
const std::string WDataModule<T>::getDescription() const
{
    return "This module can encapsulate data.";
}

template < typename T >
void WDataModule<T>::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr<WModuleOutputData<T> >(
            new WModuleOutputData<T>( shared_from_this(),
                "out1", "A loaded dataset." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

template < typename T >
void WDataModule<T>::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );
}

template < typename T >
void WDataModule<T>::threadMain()
{
    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

#endif  // WDATAMODULE_H

