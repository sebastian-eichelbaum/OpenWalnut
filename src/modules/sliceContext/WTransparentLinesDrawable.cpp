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
#include <iomanip>
namespace
{
    double depth( osg::Vec3f pos, wmath::WPosition viewDir )
    {
        return ( pos - wmath::WPosition() ) * viewDir;
    }

    class MySorting
    {
    public:
        int operator()( std::pair< double, size_t > p1 , std::pair< double, size_t > p2 )
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

    std::vector< std::pair< double, size_t > > depthVals( _vertexData.array->getNumElements() );
    // std::cout<<"HALLO"<<depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[0], viewDir )<<std::endl;
    // std::cout<<"HALLO"<<depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[1], viewDir )<<std::endl;
    // std::cout<<"HALLO"<<depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[2], viewDir )<<std::endl;
    // std::cout<<"HALLO"<<depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[3], viewDir )<<std::endl;
    // std::cout<<"HALLO"<<depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[4], viewDir )<<std::endl;
    // std::cout<< depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[0], viewDir )<<std::endl;
    for( size_t i = 0; i < _vertexData.array->getNumElements(); i += 2 )
    {
        // std::cout << viewDir << " depth: " << depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[i], viewDir ) << std::endl;
        double myDepth = -1 * depth( (*(dynamic_cast<osg::Vec3Array*>(_vertexData.array.get())))[i], viewDir );
        // TODO(wiebel): improve this unidication of values
        depthVals[i]   = std::make_pair( myDepth, i );
        depthVals[i+1] = std::make_pair( myDepth, i+1 );
    }

    std::stable_sort( depthVals.begin(), depthVals.end(), MySorting() );


    // osg::ref_ptr< osg::Vec3Array > tmp( new osg::Vec3Array( _vertexData.array->getNumElements() ) );
    osg::Vec3Array* oldVec = new  osg::Vec3Array( *(dynamic_cast<osg::Vec3Array*>( _vertexData.array.get() ) ), osg::CopyOp::DEEP_COPY_ALL );
    osg::Vec3Array* tmpVec = const_cast< osg::Vec3Array* >( (dynamic_cast< const osg::Vec3Array*>( _vertexData.array.get()) ) );
    osg::Vec3Array* oldTexCoords = new  osg::Vec3Array( *(dynamic_cast<osg::Vec3Array*>( getTexCoordData(0).array.get() ) ), osg::CopyOp::DEEP_COPY_ALL );
    osg::Vec3Array* tmpTexCoords = const_cast< osg::Vec3Array* >( (dynamic_cast< const osg::Vec3Array*>( getTexCoordData(0).array.get()) ) );
    for( size_t i = 0; i < _vertexData.array->getNumElements(); ++i )
    {
        (*tmpTexCoords)[i] = (*oldTexCoords)[ depthVals[i].second ];
        (*tmpVec)[i] = (*oldVec)[ depthVals[i].second ];
    }

    // delete oldVec;
    // delete oldTexCoords;

    osg::Geometry::drawImplementation( renderInfo );
}
