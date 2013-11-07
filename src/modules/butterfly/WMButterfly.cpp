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

// Additional copyright information:
// The class WButterflyCalculator partially relies on the Butterfly Subdivision
// algorithm.that dates from Denis Zorin, Peter Schroeder, Wim Sweldens, Nira
// Dyn, David Levin and John A. Gregory. The original algorithm is depicted in:
// http://wwwmath.tau.ac.il/~niradyn/papers/butterfly.pdf
// http://mrl.nyu.edu/~dzorin/papers/zorin1996ism.pdf
// This work was required especially in the methods calcNewVertex and
// getInterpolatedValue.

#include <string>
#include <cmath>

#include <osg/Geometry>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WMButterfly.xpm"
#include "WMButterfly.h"
#include "structure/WVertexFactory.h"


// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMButterfly )

WMButterfly::WMButterfly() :
    WModule(), m_propCondition( new WCondition() )
{
    m_butterfly = new WButterflyFactory();
    m_butterfly->assignProgressCombiner( m_progress );
}

WMButterfly::~WMButterfly()
{
}

boost::shared_ptr<WModule> WMButterfly::factory() const
{
    return boost::shared_ptr<WModule>(new WMButterfly());
}

const char** WMButterfly::getXPMIcon() const
{
    return WMButterfly_xpm;
}

const std::string WMButterfly::getName() const
{
    return "Butterfly subdivision";
}

const std::string WMButterfly::getDescription() const
{
    return "Subdivides triangles in a mesh using the improved butterfly scheme (Zorin et al., 1996).";
}

/**Plugin connectors
 * @author schwarzkopf */
void WMButterfly::connectors()
{
    m_input = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(),
                                                               "input mesh", "The mesh to display" );
    m_output = boost::shared_ptr<WModuleOutputData<WTriangleMesh> >(
        new WModuleOutputData<WTriangleMesh>( shared_from_this(),
                                              "output mesh",
                                              "The loaded mesh." ) );
    addConnector( m_output );
    WModule::connectors();
}

/**Properties panel
 * @author schwarzkopf */
void WMButterfly::properties()
{
    // ---> Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    m_butterflySettingW = m_properties->addProperty( "Butterfly setting w: ",
                                                   "It's the general Butterfly Subdivision setting w that affects "
                                                   "the subdivision. It's usually chosen substantially small. "
                                                   "The original authors used 0.0.",
                                                   0.0,
                                                   m_propCondition );
    m_butterflySettingW->setMin( -0.125 );
    m_butterflySettingW->setMax( 0.125 );
    m_iterations = m_properties->addProperty( "Iterations count: ",
                                                   "Iteration count that is attempted regarding the maximal triangle count.",
                                                   1,
                                                   m_propCondition );
    m_iterations->setMin( 0 );
    m_iterations->setMax( 10 );
    m_maxTriangles10n = m_properties->addProperty( "Max. triangles = 10^: ",
                                                   "Maximal triangle count as result, scaled by 10^n. "
                                                   "The Subdivision stops when the triangle count is above it.",
                                                   6.8,
                                                   m_propCondition );
    m_maxTriangles10n->setMin( 2 );
    m_maxTriangles10n->setMax( 8 );
    m_maxTriangles = m_properties->addProperty( "Max triangles: ",
                                                   "Maximal triangle count as result. "
                                                   "The Subdivision stops when the triangle count is above it.",
                                                   ( int ) pow( 10, m_maxTriangles10n->get() ) );
    m_maxTriangles->setPurpose( PV_PURPOSE_INFORMATION );
    m_cpuThreads = m_properties->addProperty(      "CPU threads: ",
                                                   "Number of CPU cores/threads to use. 1,4x with 2, "
                                                   "1,7x with 4 threads and 2,2x speed with 8 threads "
                                                   "(total cycle performance).",
                                                   8,
                                                   m_propCondition );
    m_cpuThreads->setMin( 1 );
    m_cpuThreads->setMax( 12 );
    m_minTransformationAngle = m_properties->addProperty( "Min. transform angle: ",
                                                   "There are no subdivision in lines where the angle transformation "
                                                   "at the stencil center point would not exceed this value.",
                                                   0.0,
                                                   m_propCondition );
    m_minTransformationAngle->setMin( 0.0 );
    m_minTransformationAngle->setMax( 90.0 );
    m_maxTransformationAngle = m_properties->addProperty( "Max. transform angle: ",
                                                   "There are no subdivision in lines where the angle transformation "
                                                   "at the stencil center point would exceed this value.",
                                                   180.0,
                                                   m_propCondition );
    m_maxTransformationAngle->setMin( 0.0 );
    m_maxTransformationAngle->setMax( 180.0 );
    m_minQuotient = m_properties->addProperty( "Min. lengths quotient: ",
                                                   "Minimal allowed quotient of the new lines that lie between "
                                                   "the stencil center points and the new point.",
                                                   0.0,
                                                   m_propCondition );
    m_minQuotient->setMin( 0.0 );
    m_minQuotient->setMax( 1.0 );
    m_minAmountOfMax = m_properties->addProperty( "Min. amount of max.: ",
                                                   "Line length divided by that value must be at least as big "
                                                   "as the longest line in the stencil in order to subdivide.",
                                                   0.0,
                                                   m_propCondition );
    m_minAmountOfMax->setMin( 0.0 );
    m_minAmountOfMax->setMax( 1.0 );
    m_minAmountOfAverage = m_properties->addProperty( "Min. am. of average.: ",
                                                   "Line length divided by that value must be at least as big "
                                                   "as the average line in the stencil in order to subdivide.",
                                                   0.0,
                                                   m_propCondition );
    m_minAmountOfAverage->setMin( 0.0 );
    m_minAmountOfAverage->setMax( 1.0 );
    m_minSubdividedLineLength = m_properties->addProperty( "Min. div. line length: ",
                                                   "Lines below that length are not subdivided.",
                                                   0.0,
                                                   m_propCondition );
    m_minSubdividedLineLength->setMin( 0.0 );
    m_minSubdividedLineLength->setMax( 1.0 );
    m_minSubdividedLineLengthMultiplierPerIteration = m_properties->addProperty( "    Its iteration factor: ",
                                                   "Amount which multiplies the value above after each iteration. "
                                                   "As consequence subdivision is don by the order of connection "
                                                   "length.",
                                                   1.0,
                                                   m_propCondition );
    m_minSubdividedLineLengthMultiplierPerIteration->setMin( 0.5 );
    m_minSubdividedLineLengthMultiplierPerIteration->setMax( 1.0 );
    m_minNeighbourTriangleAngle = m_properties->addProperty( "Minimal triangle angle: ",
                                                   "Connections that have angles in shared triangles below that "
                                                   "angle are interpolated by the mixture of linearly and proposed "
                                                   "subdivision, weighted by the comparison between the smallest "
                                                   "angle and this limit.",
                                                   0.0,
                                                   m_propCondition );
    m_minNeighbourTriangleAngle->setMin( 0.0 );
    m_minNeighbourTriangleAngle->setMax( 90.0 );
    m_maxNeighbourTriangleAngle = m_properties->addProperty( "Maximal triangle angle: ",
                                                   "A subdivision is marked as invalid if the angle between any "
                                                   "neighbor triangle line is above that angle.",
                                                   180.0,
                                                   m_propCondition );
    m_maxNeighbourTriangleAngle->setMin( 0.0 );
    m_maxNeighbourTriangleAngle->setMax( 180.0 );
    m_maxNeighbourTriangleAngleLengthRatio = m_properties->addProperty( "    Its length ratio: ",
                                                   "If the line length is smaller than the subdividable line "
                                                   "by this ratio, then it won't be marked as invalid.",
                                                   0.3,
                                                   m_propCondition );
    m_maxNeighbourTriangleAngleLengthRatio->setMin( 0.0 );
    m_maxNeighbourTriangleAngleLengthRatio->setMax( 1.0 );
    m_maxAmountOfMaxForVertexJoin = m_properties->addProperty( "min Edge amount for join: ",
                                                   "Examining all triangles. If any line is smaller than the "
                                                   "longest multiplied by this value so the points of the small "
                                                   "line will be joined.",
                                                   0.0,
                                                   m_propCondition );
    m_maxAmountOfMaxForVertexJoin->setMin( 0.0 );
    m_maxAmountOfMaxForVertexJoin->setMax( 0.6 );
    m_maxNeighbourTriangleAngleTriangleFlip = m_properties->addProperty( "Flip below angle: ",
                                                   "If any triangle that connects two vertices has two angles at "
                                                   "these points that both are below this degree value so the "
                                                   "triangle orientation is rotated in this square constellation",
                                                   0.0,
                                                   m_propCondition );
    m_maxNeighbourTriangleAngleTriangleFlip->setMin( 0.0 );
    m_maxNeighbourTriangleAngleTriangleFlip->setMax( 70.0 );
    WModule::properties();
}

void WMButterfly::requirements()
{
}

/**Main method which directs the input WTriangleMesh to the Butterfly Subdivision filter
 * and puts out the result
 * @author schwarzkopf */
void WMButterfly::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // graphics setup
    m_rootNode = osg::ref_ptr<WGEManagedGroupNode>( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // main loop
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        int maxTriangles = ( int ) pow( 10, m_maxTriangles10n->get() );
        m_maxTriangles->set( maxTriangles );

        m_butterfly->setButterflySettingW( static_cast<float>( m_butterflySettingW->get() ) );
        m_butterfly->setIterationsSettings( ( size_t )( m_iterations->get() ),
                static_cast<float>( m_maxTriangles10n->get() ) );
        m_butterfly->setCpuThreadCount( ( size_t ) m_cpuThreads->get() );

        WSubdivisionValidator* validator = m_butterfly->getValidator();
        validator->setMinTransformationAngle( static_cast<float>( m_minTransformationAngle->get() ) );
        validator->setMaxTransformationAngle( static_cast<float>( m_maxTransformationAngle->get() ) );
        validator->setMinLenghtsQuotient( static_cast<float>( m_minQuotient->get() ) );
        validator->setMinAmountOfMax( static_cast<float>( m_minAmountOfMax->get() ) );
        validator->setMinAmountOfAverage( static_cast<float>( m_minAmountOfAverage->get() ) );
        validator->setMinSubdividedLineLength( static_cast<float>( m_minSubdividedLineLength->get() ) );
        validator->setMinSubdividedLineLengthMultiplierPerIteration( static_cast<float>( m_minSubdividedLineLengthMultiplierPerIteration->get() ) );
        validator->setMinMeighbourTriangleAngle( static_cast<float>( m_minNeighbourTriangleAngle->get() ) );
        validator->setMaxNeighbourTriangleAngle( static_cast<float>( m_maxNeighbourTriangleAngle->get() ),
                static_cast<float>( m_maxNeighbourTriangleAngleLengthRatio->get() ) );
        validator->setMaxAmountOfMaxForVertexJoin( static_cast<float>( m_maxAmountOfMaxForVertexJoin->get() ) );
        validator->setMaxNeighbourTriangleAngleTriangleFlip( static_cast<float>( m_maxNeighbourTriangleAngleTriangleFlip->get() ) );

        m_mesh = m_input->getData();
        if  ( m_mesh )
        {
            m_minSubdividedLineLength->setMax( getMaxLineLength( m_mesh ) );

            boost::shared_ptr< WTriangleMesh > interpolatedMesh = m_butterfly->getSubdividedMesh( m_mesh );
            m_output->updateData( interpolatedMesh );

            if  ( m_oldInterpolatedMesh && m_oldInterpolatedMesh != m_mesh )
            {
                usleep( 1000000 ); //Some plugins have problems if old is purged too fast

                m_oldInterpolatedMesh->~WTriangleMesh();
                m_oldInterpolatedMesh->~WTransferable();
                m_oldInterpolatedMesh->~WPrototyped();
            }
            m_oldInterpolatedMesh = interpolatedMesh;
        }

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
            break;

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WTriangleMesh > meshData = m_input->getData();

        if( !meshData )
            continue;

        // ---> Insert code doing the real stuff here
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

float WMButterfly::getMaxLineLength( boost::shared_ptr< WTriangleMesh > inputMesh )
{
    float maxLineLength = 0.0f;
    for  ( size_t triID = 0; triID < inputMesh->triangleSize(); triID++ )
    {
        size_t id0 = inputMesh->getTriVertId0( triID );
        size_t id1 = inputMesh->getTriVertId1( triID );
        size_t id2 = inputMesh->getTriVertId2( triID );
        osg::Vec3 point0 = inputMesh->getVertex( id0 );
        osg::Vec3 point1 = inputMesh->getVertex( id1 );
        osg::Vec3 point2 = inputMesh->getVertex( id2 );
        float length0 = WVertexFactory::getDistance( point0, point1 );
        float length1 = WVertexFactory::getDistance( point0, point2 );
        float length2 = WVertexFactory::getDistance( point2, point1 );
        if  ( length0 > maxLineLength )
            maxLineLength = length0;

        if  ( length1 > maxLineLength )
            maxLineLength = length1;

        if  ( length2 > maxLineLength )
            maxLineLength = length2;
    }
    return maxLineLength>0.0f ?maxLineLength :1.0f;
}
