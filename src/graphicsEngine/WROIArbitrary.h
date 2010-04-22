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

#ifndef WROIARBITRARY_H
#define WROIARBITRARY_H

#include <string>
#include <utility>

#include <boost/thread.hpp>

#include "../common/math/WPosition.h"
#include "WPickHandler.h"
#include "WGEViewer.h"

#include "WTriangleMesh2.h"

#include "WROI.h"

class WDataSetScalar;

/**
 * A box representing a region of interest.
 */
class WROIArbitrary : public WROI
{
public:
    /**
     * constructor
     * \param dataSet
     * \param triMesh
     * \param threshold
     */
    WROIArbitrary( boost::shared_ptr< const WDataSetScalar > dataSet, boost::shared_ptr< WTriangleMesh2 > triMesh, float threshold );

    /**
     * destructor
     */
    virtual ~WROIArbitrary();

    /**
     * setter
     * \param threshold
     */
    void setThreshold( double threshold );

    /**
     * getter
     */
    double getThreshold();

    /**
     * getter
     */
    double getMaxThreshold();

    /**
     * getter
     */
    boost::shared_ptr< const WDataSetScalar > getDataSet();

    /**
     *  updates the graphics
     */
    virtual void updateGFX();


protected:
private:
    boost::shared_ptr< const WDataSetScalar > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    boost::shared_ptr< WTriangleMesh2 > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    double m_threshold; //!< the threshold

    /**
     * Node callback to handle updates properly
     */
    class ROIArbNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WROIArbitrary > module = static_cast< WROIArbitrary* > ( node->getUserData() );
            if ( module )
            {
                module->updateGFX();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WROIARBITRARY_H
