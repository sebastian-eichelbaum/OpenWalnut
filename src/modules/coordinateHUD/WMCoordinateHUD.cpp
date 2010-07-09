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




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to create your own module in OpenWalnut? Here are the steps to take:
//   * copy the template module directory
//   * think about a name for your module
//   * rename the files from WMTemplate.cpp and WMTemplate.h to WMYourModuleName.cpp and WMYourModuleName.h
//   * rename the class inside these files to WMYourModuleName
//   * change WMYourModuleName::getName() to a unique name, like "Your Module Name"
//   * add a new prototype of your module to src/kernel/WModuleFactory.cpp -> search for m_prototypes.insert
//     * analogously to the other modules, add yours
//     * Note: this step will be automated in some time
//   * run CMake and compile
//   * read the documentation in this module and modify it to your needs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include <osg/Vec3>
#include <osg/Geometry>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../graphicsEngine/WGEUtils.h"

#include "coordinateHUD.xpm"
#include "WMCoordinateHUD.h"

WMCoordinateHUD::WMCoordinateHUD():
    WModule()
{

}

WMCoordinateHUD::~WMCoordinateHUD()
{

}

boost::shared_ptr< WModule > WMCoordinateHUD::factory() const
{
    return boost::shared_ptr< WModule >( new WMCoordinateHUD() );
}

const char** WMCoordinateHUD::getXPMIcon() const
{
    return coordinateHUD_xpm;
}

const std::string WMCoordinateHUD::getName() const
{
    return "CoordinateHUD";
}

const std::string WMCoordinateHUD::getDescription() const
{
    return "This module is to display a coordinatesystem as HUD.";
}

void WMCoordinateHUD::connectors()
{

//    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
//        new WModuleInputData< WDataSetSingle >( shared_from_this(),
//                                                               "in", "The dataset to display" )
//        );
//    addConnector( m_input );


//    m_output = boost::shared_ptr< WModuleOutputData < WDataSetSingle  > >(
//       new WModuleOutputData< WDataSetSingle >( shared_from_this(),
//                                                               "out", "The calculated dataset" )
//      );
//    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMCoordinateHUD::properties()
{
//    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
}

void WMCoordinateHUD::moduleMain()
{

    debugLog() << "Entering moduleMain()";
    m_moduleState.setResetable( true, true );
//    m_moduleState.add( m_input->getDataChangedCondition() );
//    m_moduleState.add( m_propCondition );
  
    ready();
    debugLog() << "Module is now ready.";


    //m_rootNode = new WGEManagedGroupNode( m_active );
    m_rootNode = osg::ref_ptr< osg::Projection >( new osg::Projection );
    m_rootNode->setName("coordHUDNode");

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    debugLog() << "Entering main loop";


        osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
        osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;
	
        //Eckpunkte
        float size = 100;
        osg::Vec3Array* vertices = new osg::Vec3Array;
/*
 	    vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(size, 0, 0));
	    vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(0, size, 0));
	    vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(0, 0, size));	
        vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(-size, 0, 0));
	    vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(0, -size, 0));
	    vertices->push_back(osg::Vec3(0, 0, 0));
	    vertices->push_back(osg::Vec3(0, 0, -size));
*/

 	    vertices->push_back(osg::Vec3(100, 100,100));
	    vertices->push_back(osg::Vec3(100+size, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100+size, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100+size));	
        vertices->push_back(osg::Vec3(100, 100, 100));
	    vertices->push_back(osg::Vec3(100-size, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100-size, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100));
	    vertices->push_back(osg::Vec3(100, 100, 100-size));
        //colors
	    osg::Vec4 x_color(1.0f,0.0f,0.0f,1.0f);     //red
	    osg::Vec4 y_color(0.0f,1.0f,0.0f,1.0f);     //green
	    osg::Vec4 z_color(0.0f,0.0f,1.0f,1.0f);     //blue
	    osg::Vec4 neg_color(1.0f,1.0f,1.0f,1.0f);   //white

	    osg::Vec4Array* color = new osg::Vec4Array(12);
	    (*color)[0] = x_color;
	    (*color)[1] = x_color;
	    (*color)[2] = y_color;
	    (*color)[3] = y_color;
	    (*color)[4] = z_color;
	    (*color)[5] = z_color;
	    (*color)[6] = x_color;
	    (*color)[7] = neg_color;
	    (*color)[8] = y_color;
	    (*color)[9] = neg_color;
	    (*color)[10] = z_color;
	    (*color)[11] = neg_color;
 
        coordGeom->setColorArray(color);
        coordGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
 
        osg::DrawArrays *da = new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertices->size());

        coordGeom->setVertexArray( vertices );
        coordGeom->addPrimitiveSet( da);
        coordGeode->addDrawable( coordGeom);
        coordGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        //HUD
        int sizeX = 1024;
        int sizeY = 768;
        int sizeZ = 768;
        m_rootNode->setMatrix(osg::Matrix::ortho(0,sizeX,0,sizeY,0,sizeZ));
        // For the HUD model view matrix use an identity matrix:
        osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
        HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

        // Make sure the model view matrix is not affected by any transforms
        // above it in the scene graph:
        HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

        // Add the HUD projection matrix as a child of the root node
        // and the HUD model view matrix as a child of the projection matrix
        // Anything under this node will be viewed using this projection matrix
        // and positioned with this model view matrix.
        m_rootNode->addChild( HUDModelViewMatrix );

        

        // Add the Geometry node to contain HUD geometry as a child of the
        // HUD model view matrix.
        m_HUDs = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );
        // A geometry node for our HUD
        osg::ref_ptr<osg::Geode> HUDGeode = osg::ref_ptr<osg::Geode>( new osg::Geode() );

        HUDModelViewMatrix->addChild( m_HUDs );
        m_HUDs->insert( HUDGeode );
        m_HUDs->insert( coordGeode );

        // Set up geometry for the HUD and add it to the HUD
        osg::ref_ptr< osg::Geometry > HUDBackgroundGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Vec3Array > HUDBackgroundVertices = new osg::Vec3Array;
        HUDBackgroundVertices->push_back( osg::Vec3( 0, 0, -1 ) );
        HUDBackgroundVertices->push_back( osg::Vec3( 200, 0, -1 ) );
        HUDBackgroundVertices->push_back( osg::Vec3( 200, 200, -1 ) );
        HUDBackgroundVertices->push_back( osg::Vec3( 0, 200, -1 ) );
        osg::ref_ptr< osg::DrawElementsUInt > HUDBackgroundIndices = new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
        HUDBackgroundIndices->push_back( 0 );
        HUDBackgroundIndices->push_back( 1 );
        HUDBackgroundIndices->push_back( 2 );
        HUDBackgroundIndices->push_back( 3 );

        osg::ref_ptr< osg::Vec4Array > HUDcolors = new osg::Vec4Array;
        HUDcolors->push_back( osg::Vec4( 0.8f, 0.8f, 0.8f, 0.8f ) );

        osg::ref_ptr< osg::Vec3Array > HUDnormals = new osg::Vec3Array;
        HUDnormals->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
        HUDBackgroundGeometry->setNormalArray( HUDnormals );
        HUDBackgroundGeometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
        HUDBackgroundGeometry->addPrimitiveSet( HUDBackgroundIndices );
        HUDBackgroundGeometry->setVertexArray( HUDBackgroundVertices );
        HUDBackgroundGeometry->setColorArray( HUDcolors );
        HUDBackgroundGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        HUDGeode->addDrawable( HUDBackgroundGeometry );
        HUDGeode->addDrawable( coordGeom);

        // Create and set up a state set using the texture from above:
        osg::StateSet* HUDStateSet = new osg::StateSet();
        HUDGeode->setStateSet(HUDStateSet);

        // For this state set, turn blending on (so alpha texture looks right)
        HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

        // Disable depth testing so geometry is draw regardless of depth values
        // of geometry already draw.
        HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
        HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

        // Need to make sure this geometry is draw last. RenderBins are handled
        // in numerical order so set bin number to 11
        HUDStateSet->setRenderBinDetails( 11, "RenderBin");

       /////////////////


        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

 

        //add to root
/*        m_rootNode->remove( m_geode );
        m_geode = newGeode;
        m_geode->addUpdateCallback( new SafeUpdateCallback( this ) );
        m_rootNode->insert( m_geode );    
*/           
    if ( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    // connect updateGFX with picking
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );

   // WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMCoordinateHUD::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    traverse(node, nv);
}

void WMCoordinateHUD::TranslateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    traverse(node, nv);
}

bool WMCoordinateHUD::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */,
                                       WPVBaseTypes::PV_STRING value )
{
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint > WMCoordinateHUD::StringLength::clone()
{
    // If you write your own constraints, you NEED to provide a clone function. It creates a new copied instance of the constraints with the
    // correct runtime type.
    return boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint >( new WMCoordinateHUD::StringLength( *this ) );
}

void WMCoordinateHUD::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this if you do not use the
    // WGEManagedGroupNode for your scene. The user can (de-)activate modules in his GUI and you can handle this case here:
    if ( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    // The simpler way is by using WGEManagedGroupNode which deactivates itself according to m_active. See moduleMain for details.

    // Always call WModule's activate!
    WModule::activate();
}

