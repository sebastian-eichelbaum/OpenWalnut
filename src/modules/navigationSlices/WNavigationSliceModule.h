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

#include <osg/Node>

#include "../../kernel/WModule.h"
#include "../../graphicsEngine/WShader.h"


/**
 * \par Description:
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
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WNavigationSliceModule( const WNavigationSliceModule& other );

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

private:
    /**
     *
     */
    void createSlices();

    /**
     *
     */
    void updateSlices();

    /**
     *
     */
    void sliderAxialMoved( int value );
    void sliderCoronalMoved( int value );
    void sliderSagittalMoved( int value );

    void buttonAxialChanged( bool check );
    void buttonCoronalChanged( bool check );
    void buttonSagittalChanged( bool check );

    /**
     *
     */
    osg::Geode* m_sliceNode;

    /**
     *
     */
    boost::shared_ptr< WShader >m_shader;

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
};

#endif  // WNAVIGATIONSLICEMODULE_H

