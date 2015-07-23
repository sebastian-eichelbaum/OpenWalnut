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

//---------------------------------------------------------------------------
//
// This file is also part of the
// Whole-Brain Connectivity-Based Hierarchical Parcellation Project
// David Moreno-Dominguez
// Ralph Schurade
// moreno@cbs.mpg.de
// www.cbs.mpg.de/~moreno
//
//---------------------------------------------------------------------------

#ifndef WMPARTITION2MESH_H
#define WMPARTITION2MESH_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WValueSet.h"

#include "core/graphicsEngine/WTriangleMesh.h"


/**
 * This module projects a clustering partition result onto a (freesurfer) mesh
 * Meant to work with hierchClustDisplay. It accepts 4 different inputs:
 * - A vector with the cluster color of each data point
 * - A vector with the voxel positions from the clustering data points
 * - The reference mesh to project the results to (normaly an original surface mesh)
 * - The Visualization Mesh. Can be equal to the reference mesh or can be a 1-1 matched mesh (inflated, pial. etc...)
 *
 * \ingroup modules
 */
class WMPartition2Mesh: public WModule
{
public:
    /**
     *
     */
    WMPartition2Mesh();

    /**
     *
     */
    virtual ~WMPartition2Mesh();

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
    void writeLabels(); //!< writes a freesurfer set of labels and tcl loading file

#if 0
    void writeAnnotation(); //!< writes a freesurfer annotation file (NOT WORKING)
#endif

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_coordInput;     //!< An input connector that accepts order 1 datasets.
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput1;     //!< An input connector for the reference mesh
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput2;     //!< An input connector for the mesh
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_colorInput;     //!< This is a pointer to the color input dataset.

    boost::shared_ptr< WDataSetScalar > m_coordinateVector;     //!< This is a pointer to the coord vector.
    boost::shared_ptr< WDataSetVector > m_colorVector;    //!< This is a pointer to the color vector.
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_meshOutput;  //!< Output connector provided by this module.

    boost::shared_ptr< WTriangleMesh > m_referenceMesh; //!< A pointer to the currently processed tri mesh
    boost::shared_ptr< WTriangleMesh > m_outMesh; //!< A pointer to the currently processed tri mesh
    std::vector<size_t>m_refs; //!< a vector holding the cluster id corresponding to each vertex

    unsigned int m_datasetSizeX; //!< Size of the dataset (X)
    unsigned int m_datasetSizeY; //!< Size of the dataset (Y)
    unsigned int m_datasetSizeZ; //!< Size of the dataset (Z)
    bool m_blankOutMesh; //!< if set mesh colors must be initialized

    boost::shared_ptr< WCondition > m_propCondition;    //!< A condition used to notify about changes in several properties.
    WPropColor  m_propNonActiveColor; //!< color for the non labeled texture voxels
    WPropColor  m_propHoleColor; //!< color for the non labeled texture voxels
    WPropBool   m_monochrome; //!< if used all labels will have m_propNonActiveColor
    WPropDouble  m_propProjectDistance; //!< maximum distance to project the voxels
    WPropFilename m_propLabelFolder; //!< folder where the labels will be written
    WPropFilename m_propAnnotationFile; //!< filename for the annotation file
    WPropTrigger  m_propWriteTrigger; //!< trigger the writing of the labels
    WPropInt m_infoTotalVertices; //!< Info property: number of vertices
    WPropInt m_infoUnassignedVertices; //!< Info property: number of vertices without match
};

#endif  // WMPARTITION2MESH_H
