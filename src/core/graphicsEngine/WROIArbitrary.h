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
#include <vector>

#include <boost/thread.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/math/WMatrix.h"
#include "../common/WColor.h"
#include "WPickHandler.h"
#include "WGEViewer.h"

#include "WTriangleMesh.h"

#include "WROI.h"
#include "WExportWGE.h"

class WDataSetScalar;

/**
 * A box containing information on an arbitrarily shaped a region of interest.
 */
class WGE_EXPORT WROIArbitrary : public WROI
{
public:
    /**
     * constructor
     * \param nbCoordsX number of vertices in X direction
     * \param nbCoordsY number of vertices in Y direction
     * \param nbCoordsZ number of vertices in Z direction
     * \param mat the matrix transforming the vertices from canonical space
     * \param vals the values at the vertices
     * \param triMesh
     * \param threshold
     * \param maxThreshold The maximum of the values.
     * \param color the color to use for the ROI.
     */
    WROIArbitrary( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                   const WMatrix< double >& mat,
                   const std::vector< float >& vals,
                   boost::shared_ptr< WTriangleMesh > triMesh,
                   float threshold,
                   float maxThreshold,
                   WColor color );

    /**
     * constructor
     * \param nbCoordsX number of vertices in X direction
     * \param nbCoordsY number of vertices in Y direction
     * \param nbCoordsZ number of vertices in Z direction
     * \param mat the matrix transforming the vertices from canonical space
     * \param vals the values at the vertices
     * \param maxThreshold The maximum of the values.
     * \param color the color to use for the ROI.
     */
    WROIArbitrary( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                   const WMatrix< double >& mat,
                   const std::vector< float >& vals,
                   float maxThreshold,
                   WColor color );


    /**
     * destructor
     */
    virtual ~WROIArbitrary();

    /**
     * initalizes the properties
     */
    void properties();

    /**
     *
     */
    void propertyChanged();

    /**
     * setter
     * \param threshold
     */
    void setThreshold( double threshold );

    /**
     * getter
     *
     * \return The threshold on the data in box which leads to the arbitrary ROI
     */
    double getThreshold();

    /**
     * Get the number of vertices in the three coordinate directions
     *
     * \return A vector containing the numbers of vertices
     */
    std::vector< size_t > getCoordDimensions();

    /**
     * Get the vertex offsets in the three coordinate directions
     *
     * \return The offsets between point in each of the three coordinate directions
     */
    std::vector< double > getCoordOffsets();

    /**
     * Get the i-th value of the data defining the ROI
     * \param i the index of the value
     *
     * \return The value at the given index.
     */
    float getValue( size_t i );

    /**
     *  updates the graphics
     */
    virtual void updateGFX();

protected:
private:
    std::vector< size_t > m_nbCoordsVec; //!< The data's number of vertices in X, Y and Z direction.

    WMatrix< double > m_matrix; //!< The 4x4 transformation matrix for the vertices.

    const std::vector< float > m_vals; //!< The data at the vertices.

    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    WPropDouble m_threshold; //!< the threshold

    /**
     * The ROI color
     */
    WColor m_color;

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
            if( module )
            {
                module->updateGFX();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WROIARBITRARY_H
