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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../common/datastructures/WFiber.h"
#include "../common/WBoundingBox.h"
#include "../common/WColor.h"
#include "../common/WLogger.h"
#include "../common/WPredicateHelper.h"
#include "../common/WPropertyHelper.h"
#include "../graphicsEngine/WGEUtils.h"
#include "exceptions/WDHNoSuchDataSet.h"
#include "WCreateColorArraysThread.h"
#include "WDataSet.h"
#include "WDataSetFibers.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFibers::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFibers::WDataSetFibers()
    : WDataSet()
{
    // default constructor used by the prototype mechanism
}

WDataSetFibers::WDataSetFibers( WDataSetFibers::VertexArray vertices,
                WDataSetFibers::IndexArray lineStartIndexes,
                WDataSetFibers::LengthArray lineLengths,
                WDataSetFibers::IndexArray verticesReverse,
                WBoundingBox boundingBox )
    : WDataSet(),
      m_vertices( vertices ),
      m_lineStartIndexes( lineStartIndexes ),
      m_lineLengths( lineLengths ),
      m_verticesReverse( verticesReverse ),
      m_bb( boundingBox )
{
    WAssert( m_vertices->size() % 3 == 0,  "Invalid vertex array."  );
    init();
}

WDataSetFibers::WDataSetFibers( WDataSetFibers::VertexArray vertices,
                WDataSetFibers::IndexArray lineStartIndexes,
                WDataSetFibers::LengthArray lineLengths,
                WDataSetFibers::IndexArray verticesReverse )
    : WDataSet(),
      m_vertices( vertices ),
      m_lineStartIndexes( lineStartIndexes ),
      m_lineLengths( lineLengths ),
      m_verticesReverse( verticesReverse )
{
    WAssert( m_vertices->size() % 3 == 0,  "Invalid vertex array."  );
    // determine bounding box
    for( size_t i = 0; i < vertices->size()/3; ++i )
    {
        m_bb.expandBy( (*vertices)[ 3 * i + 0 ], (*vertices)[ 3 * i + 1 ], (*vertices)[ 3 * i + 2 ] );
    }
    // remaining initilisation
    init();
}

WDataSetFibers::WDataSetFibers( WDataSetFibers::VertexArray vertices,
                WDataSetFibers::IndexArray lineStartIndexes,
                WDataSetFibers::LengthArray lineLengths,
                WDataSetFibers::IndexArray verticesReverse,
                WBoundingBox boundingBox,
                WDataSetFibers::VertexParemeterArray vertexParameters )
    : WDataSet(),
      m_vertices( vertices ),
      m_lineStartIndexes( lineStartIndexes ),
      m_lineLengths( lineLengths ),
      m_verticesReverse( verticesReverse ),
      m_bb( boundingBox ),
      m_vertexParameters( vertexParameters )
{
    WAssert( m_vertices->size() % 3 == 0,  "Invalid vertex array."  );
    init();
}

WDataSetFibers::WDataSetFibers( WDataSetFibers::VertexArray vertices,
                WDataSetFibers::IndexArray lineStartIndexes,
                WDataSetFibers::LengthArray lineLengths,
                WDataSetFibers::IndexArray verticesReverse,
                WDataSetFibers::VertexParemeterArray vertexParameters )
    : WDataSet(),
      m_vertices( vertices ),
      m_lineStartIndexes( lineStartIndexes ),
      m_lineLengths( lineLengths ),
      m_verticesReverse( verticesReverse ),
      m_vertexParameters( vertexParameters )
{
    WAssert( m_vertices->size() % 3 == 0,  "Invalid vertex array."  );
    // determine bounding box
    for( size_t i = 0; i < vertices->size()/3; ++i )
    {
        m_bb.expandBy( (*vertices)[ 3 * i + 0 ], (*vertices)[ 3 * i + 1 ], (*vertices)[ 3 * i + 2 ] );
    }
    // remaining initilisation
    init();
}

void WDataSetFibers::init()
{
    size_t size = m_vertices->size();
    m_tangents = boost::shared_ptr< std::vector< float > >( new std::vector<float>( size ) );

    boost::shared_ptr< std::vector< float > > globalColors( new std::vector<float>( size ) );
    boost::shared_ptr< std::vector< float > > localColors( new std::vector<float>( size ) );
    boost::shared_ptr< std::vector< float > > customColors( new std::vector<float>( size ) );

    // TODO(all): use the new WThreadedJobs functionality
    WCreateColorArraysThread* t1 = new WCreateColorArraysThread( 0, m_lineLengths->size()/4, m_vertices,
            m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t2 = new WCreateColorArraysThread( m_lineLengths->size()/4+1, m_lineLengths->size()/2, m_vertices,
            m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t3 = new WCreateColorArraysThread( m_lineLengths->size()/2+1, m_lineLengths->size()/4*3, m_vertices,
            m_lineLengths, globalColors, localColors, m_tangents );
    WCreateColorArraysThread* t4 = new WCreateColorArraysThread( m_lineLengths->size()/4*3+1, m_lineLengths->size()-1, m_vertices,
            m_lineLengths, globalColors, localColors, m_tangents );
    t1->run();
    t2->run();
    t3->run();
    t4->run();

    t1->wait();
    t2->wait();
    t3->wait();
    t4->wait();

    delete t1;
    delete t2;
    delete t3;
    delete t4;

    // add both arrays to m_colors
    m_colors = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colors->push_back( boost::shared_ptr< WItemSelectionItem >(
            new ColorScheme( "Global Color", "Colors direction by using start and end vertex per fiber.", NULL, globalColors, ColorScheme::RGB )
        )
    );
    m_colors->push_back( boost::shared_ptr< WItemSelectionItem >(
            new ColorScheme( "Local Color", "Colors direction by using start and end vertex per segment.", NULL, localColors, ColorScheme::RGB )
        )
    );

    for( size_t i = 0; i < size; ++i )
    {
        ( *customColors )[i] = ( *globalColors )[i];
    }
    m_colors->push_back( boost::shared_ptr< WItemSelectionItem >(
            new ColorScheme( "Custom Color", "Colors copied from the global colors, will be used for bundle coloring.",
                    NULL, customColors, ColorScheme::RGB )
        )
    );

    // the colors can be selected by properties
    m_colorProp = m_properties->addProperty( "Color Scheme", "Determines the coloring scheme to use for this data.", m_colors->getSelectorFirst() );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorProp );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_colorProp );
    m_infoProperties->addProperty( "#Fibers", "The number of fibers", static_cast< WPVBaseTypes::PV_INT >( m_lineLengths->size() ) );
    m_infoProperties->addProperty( "#Vertices", "The number of vertices", static_cast< WPVBaseTypes::PV_INT >( m_vertices->size() ) );
}

bool WDataSetFibers::isTexture() const
{
    return false;
}

size_t WDataSetFibers::size() const
{
    return m_lineStartIndexes->size();
}

const std::string WDataSetFibers::getName() const
{
    return "WDataSetFibers";
}

const std::string WDataSetFibers::getDescription() const
{
    return "Contains tracked fiber data.";
}

boost::shared_ptr< WPrototyped > WDataSetFibers::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFibers() );
    }

    return m_prototype;
}

WDataSetFibers::VertexArray WDataSetFibers::getVertices() const
{
    return m_vertices;
}

WDataSetFibers::IndexArray WDataSetFibers::getLineStartIndexes() const
{
    return m_lineStartIndexes;
}

WDataSetFibers::LengthArray WDataSetFibers::getLineLengths() const
{
    return m_lineLengths;
}

WDataSetFibers::IndexArray WDataSetFibers::getVerticesReverse() const
{
    return m_verticesReverse;
}

WDataSetFibers::TangentArray WDataSetFibers::getTangents() const
{
    return m_tangents;
}

void WDataSetFibers::addColorScheme( WDataSetFibers::ColorArray colors, std::string name, std::string description )
{
    ColorScheme::ColorMode mode = ColorScheme::GRAY;

    // number of verts is needed to distinguish color mode.
    size_t verts = m_vertices->size() / 3;
    size_t cols  = colors->size();
    if( cols / verts == 3 )
    {
        mode = ColorScheme::RGB;
    }
    else if( cols / verts == 4 )
    {
        mode = ColorScheme::RGBA;
    }

    m_colors->push_back( boost::shared_ptr< WItemSelectionItem >(
        new ColorScheme( name, description, NULL, colors, mode )
        )
    );
}

void WDataSetFibers::removeColorScheme( WDataSetFibers::ColorArray colors )
{
    // this is nearly the same like std::remove_if
    WItemSelection::WriteTicket l = m_colors->getWriteTicket();

    WItemSelection::Iterator i = l->get().begin();
    while( i != l->get().end() )
    {
        if( boost::static_pointer_cast< const ColorScheme >( *i )->getColor() == colors )
        {
            i = l->get().erase( i );
        }
        else
        {
            ++i;
        }
    }
}

void WDataSetFibers::replaceColorScheme( WDataSetFibers::ColorArray oldColors, WDataSetFibers::ColorArray newColors )
{
    // this is nearly the same as std::replace_if
    WItemSelection::WriteTicket l = m_colors->getWriteTicket();
    for( WItemSelection::Iterator i = l->get().begin(); i != l->get().end(); ++i )
    {
        boost::shared_ptr< ColorScheme > ci = boost::static_pointer_cast< ColorScheme >( *i );
        if(ci->getColor() == oldColors )
        {
            ci->setColor( newColors );
        }
    }
}

const boost::shared_ptr< WDataSetFibers::ColorScheme > WDataSetFibers::getColorScheme( std::string name ) const
{
    WItemSelection::ReadTicket l = m_colors->getReadTicket();
    WItemSelection::ConstIterator i = std::find_if( l->get().begin(), l->get().end(),
            WPredicateHelper::Name< boost::shared_ptr< WItemSelectionItem > >( name )
    );
    if( i == l->get().end() )
    {
        throw WDHNoSuchDataSet( std::string( "Color scheme with specified name could not be found." ) );
    }

    return boost::static_pointer_cast< ColorScheme >( *i );
}

const boost::shared_ptr< WDataSetFibers::ColorScheme > WDataSetFibers::getColorScheme( size_t idx ) const
{
    WItemSelection::ReadTicket l = m_colors->getReadTicket();
    return boost::static_pointer_cast< ColorScheme >( l->get()[ idx ] );
}

const boost::shared_ptr< WDataSetFibers::ColorScheme > WDataSetFibers::getColorScheme() const
{
    return boost::static_pointer_cast< ColorScheme >( m_colorProp->get().at( 0 ) );
}

const WPropSelection WDataSetFibers::getColorSchemeProperty() const
{
    return m_colorProp;
}

WDataSetFibers::VertexParemeterArray WDataSetFibers::getVertexParameters() const
{
    return m_vertexParameters;
}

WPosition WDataSetFibers::getPosition( size_t fiber, size_t vertex ) const
{
    size_t index = m_lineStartIndexes->at( fiber ) * 3;
    index += vertex * 3;
    return WPosition( m_vertices->at( index ), m_vertices->at( index + 1 ), m_vertices->at( index + 2 ) );
}

WPosition WDataSetFibers::getTangent( size_t fiber, size_t vertex ) const
{
    WPosition point = getPosition( fiber, vertex );
    WPosition tangent;

    if( vertex == 0 ) // first point
    {
        WPosition pointNext = getPosition( fiber, vertex + 1 );
        tangent = point - pointNext;
    }
    else if( vertex == m_lineLengths->at( fiber ) - 1 ) // last point
    {
        WPosition pointBefore = getPosition( fiber, vertex - 1 );
        tangent = pointBefore - point;
    }
    else // somewhere in between
    {
        WPosition pointBefore = getPosition( fiber, vertex - 1 );
        WPosition pointNext = getPosition( fiber, vertex + 1 );
        tangent = pointBefore - pointNext;
    }

    return normalize( tangent );
}

WBoundingBox WDataSetFibers::getBoundingBox() const
{
    return m_bb;
}

WFiber WDataSetFibers::operator[]( size_t numTract ) const
{
    WAssert( numTract < m_lineLengths->size(), "WDataSetFibers: out of bounds - invalid tract number requested." );
    WFiber result;
    result.reserve( ( *m_lineLengths )[ numTract ] );
    size_t vIdx = ( *m_lineStartIndexes )[ numTract ] * 3;
    for( size_t vertexNum = 0; vertexNum < ( *m_lineLengths )[ numTract ]; ++vertexNum )
    {
        result.push_back( WPosition( ( *m_vertices )[vIdx], ( *m_vertices )[vIdx + 1], ( *m_vertices )[vIdx + 2]  ) );
        vIdx += 3;
    }
    return result;
}

WDataSetFibers::const_iterator WDataSetFibers::begin() const
{
    return WFiberIterator( this, 0 );
}

WDataSetFibers::const_iterator WDataSetFibers::end() const
{
    return WFiberIterator( this, m_lineLengths->size() );
}

WFiberIterator::WFiberIterator()
    : m_fibers( NULL ),
      m_index( 0 )
{
}

WFiberIterator::WFiberIterator( WDataSetFibers const* fibers, std::size_t idx )
    : m_fibers( fibers ),
      m_index( idx )
{
}

WFiberIterator::WFiberIterator( WFiberIterator const& iter )
    : m_fibers( iter.m_fibers ),
      m_index( iter.m_index )
{
}

WFiberIterator::~WFiberIterator()
{
}

WFiberIterator& WFiberIterator::operator= ( WFiberIterator const& iter )
{
    if( this == &iter )
    {
        return *this;
    }

    m_fibers = iter.m_fibers;
    m_index = iter.m_index;

    return *this;
}

WFiberIterator& WFiberIterator::operator++()
{
    ++m_index;
    return *this;
}

WFiberIterator WFiberIterator::operator++( int )
{
    WFiberIterator t( m_fibers, m_index );
    ++m_index;
    return t;
}

WFiberIterator& WFiberIterator::operator--()
{
    --m_index;
    return *this;
}

WFiberIterator WFiberIterator::operator--( int )
{
    WFiberIterator t( m_fibers, m_index );
    --m_index;
    return t;
}

bool WFiberIterator::operator==( WFiberIterator const& rhs ) const
{
    return m_fibers == rhs.m_fibers && m_index == rhs.m_index;
}

bool WFiberIterator::operator!=( WFiberIterator const& rhs ) const
{
    return !( this->operator==( rhs ) );
}

std::size_t WFiberIterator::numPoints() const
{
    WAssert( m_index < m_fibers->getLineLengths()->size(), "" );

    return m_fibers->getLineLengths()->operator[] ( m_index );
}

WFiberPointsIterator WFiberIterator::begin()
{
    return WFiberPointsIterator( m_fibers, m_index, 0 );
}

WFiberPointsIterator WFiberIterator::end()
{
    WAssert( numPoints() != 0, "" );

    return WFiberPointsIterator( m_fibers, m_index, numPoints() );
}

WFiberPointsIterator WFiberIterator::rbegin()
{
    return WFiberPointsIterator( m_fibers, m_index, 0, true );
}

WFiberPointsIterator WFiberIterator::rend()
{
    WAssert( numPoints() != 0, "" );

    return WFiberPointsIterator( m_fibers, m_index, numPoints(), true );
}

WFiberSegmentsIterator WFiberIterator::sbegin()
{
    return WFiberSegmentsIterator( m_fibers, m_index, 0 );
}

WFiberSegmentsIterator WFiberIterator::send()
{
    WAssert( numPoints() != 0, "" );

    return WFiberSegmentsIterator( m_fibers, m_index, numPoints() - 1 );
}

WFiberSegmentsIterator WFiberIterator::srbegin()
{
    return WFiberSegmentsIterator( m_fibers, m_index, 0, true );
}

WFiberSegmentsIterator WFiberIterator::srend()
{
    WAssert( numPoints() != 0, "" );

    return WFiberSegmentsIterator( m_fibers, m_index, numPoints() - 1, true );
}

std::size_t WFiberIterator::getLineStartIndex() const
{
    return m_fibers->getLineStartIndexes()->operator[]( getIndex() );
}

std::size_t WFiberIterator::getIndex() const
{
    return m_index;
}

WFiberPointsIterator::WFiberPointsIterator()
    : m_fibers( NULL ),
      m_fiberIndex( 0 ),
      m_index( 0 ),
      m_reverse( false )
{
}

WFiberPointsIterator::WFiberPointsIterator( WDataSetFibers const* fibers, std::size_t fbIdx, std::size_t idx, bool reverse )
    : m_fibers( fibers ),
      m_fiberIndex( fbIdx ),
      m_index( idx ),
      m_reverse( reverse )
{
}

WFiberPointsIterator::WFiberPointsIterator( WFiberPointsIterator const& iter )
    : m_fibers( iter.m_fibers ),
      m_fiberIndex( iter.m_fiberIndex ),
      m_index( iter.m_index ),
      m_reverse( iter.m_reverse )
{
}

WFiberPointsIterator::~WFiberPointsIterator()
{
}

WFiberPointsIterator& WFiberPointsIterator::operator=( WFiberPointsIterator const& iter )
{
    if( this == &iter )
    {
        return *this;
    }

    m_fibers = iter.m_fibers;
    m_fiberIndex = iter.m_fiberIndex;
    m_index = iter.m_index;
    m_reverse = iter.m_reverse;

    return *this;
}

WFiberPointsIterator& WFiberPointsIterator::operator++()
{
    ++m_index;
    return *this;
}

WFiberPointsIterator WFiberPointsIterator::operator++( int )
{
    WFiberPointsIterator t( m_fibers, m_fiberIndex, m_index, m_reverse );
    ++m_index;
    return t;
}

WFiberPointsIterator& WFiberPointsIterator::operator--()
{
    --m_index;
    return *this;
}

WFiberPointsIterator WFiberPointsIterator::operator--( int )
{
    WFiberPointsIterator t( m_fibers, m_fiberIndex, m_index, m_reverse );
    --m_index;
    return t;
}

std::size_t WFiberPointsIterator::getBaseIndex() const
{
    WAssert( m_fibers, "" );
    WAssert( m_fiberIndex < m_fibers->getLineLengths()->size(), "" );
    WAssert( m_index < m_fibers->getLineLengths()->operator[] ( m_fiberIndex ), "" );

    std::size_t i = m_index;
    if( m_reverse )
    {
        i = m_fibers->getLineLengths()->operator[] ( m_fiberIndex ) - i - 1;
    }
    return m_fibers->getLineStartIndexes()->operator[] ( m_fiberIndex ) + i;
}

WPosition WFiberPointsIterator::operator*()
{
    std::size_t v = getBaseIndex();
    return WPosition( m_fibers->getVertices()->operator[]( 3 * v + 0 ),
                      m_fibers->getVertices()->operator[]( 3 * v + 1 ),
                      m_fibers->getVertices()->operator[]( 3 * v + 2 ) );
}

bool WFiberPointsIterator::operator==( WFiberPointsIterator const& rhs ) const
{
    if( m_reverse != rhs.m_reverse )
    {
        wlog::warn( "FiberPointsIterator" ) << "Comparing a reverse and a normal iterator!";
    }

    return m_fibers == rhs.m_fibers && m_fiberIndex == rhs.m_fiberIndex && m_index == rhs.m_index && m_reverse == rhs.m_reverse;
}

bool WFiberPointsIterator::operator!=( WFiberPointsIterator const& rhs ) const
{
    return !( this->operator==( rhs ) );
}

double WFiberPointsIterator::getParameter( double def ) const
{
    if( m_fibers->getVertexParameters() )
    {
        return m_fibers->getVertexParameters()->operator[]( getBaseIndex() );
    }
    return def;
}

WPosition WFiberPointsIterator::getTangent() const
{
    std::size_t v = getBaseIndex();
    return WPosition( m_fibers->getTangents()->operator[]( 3 * v + 0 ),
                      m_fibers->getTangents()->operator[]( 3 * v + 1 ),
                      m_fibers->getTangents()->operator[]( 3 * v + 2 ) );
}


WColor WFiberPointsIterator::getColor( const boost::shared_ptr< WDataSetFibers::ColorScheme > scheme ) const
{
    std::size_t v = getBaseIndex();
    WColor ret;
    switch( scheme->getMode() )
    {
        case WDataSetFibers::ColorScheme::GRAY:
            {
                double r = scheme->getColor()->operator[]( 1 * v + 0 );
                ret.set( r, r, r, 1.0 );
            }
            break;
        case WDataSetFibers::ColorScheme::RGB:
            {
                double r = scheme->getColor()->operator[]( 3 * v + 0 );
                double g = scheme->getColor()->operator[]( 3 * v + 1 );
                double b = scheme->getColor()->operator[]( 3 * v + 2 );
                ret.set( r, g, b, 1.0 );
            }
            break;
        case WDataSetFibers::ColorScheme::RGBA:
            {
                double r = scheme->getColor()->operator[]( 4 * v + 0 );
                double g = scheme->getColor()->operator[]( 4 * v + 1 );
                double b = scheme->getColor()->operator[]( 4 * v + 2 );
                double a = scheme->getColor()->operator[]( 4 * v + 3 );
                ret.set( r, g, b, a );
            }
            break;
        default:
            ret.set( 1.0, 1.0, 1.0, 1.0 );
            break;
    }
    return ret;
}

WColor WFiberPointsIterator::getColor() const
{
    return getColor( m_fibers->getColorScheme() );
}

WColor WFiberPointsIterator::getColor( std::size_t idx ) const
{
    return getColor( m_fibers->getColorScheme( idx ) );
}

WColor WFiberPointsIterator::getColor( std::string name ) const
{
    return getColor( m_fibers->getColorScheme( name ) );
}

WFiberSegmentsIterator::WFiberSegmentsIterator()
    : m_fibers( NULL ),
      m_fiberIndex( 0 ),
      m_index( 0 ),
      m_reverse( false )
{
}

WFiberSegmentsIterator::WFiberSegmentsIterator( WDataSetFibers const* fibers, std::size_t fbIdx, std::size_t idx, bool reverse )
    : m_fibers( fibers ),
      m_fiberIndex( fbIdx ),
      m_index( idx ),
      m_reverse( reverse )
{
}

WFiberSegmentsIterator::WFiberSegmentsIterator( WFiberSegmentsIterator const& iter )
    : m_fibers( iter.m_fibers ),
      m_fiberIndex( iter.m_fiberIndex ),
      m_index( iter.m_index ),
      m_reverse( iter.m_reverse )
{
}

WFiberSegmentsIterator::~WFiberSegmentsIterator()
{
}

WFiberSegmentsIterator& WFiberSegmentsIterator::operator=( WFiberSegmentsIterator const& iter )
{
    if( this == &iter )
    {
        return *this;
    }

    m_fibers = iter.m_fibers;
    m_fiberIndex = iter.m_fiberIndex;
    m_index = iter.m_index;
    m_reverse = iter.m_reverse;

    return *this;
}

WFiberSegmentsIterator& WFiberSegmentsIterator::operator++()
{
    ++m_index;
    return *this;
}

WFiberSegmentsIterator WFiberSegmentsIterator::operator++( int )
{
    WFiberSegmentsIterator t( m_fibers, m_fiberIndex, m_index, m_reverse );
    ++m_index;
    return t;
}

WFiberSegmentsIterator& WFiberSegmentsIterator::operator--()
{
    --m_index;
    return *this;
}

WFiberSegmentsIterator WFiberSegmentsIterator::operator--( int )
{
    WFiberSegmentsIterator t( m_fibers, m_fiberIndex, m_index, m_reverse );
    --m_index;
    return t;
}

bool WFiberSegmentsIterator::operator==( WFiberSegmentsIterator const& rhs ) const
{
    if( m_reverse != rhs.m_reverse )
    {
        wlog::warn( "WFiberSegmentsIterator" ) << "Comparing a reverse and a normal iterator!";
    }

    return m_fibers == rhs.m_fibers && m_fiberIndex == rhs.m_fiberIndex && m_index == rhs.m_index && m_reverse == rhs.m_reverse;
}

bool WFiberSegmentsIterator::operator!=( WFiberSegmentsIterator const& rhs ) const
{
    return !( this->operator==( rhs ) );
}

WFiberPointsIterator WFiberSegmentsIterator::start() const
{
    return WFiberPointsIterator( m_fibers, m_fiberIndex, m_index, m_reverse );
}

WFiberPointsIterator WFiberSegmentsIterator::end() const
{
    return WFiberPointsIterator( m_fibers, m_fiberIndex, m_index + 1, m_reverse );
}

osg::Vec3 WFiberSegmentsIterator::direction() const
{
    if( m_index > m_fibers->getLineLengths()->at( m_fiberIndex ) - 2 )
        return osg::Vec3( 0.0, 0.0, 0.0 );

    return *end() - *start();
}

