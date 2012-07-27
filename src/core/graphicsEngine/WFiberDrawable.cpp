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

#include <vector>

#include "../kernel/WKernel.h"

#include "WFiberDrawable.h"

// The constructor here does nothing. One thing that may be necessary is
// disabling display lists. This can be done by calling
//    setSupportsDisplayList (false);
// Display lists should be disabled for 'Drawable's that can change over
// time (that is, the vertices drawn change from time to time).
WFiberDrawable::WFiberDrawable():
    osg::Drawable(),
    m_useTubes( false )
{
    setSupportsDisplayList( false );
    // This contructor intentionally left blank. Duh.
}

// I can't say much about the methods below, but OSG seems to expect
// that we implement them.
WFiberDrawable::WFiberDrawable( const WFiberDrawable& /*pg*/, const osg::CopyOp& /*copyop*/ ):
    osg::Drawable()
{
}

osg::Object* WFiberDrawable::cloneType() const
{
    return new WFiberDrawable();
}

osg::Object* WFiberDrawable::clone( const osg::CopyOp& copyop ) const
{
    return new WFiberDrawable( *this, copyop );
}

// Real work is done here. THERE IS A VERY IMPORTANT THING TO NOTE HERE:
// the 'drawImplementation()' method receives an 'osg::State' as
// parameter. This can be used to change the OpenGL state, but changing
// the OpenGL state here is something to be avoided as much as possible.
// Do this *only* if it is *absolutely* necessary to make your rendering
// algorithm work. The "right" (most efficient and flexible) way to change
// the OpenGL state in OSG is by attaching 'StateSet's to 'Node's and
// 'Drawable's.
// That said, the example below shows how to change the OpenGL state in
// these rare cases in which it is necessary. But always keep in mind:
// *Change the OpenGL state only if strictly necessary*.
void WFiberDrawable::drawImplementation( osg::RenderInfo& renderInfo ) const //NOLINT
{
    if( m_useTubes )
    {
        drawTubes();
    }
    else
    {
        drawFibers( renderInfo );
    }
}

void WFiberDrawable::drawFibers( osg::RenderInfo& renderInfo ) const //NOLINT
{
    osg::State& state = *renderInfo.getState();

    state.disableAllVertexArrays();
    state.setVertexPointer( 3, GL_FLOAT , 0, &( *m_verts )[0] );
    state.setColorPointer( 3 , GL_FLOAT , 0, &( *m_colors )[0] );
    //state.setNormalPointer( GL_FLOAT , 0, &( *m_tangents )[0] );
    for( size_t i = 0; i < m_active->size(); ++i )
    {
        if( (*m_active)[i] )
        {
            state.glDrawArraysInstanced( GL_LINE_STRIP, (*m_startIndexes)[i], (*m_pointsPerLine)[i], 1);
        }
    }

    state.disableVertexPointer();
    state.disableColorPointer();
}

void WFiberDrawable::drawTubes() const
{
    // This does not work if GLES is used
    #ifndef GL_ES_VERSION_2_0
    for( size_t i = 0; i < m_active->size(); ++i )
    {
        if( (*m_active)[i] )
        {
            glBegin( GL_QUAD_STRIP );
            int idx = m_startIndexes->at( i ) * 3;
            for( size_t k = 0; k < m_pointsPerLine->at( i ); ++k )
            {
                glNormal3f( m_tangents->at( idx ), m_tangents->at( idx + 1 ), m_tangents->at( idx + 2 ) );
                glColor3f( m_colors->at( idx ), m_colors->at( idx + 1 ), m_colors->at( idx + 2 ) );
                glTexCoord1f( -1.0f );
                glVertex3f( m_verts->at( idx ), m_verts->at( idx + 1 ), m_verts->at( idx + 2 ) );
                glTexCoord1f( 1.0f );
                glVertex3f( m_verts->at( idx ), m_verts->at( idx + 1 ), m_verts->at( idx + 2 ) );
                idx += 3;
                //
            }
            glEnd();
        }
    }
    #endif
}

