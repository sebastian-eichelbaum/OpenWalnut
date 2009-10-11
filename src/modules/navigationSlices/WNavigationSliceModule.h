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

#ifndef WNAVIGATIONSLICEMODULE_H
#define WNAVIGATIONSLICEMODULE_H

#include <string>
#include <list>

#include <osg/Node>

#include "../../dataHandler/WDataSet.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.hpp"
#include "../../graphicsEngine/WShader.h"


// TODO(schurade): fix description
/**
 * \par Description:
 * 
 * Simple module for testing some WKernel functionality.
 */
class WNavigationSliceModule: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WNavigationSliceModule();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WNavigationSliceModule();

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

    /**
     * Connect the listener function of the module to the gui signals
     * this has to be called after full initialization fo the gui
     */
    void connectToGui();

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
     * TODO(schurade): add comment
     */
    void createSlices();

    /**
     *
     */
    void updateSlices();

    /**
     * TODO(schurade): add comment
     */
    void sliderAxialMoved( int value );
    void sliderCoronalMoved( int value );
    void sliderSagittalMoved( int value );

    void buttonAxialChanged( bool check );
    void buttonCoronalChanged( bool check );
    void buttonSagittalChanged( bool check );

    /**
     * TODO(schurade): add comment
     */
    osg::Geode* m_sliceNode;

    /**
     * TODO(schurade): add comment
     */
    boost::shared_ptr< WShader >m_shader;

    /**
     * TODO(schurade): add comment
     */
    bool m_textureAssigned;

    int m_axialSlice;
    int m_coronalSlice;
    int m_sagittalSlice;

    int m_maxAxial;
    int m_maxCoronal;
    int m_maxSagittal;

    bool m_showAxial;
    bool m_showCoronal;
    bool m_showSagittal;

    /** 
     * Input connector required by this module.
     */
    boost::shared_ptr<WModuleInputData<std::list<boost::shared_ptr<WDataSet> > > > m_Input;
};

#endif  // WNAVIGATIONSLICEMODULE_H

