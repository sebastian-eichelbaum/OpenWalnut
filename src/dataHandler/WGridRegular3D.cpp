//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include "../math/WPosition.h"
#include "../math/WVector3D.h"

#include "WGridRegular3D.h"

WGridRegular3D::WGridRegular3D( double originX, double originY, double originZ,
                                unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( WPosition( originX, originY, originZ ) ),
      m_nbPosX( nbPosX ),  m_nbPosY( nbPosY ),  m_nbPosZ( nbPosZ ),
      m_offset( WVector3D( offsetX, offsetY, offsetZ ) )
{
}

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( WPosition( 0., 0., 0. ) ),
      m_nbPosX( nbPosX ),  m_nbPosY( nbPosY ),  m_nbPosZ( nbPosZ ),
      m_offset( WVector3D( offsetX, offsetY, offsetZ ) )
{
}





