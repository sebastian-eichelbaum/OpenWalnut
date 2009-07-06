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

#include <string>
#include <vector>

#include "WMetaInfo.h"


WMetaInfo::WMetaInfo() :
    m_name( "" ),
    m_fileName( "" ),
    m_dataType( 0 ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 0.0 ),
    m_dy( 0.0 ),
    m_dz( 0.0 ),
    m_xyzUnits( 0 ),
    m_xyzUnitsName( "" ),
    m_qformCode( 0 ),
    m_qformCodeName( "" ),
    // m_qtoXyzMatrix
    // m_qtoIjkMatrix
    m_quaternB( 0.0 ),
    m_quaternC( 0.0 ),
    m_quaternD( 0.0 ),
    m_qoffsetX( 0.0 ),
    m_qoffsetY( 0.0 ),
    m_qoffsetZ( 0.0 ),
    m_qfac( 0.0 ),
    m_qformOrientationI( 0 ),
    m_qformOrientationJ( 0 ),
    m_qformOrientationK( 0 ),
    m_freqDim( 0 ),
    m_phaseDim( 0 ),
    m_sliceDim( 0 )
{
}

std::string WMetaInfo::getName() const
{
    return m_name;
}

void WMetaInfo::setName( std::string name )
{
    m_name = name;
}

std::string WMetaInfo::getFileName() const
{
    return m_fileName;
}

void WMetaInfo::setFileName( std::string fileName )
{
    m_fileName = fileName;
}

int WMetaInfo::getDataType() const
{
    return m_dataType;
}

void WMetaInfo::setDataType( int dataType )
{
    m_dataType = dataType;
}

int WMetaInfo::WMetaInfo::getNx() const
{
    return m_nx;
}

void WMetaInfo::setNx( int nx )
{
    m_nx = nx;
}

int WMetaInfo::WMetaInfo::getNy() const
{
    return m_ny;
}

void WMetaInfo::setNy( int ny )
{
    m_ny = ny;
}

int WMetaInfo::WMetaInfo::getNz() const
{
    return m_nz;
}

void WMetaInfo::setNz( int nz )
{
    m_nz = nz;
}

float WMetaInfo::getDx() const
{
    return m_dx;
}

void WMetaInfo::setDx( float dx )
{
    m_dx = dx;
}

float WMetaInfo::getDy() const
{
    return m_dy;
}

void WMetaInfo::setDy( float dy )
{
    m_dy = dy;
}

float WMetaInfo::getDz() const
{
    return m_dz;
}

void WMetaInfo::setDz( float dz )
{
    m_dz = dz;
}

int WMetaInfo::getXyzUnits() const
{
    return m_xyzUnits;
}

void WMetaInfo::setXyzUnits( int xyzUnits )
{
    m_xyzUnits = xyzUnits;
}

std::string WMetaInfo::getXyzUnitsName() const
{
    return m_xyzUnitsName;
}

void WMetaInfo::setXyzUnitsName( std::string xyzUnitsName )
{
    m_xyzUnitsName = xyzUnitsName;
}

int WMetaInfo::getFreqDim() const
{
    return m_freqDim;
}

void WMetaInfo::setFreqDim( int freqDim )
{
    m_freqDim = freqDim;
}

int WMetaInfo::getPhaseDim() const
{
    return m_phaseDim;
}

void WMetaInfo::setPhaseDim( int phaseDim )
{
    m_phaseDim = phaseDim;
}

int WMetaInfo::getSliceDim() const
{
    return m_sliceDim;
}

void WMetaInfo::setSliceDim( int sliceDim )
{
    m_sliceDim = sliceDim;
}

int WMetaInfo::getQformCode() const
{
    return m_qformCode;
}

void WMetaInfo::setQformCode( int qformCode )
{
    m_qformCode = qformCode;
}

std::string WMetaInfo::getQformCodeName() const
{
    return m_qformCodeName;
}

void WMetaInfo::setQformCodeName( std::string qformCodeName )
{
    m_qformCodeName = qformCodeName;
}

std::vector<float> WMetaInfo::getQtoXyzMatrix() const
{
    return m_qtoXyzMatrix;
}

void WMetaInfo::setQtoXyzMatrix( std::vector<float> qtoXyzMatrix )
{
    m_qtoXyzMatrix = qtoXyzMatrix;
}

std::vector<float> WMetaInfo::getQtoIjkMatrix() const
{
    return m_qtoIjkMatrix;
}

void WMetaInfo::setQtoIjkMatrix( std::vector<float> qtoIjkMatrix )
{
    m_qtoIjkMatrix = qtoIjkMatrix;
}

float WMetaInfo::getQuaternB() const
{
    return m_quaternB;
}

void WMetaInfo::setQuaternB( float quaternB )
{
    m_quaternB = quaternB;
}

float WMetaInfo::getQuaternC() const
{
    return m_quaternC;
}

void WMetaInfo::setQuaternC( float quaternC )
{
    m_quaternC = quaternC;
}

float WMetaInfo::getQuaternD() const
{
    return m_quaternD;
}

void WMetaInfo::setQuaternD( float quaternD )
{
    m_quaternD = quaternD;
}

float WMetaInfo::getQoffsetX() const
{
    return m_qoffsetX;
}

void WMetaInfo::setQoffsetX( float qoffsetX )
{
    m_qoffsetX = qoffsetX;
}

float WMetaInfo::getQoffsetY() const
{
    return m_qoffsetY;
}

void WMetaInfo::setQoffsetY( float qoffsetY )
{
    m_qoffsetY = qoffsetY;
}

float WMetaInfo::getQoffsetZ() const
{
    return m_qoffsetZ;
}

void WMetaInfo::setQoffsetZ( float qoffsetZ )
{
    m_qoffsetZ = qoffsetZ;
}

float WMetaInfo::getQfac() const
{
    return m_qfac;
}

void WMetaInfo::setQfac( float qfac )
{
    m_qfac = qfac;
}

int WMetaInfo::getQformOrientationI() const
{
    return m_qformOrientationI;
}

void WMetaInfo::setQformOrientationI( int qformOrientationI )
{
    m_qformOrientationI = qformOrientationI;
}

int WMetaInfo::getQformOrientationJ() const
{
    return m_qformOrientationJ;
}

void WMetaInfo::setQformOrientationJ( int qformOrientationJ )
{
    m_qformOrientationJ = qformOrientationJ;
}

int WMetaInfo::getQformOrientationK() const
{
    return m_qformOrientationK;
}

void WMetaInfo::setQformOrientationK( int qformOrientationK )
{
    m_qformOrientationK = qformOrientationK;
}
