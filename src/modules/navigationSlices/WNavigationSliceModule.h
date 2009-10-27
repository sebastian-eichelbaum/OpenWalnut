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

#include <list>
#include <string>
#include <vector>

#include <osg/Node>

#include "../../dataHandler/WDataSet.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.hpp"
#include "../../graphicsEngine/WShader.h"


/**
 * \par Description:
 *
 * Simple module for testing some WKernel functionality.
 */
class WNavigationSliceModule: public WModule, public osg::Referenced
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

    /**
     * updates the positions of the navigation slices
     */
    void updateSlices();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

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
     * Initialize the properties for this module.
     */
    virtual void properties();

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
     * initial creation function for the slice geometry
     */
    void createSlices();

    /**
     * creates and initializes the uniform parameters for the shader
     */
    void initUniforms( osg::StateSet* sliceState );

    /**
     * the root node for this module
     */
    osg::Geode* m_sliceNode;

    /**
     * the shader object for this module
     */
    boost::shared_ptr< WShader >m_shader;

    /**
     * Input connector required by this module.
     */
    boost::shared_ptr<WModuleInputData<std::list<boost::shared_ptr<WDataSet> > > > m_Input;

    std::vector< osg::Uniform* > m_typeUniforms;
    std::vector< osg::Uniform* > m_alphaUniforms;
    std::vector< osg::Uniform* > m_thresholdUniforms;
    std::vector< osg::Uniform* > m_samplerUniforms;
};


class sliceNodeCallback : public osg::NodeCallback
{
public:
    explicit sliceNodeCallback( boost::shared_ptr< WNavigationSliceModule > module )
    {
        m_module = module;
    }

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        if ( m_module )
        {
            m_module->updateSlices();
            m_module->updateTextures();
        }
        traverse( node, nv );
    }
private:
    boost::shared_ptr< WNavigationSliceModule > m_module;
};

#endif  // WNAVIGATIONSLICEMODULE_H

