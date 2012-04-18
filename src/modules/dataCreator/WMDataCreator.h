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

#ifndef WMDATACREATOR_H
#define WMDATACREATOR_H

#include <string>

#include "core/common/WStrategyHelper.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSet.h"

/**
 * Module which utilizes the strategy pattern to provide a multitude of dataset creation algorithms
 *
 * \ingroup modules
 */
class WMDataCreator : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMDataCreator();

    /**
     * Destructor.
     */
    ~WMDataCreator();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

    /**
     * This class is the base for all data creators. Derive your creator from this class and implement the pure virtual methods.
     */
    class DataCreatorBase
    {
    public:
        /**
         * Shared ptr to an instance. Needed by WStrategyHelper.
         */
        typedef boost::shared_ptr< DataCreatorBase > SPtr;

        /**
         * Shared ptr to a const instance.
         */
        typedef boost::shared_ptr< const DataCreatorBase > ConstSPtr;

        /**
         * Construct a creator. Name and description are mandatory and must be unique among all DataCreator.
         *
         * \param name the name
         * \param description the description
         * \param icon an icon in XPM format. Can be NULL.
         */
        DataCreatorBase( std::string name, std::string description, const char** icon = NULL );

        /**
         * Destructor. Implement if you have non trivial cleanup stuff.
         */
        virtual ~DataCreatorBase();

        /**
         * The name of the creator.
         *
         * \return the name
         */
        virtual std::string getName() const;

        /**
         * The description of this creator.
         *
         * \return description text.
         */
        virtual std::string getDescription() const;

        /**
         * The icon of this creator.
         *
         * \return the icon in XPM format. Can be NULL.
         */
        virtual const char** getIcon() const;

        /**
         * Return the property group of this creator.
         *
         * \note the method is non-const to allow returning the properties as non-const
         *
         * \return  the properties.
         */
        virtual WProperties::SPtr getProperties();

    private:
        std::string m_name; //!< the name
        std::string m_description; //!< the description
        const char** m_icon; //!< the icon
        WProperties::SPtr m_properties; //!< the properties of the creator.
    };

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

private:
    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    boost::shared_ptr< WModuleOutputData< WDataSet > > m_output; //!< The only output of this module.


    WPropInt m_nbVoxelsX; //!< number of voxels in x direction
    WPropInt m_nbVoxelsY; //!< number of voxels in y direction
    WPropInt m_nbVoxelsZ; //!< number of voxels in z direction

    WPropPosition m_origin; //!< where to put the origin
    WPropPosition m_size; //!< where to put the origin

    WPropSelection m_valueType; //!< the datatype of the valueset
    WPropSelection m_structuralType; //!< select between scalar, vector and other structural types

    WStrategyHelper< DataCreatorBase > m_strategy; //!< the strategy currently active.
};

#endif  // WMDATACREATOR_H
