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

#ifndef WMDETTRACTCLUSTERINGGP_H
#define WMDETTRACTCLUSTERINGGP_H

#include <string>
#include <map>
#include <utility>

#include <osg/Geode>

#include "../../../common/math/WMatrixSym.h"
#include "../../../kernel/WModule.h"
#include "../../../kernel/WModuleInputData.h"
#include "../../../kernel/WModuleOutputData.h"
#include "../WDataSetGP.h"

class WDendrogram;

/**
 * Module for clustering gaussian processes which representing deterministic tracts.
 *
 * \ingroup modules
 */
class WMDetTractClusteringGP: public WModule
{
public:

    /**
     * Constructs a new clustering instance.
     */
    WMDetTractClusteringGP();

    /**
     * Destructs this.
     */
    virtual ~WMDetTractClusteringGP();

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
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Represents an edge from one vertex/tract to another one.
     */
    typedef std::pair< size_t, size_t > Edge;

    /**
     * Implicit definition of an Minimum Spanning Tree with weighted edges. First is the weight of second the edge. The vertexes
     * or tracts are just implicit given with the number.
     */
    typedef std::multimap< double, Edge > MST;

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
     * Compute the longest segment of all segments of all tracts/gaussian processes.
     *
     * \param dataSet The dataset of gaussian processes.
     *
     * \return The length of the longest segement overall.
     */
    double searchGlobalMaxSegementLength( boost::shared_ptr< const WDataSetGP > dataSet ) const;

    /**
     * Computes the distant matrix for all pairs of gaussian processes.
     *
     * \param dataSet The dataset of gaussian processes.
     *
     * \return The similarity or also called distant matrix.
     */
    void computeDistanceMatrix( boost::shared_ptr< const WDataSetGP > dataSet );

    /**
     * Constructs Euclidean minimal spanning tree (EMST) as preprocessing for dendrogram.
     * \param dataSet Gaussian processes
     * \return Euclidean spanning tree (map from weight (double) to edge (pair<size_t,size_t>))
     */
    boost::shared_ptr< WMDetTractClusteringGP::MST > computeEMST( boost::shared_ptr< const WDataSetGP > dataSet ) const;

    /**
     * Constructs single linkage agglomerative clustering from EMST.
     * \param edges Sorted weighted edges of EMST
     * \return The dendrogram
     */
    boost::shared_ptr< WDendrogram > computeDendrogram( boost::shared_ptr< const WMDetTractClusteringGP::MST > edges ) const;

    /**
     * Input Connector for the gaussian processes which are about to be clustered.
     */
    boost::shared_ptr< WModuleInputData< WDataSetGP > > m_gpIC;

    /**
     * The maximal segment length of all segments of all tracts. In other words: there will be no
     * other segment of a tract which is bigger than this.
     */
    double m_maxSegmentLength;

    /**
     * Distant matrix of all pairs of gaussian processes.
     */
    WMatrixSymFLT m_similarities;

private:
};

#endif  // WMDETTRACTCLUSTERINGGP_H
