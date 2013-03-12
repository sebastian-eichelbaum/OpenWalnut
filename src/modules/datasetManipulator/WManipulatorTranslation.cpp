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

#include "WManipulatorTranslation.h"

WManipulatorTranslation::WManipulatorTranslation()
    : WObjectNDIP< WManipulatorInterface >( "Translation", "Translates the dataset." )
{
    m_translation = m_properties->addProperty( "Translation", "Translation in the word coordinate system.", WPosition( 0.0, 0.0, 0.0 ),
                                                getProperties()->getUpdateCondition() );
}

WManipulatorTranslation::~WManipulatorTranslation()
{
}

WMatrixFixed< double, 4, 4 > WManipulatorTranslation::getTransformationMatrix() const
{
    WMatrixFixed< double, 4, 4 > mat = WMatrixFixed< double, 4, 4 >::identity();
    WPosition translation = m_translation->get( true );

    mat( 0, 3 ) = translation[ 0 ];
    mat( 1, 3 ) = translation[ 1 ];
    mat( 2, 3 ) = translation[ 2 ];
    return mat;
}

bool WManipulatorTranslation::transformationChanged() const
{
    return m_translation->changed();
}

void WManipulatorTranslation::reset()
{
    m_translation->set( WPosition( 0.0, 0.0, 0.0 ), false );
}

