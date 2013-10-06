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

#ifndef WMBUTTERFLY_H
#define WMBUTTERFLY_H

#include <string>
#include "core/kernel/WModule.h"
#include "structure/WVertexFactory.h"
#include "WButterflyFactory.h"


using osg::Vec3;
// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetScalar;
class WGEManagedGroupNode;

/**
 * OpenWalnut module that subdivides a triangle mesh in more points resulting a smoother surface.
 * Plugin Version: 0.9.0, Release Candidate 1
 * Date: 2013/08/23 23:32
 * \ingroup modules
 */
class WMButterfly: public WModule
{
public:
    /**
     * Creates the module for subdividing a triangle mesh using Butterfly subdivision.
     */
WMButterfly();

    /**
     * Destroys this module.
     */
    virtual ~WMButterfly();

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
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
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
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * Returns the biggest length of a neighbour relationship.
     * \param inputMesh Triangle mesh to analyze.
     * \return The biggest distance of all vertex neighbour relationships.
     */
    float getMaxLineLength( boost::shared_ptr< WTriangleMesh > inputMesh );

    /**
     * Input connector for scalar data.
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_input;
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_output;  //!< Output connector provided by this module.

    /**
     * Holder for the current triangle mesh that grants that there's always a valid data output at
     * the end. The old data is deleted after the new one is applied. Deleting a triangle mesh that
     * is attached at the output connector makes some plugins crash.
     */
    boost::shared_ptr< WTriangleMesh > m_oldInterpolatedMesh;

    /**
     * The OSG root node for this module. All other geodes or OSG nodes will be attached on this single node.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * Needed for recreating the geometry, incase when resolution changes.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The general Butterfly Subdivision setting w that affects the subdivision. See the algorithm
     * documentation for the exact meaning. It's usually chosen substantially small. The original
     * authors used 0.0.
     */
    WPropDouble m_butterflySettingW;

    /**
     * The iteration count of the Butterfly subdivision to attempt.
     */
    WPropInt m_iterations;

    /**
     * Maximal allowed triangle count after Butterfly subdivision. Subdivision stops at last subdivision
     * step exceeding that triangle count. Due to usability the scale is exponential by 10^n.
     */
    WPropDouble m_maxTriangles10n;

    /**
     * Maximal allowed triangle count after Butterfly subdivision. Subdivision stops at last subdivision
     * step exceeding that triangle count.
     */
    WPropInt m_maxTriangles;

    /**
     * CPU thread count setting for multithreading support.
     */
    WPropInt m_cpuThreads;

    /**
     * Angle setting which both angles at the subdivided line ends should be at
     * least as big as this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex.
     */
    WPropDouble m_minTransformationAngle;

    /**
     * Angle setting which both angles at the subdivided line ends should be not
     * bigger than this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex.
     */
    WPropDouble m_maxTransformationAngle;

    /**
     * Maximum factor which the following lines should differ to show a valid
     * subdivision. Both lines are measured from the new subdivided point and end at the
     * ends of the subdivided line end.
     */
    WPropDouble m_minQuotient;

    /**
     * Factor for multiplying the maximal distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than this distance multiplied by it.
     */
    WPropDouble m_minAmountOfMax;

    /**
     * Factor for multiplying the average distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than the average distance multiplied by it.
     */
    WPropDouble m_minAmountOfAverage;

    /**
     * The minimal subdividable line length. Lines of smaller distance than this are not
     * subdivided.
     */
    WPropDouble m_minSubdividedLineLength;

    /**
     * The factor where the minimal subdivided line length is multiplied by in each
     * Subdivision iteration step.
     */
    WPropDouble m_minSubdividedLineLengthMultiplierPerIteration;

    /**
     * The Minimal allowed angle between two Subdivided line ends. If the two points have
     * a triangle where the angle is at least in one triangle cutting that vertices amaller than
     * this so the coordinates will be interpolated between the mean of the two neighbors and
     * its Butterfly subdivision used one, weighted using the minimal transformation angle.
     */
    WPropDouble m_minNeighbourTriangleAngle;

    /**
     * A subdivision is marked as invalid if the angle between any neighbor triangle line
     * is above that angle.
     */
    WPropDouble m_maxNeighbourTriangleAngle;

    /**
     * This settings extends
     * If the line length is smaller than the subdividable line by this ratio, then it won't
     * be marked as invalid.
     */
    WPropDouble m_maxNeighbourTriangleAngleLengthRatio;

    /**
     * The Factor for comparison to the maximal length of a triangle. Lines smaller than this
     * force a joint of the corresponding two vertices. The triangle falls finally away.
     */
    WPropDouble m_maxAmountOfMaxForVertexJoin;

    /**
     * The Minimal allowed angle which fits in both vertices at a subdividable line end.
     * An existing triangle where both angles fit in forces it to rotate it with the other triangle
     * connected with that vertices araund the four vertices of that two triangles. There is no
     * application if the final state would be also invalid.
     */
    WPropDouble m_maxNeighbourTriangleAngleTriangleFlip;

    /**
     * Triangle mesh which will be assigned to the Butterfly subdivision instance.
     */
    boost::shared_ptr< WTriangleMesh > m_mesh;

    /**
     * Butterfly subdivision instance.
     */
    butterfly::WButterflyFactory* m_butterfly;
};

#endif  // WMBUTTERFLY_H
