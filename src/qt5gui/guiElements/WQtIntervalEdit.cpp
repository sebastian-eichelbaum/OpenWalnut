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

#include "../WGuiConsts.h"

#include "WQtIntervalEdit.h"

int WQtIntervalEditBase::SliderResolution = 10000;

WQtIntervalEditBase::WQtIntervalEditBase( QWidget* parent ):
    QWidget( parent ),
    m_minSlider( Qt::Horizontal, this ),
    m_maxSlider( Qt::Horizontal, this ),
    m_minEdit( this ),
    m_maxEdit( this ),
    m_minLabel( "Minimum:", this ),
    m_maxLabel( "Maximum:", this ),
    m_layout( this )
{
    // initialize members
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    m_layout.addWidget( &m_minLabel, 0, 0 );
    m_layout.addWidget( &m_minSlider, 0, 1 );
    m_layout.addWidget( &m_minEdit, 0, 2 );
    m_layout.addWidget( &m_maxLabel, 1, 0 );
    m_layout.addWidget( &m_maxSlider, 1, 1 );
    m_layout.addWidget( &m_maxEdit, 1, 2 );

    m_minSlider.setMinimum( 0 );
    m_minSlider.setMaximum( SliderResolution );
    m_maxSlider.setMinimum( 0 );
    m_maxSlider.setMaximum( SliderResolution );

    m_minSlider.setValue( 0 );
    m_maxSlider.setValue( SliderResolution );

    setLayout( &m_layout );

    connect( &m_minSlider, SIGNAL( valueChanged( int ) ), this, SLOT( minSliderChanged() ) );
    connect( &m_maxSlider, SIGNAL( valueChanged( int ) ), this, SLOT( maxSliderChanged() ) );
    connect( &m_minEdit, SIGNAL( editingFinished() ), this, SLOT( minEditChanged() ) );
    connect( &m_maxEdit, SIGNAL( editingFinished() ), this, SLOT( maxEditChanged() ) );
}

WQtIntervalEditBase::~WQtIntervalEditBase()
{
    // cleanup
}

void WQtIntervalEditBase::minSliderChanged()
{
}

void WQtIntervalEditBase::maxSliderChanged()
{
}

void WQtIntervalEditBase::minEditChanged()
{
}

void WQtIntervalEditBase::maxEditChanged()
{
}

