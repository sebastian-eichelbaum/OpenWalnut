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

#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WGEViewer.h"
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "WTransparentLinesDrawable.h"

namespace
{
    float depth( osg::Vec3f pos, wmath::WPosition viewDir )
    {
        return ( pos - wmath::WPosition() ) * viewDir;
    }

    class MySorting
    {
    public:
        int operator()( std::pair< float, size_t > p1 , std::pair< float, size_t > p2 )
            {
                return ( p1.first < p2.first );
            }
    };
}

void WTransparentLinesDrawable::drawImplementation( osg::RenderInfo &renderInfo ) const //NOLINT
{
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > viewer; //!< Stores reference to the main viewer
    viewer = ge->getViewerByName( "main" );
    wmath::WPosition endPos = wmath::WPosition( wge::unprojectFromScreen( wmath::WPosition( 0.0, 0.0, 1.0 ), viewer->getCamera() ) );
    wmath::WPosition startPos = wmath::WPosition( wge::unprojectFromScreen( wmath::WPosition(), viewer->getCamera() ) );
    wmath::WPosition viewDir = ( endPos - startPos ).normalized();

    std::vector< std::pair< float, size_t > > depthVals( _vertexData.array->getNumElements() );
    for( size_t i = 0; i < _vertexData.array->getNumElements(); i += 2 )
    {
        // std::cout << viewDir << " depth: " << depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[i], viewDir ) << std::endl;
        float myDepth = depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[i], viewDir );
        depthVals[i]   = std::make_pair( myDepth, i );
        depthVals[i+1] = std::make_pair( myDepth, i+1 );
    }

    sort( depthVals.begin(), depthVals.end(), MySorting() );

    osg::ref_ptr< osg::Vec3Array > tmp( new osg::Vec3Array( _vertexData.array->getNumElements() ) );
    for( size_t i = 0; i < _vertexData.array->getNumElements(); ++i )
    {
        (*tmp)[i] = (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[ depthVals[i].second ];
    }

    setVertexArray( tmp );
    // _vertexData.array = tmp;


    osg::Geometry::drawImplementation( renderInfo );
}
