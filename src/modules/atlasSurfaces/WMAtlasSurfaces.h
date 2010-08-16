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

#ifndef WMATLASSURFACES_H
#define WMATLASSURFACES_H

#include <string>
#include <vector>
#include <utility>
#include <map>

#include <osg/Geode>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WTriangleMesh2.h"

class WDataSetScalar;



/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "schurade".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMAtlasSurfaces: public WModule
{
public:

    /**
     *
     */
    WMAtlasSurfaces();

    /**
     *
     */
    virtual ~WMAtlasSurfaces();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
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
     * function that updates the currently shown gfx according to the current selection
     */
    void updateGraphics();

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

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

    /**
     * Creates the osg geometry nodes from the previously created triangle meshes
     */
    void createOSGNode();

private:
    /**
     * Creates a triangle mesh for each region
     */
    void createSurfaces();

    /**
     * Callback to listen for property changes
     */
    void propertyChanged();

    /**
     * Helper function to read in a text file
     *
     * \param fileName
     * \return the text file as a vector of strings for each line
     */
    std::vector< std::string > readFile( const std::string fileName );

    /**
     * Helper function to read, parse and store the labels
     *
     * \param fileName
     */
    void loadLabels( std::string fileName );

    /**
     * This function takes a dataset and marksvoxel with the specified number, all other voxel will be unmarked
     *
     * \param inGrid
     * \param vals
     * \param number
     */
    template< typename T > std::vector< float > cutArea( boost::shared_ptr< WGrid > inGrid,
            boost::shared_ptr< WValueSet< T > > vals, unsigned int number );

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.

    boost::shared_ptr< const WDataSetScalar > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    std::vector< size_t >m_regionMeshIds; //!< stores the real id of a mesh, needed if the dataset contains unused numbers

    std::vector< boost::shared_ptr< WTriangleMesh2 > >m_regionMeshes; //!< stores pointers to all triangle meshes

    std::map< size_t, std::pair< std::string, std::string > >m_labels; //!< the labels with their id

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node. We need it to be able to update it when callback is invoked.

    osg::ref_ptr< osg::Geode > m_outputGeode; //!< Pointer to geode containing the glpyhs

    bool m_dirty; //!< flag true if something happenend that requires redrawing of gfx

    bool m_labelsLoaded; //!< true when a label file is loaded

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A property allowing the user to select multiple elements of a list.
     */
    WPropSelection m_aMultiSelection;

    /**
     * A list of items that can be selected using m_aSingleSelection or m_aMultiSelection.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelections;
};

/**
 * Adapter object for realizing callbacks of the node representing the isosurface in the osg
 */
class AtlasSurfaceNodeCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor of the callback adapter.
     * \param module A function of this module will be called
     */
    explicit AtlasSurfaceNodeCallback( WMAtlasSurfaces* module );

    /**
     * Function that is called by the osg and that call the function in the module.
     * \param node The node we are called.
     * \param nv the visitor calling us.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

private:
    WMAtlasSurfaces* m_module; //!< Pointer to the module to which the function that is called belongs to.
};

inline AtlasSurfaceNodeCallback::AtlasSurfaceNodeCallback( WMAtlasSurfaces* module )
    : m_module( module )
{
}

inline void AtlasSurfaceNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module )
    {
        m_module->updateGraphics();
    }
    traverse( node, nv );
}


#endif  // WMATLASSURFACES_H
