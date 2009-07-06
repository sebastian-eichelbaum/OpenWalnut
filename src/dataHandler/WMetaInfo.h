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

#ifndef WMETAINFO_H
#define WMETAINFO_H

#include <string>
#include <vector>

/**
 * Stores meta information such as types, file names, etc. associated with a
 * WDataSet loaded into OpenWalnut.
 */
class WMetaInfo
{
/**
 * Only UnitTests are allowed to be friends of this class
 */
friend class WMetaInfoTest;

public:
    WMetaInfo();

    std::string getName() const;
    void setName( std::string name );

    std::string getFileName() const;
    void setFileName( std::string fileName );

    int getDataType() const;
    void setDataType( int dataType );

    int getNx() const;
    void setNx( int nx );

    int getNy() const;
    void setNy( int ny );

    int getNz() const;
    void setNz( int nz );

    float getDx() const;
    void setDx( float dx );

    float getDy() const;
    void setDy( float dy );

    float getDz() const;
    void setDz( float dz );

    int getXyzUnits() const;
    void setXyzUnits( int xyzUnits );

    std::string getXyzUnitsName() const;
    void setXyzUnitsName( std::string xyzUnitsName );

    int getFreqDim() const;
    void setFreqDim( int freqDim );

    int getPhaseDim() const;
    void setPhaseDim( int phaseDim );

    int getSliceDim() const;
    void setSliceDim( int sliceDim );

    int getQformCode() const;
    void setQformCode( int qformCode );

    std::string getQformCodeName() const;
    void setQformCodeName( std::string qformCodeName );

    std::vector<float> getQtoXyzMatrix() const;
    void setQtoXyzMatrix( std::vector<float> qtoXyzMatrix );

    std::vector<float> getQtoIjkMatrix() const;
    void setQtoIjkMatrix( std::vector<float> qtoIjkMatrix );

    float getQuaternB() const;
    void setQuaternB( float quaternB );

    float getQuaternC() const;
    void setQuaternC( float quaternC );

    float getQuaternD() const;
    void setQuaternD( float quaternD );

    float getQoffsetX() const;
    void setQoffsetX( float qoffsetX );

    float getQoffsetY() const;
    void setQoffsetY( float qoffsetY );

    float getQoffsetZ() const;
    void setQoffsetZ( float qoffsetZ );

    float getQfac() const;
    void setQfac( float qfac );

    int getQformOrientationI() const;
    void setQformOrientationI( int qformOrientationI );

    int getQformOrientationJ() const;
    void setQformOrientationJ( int qformOrientationJ );

    int getQformOrientationK() const;
    void setQformOrientationK( int qformOrientationK );


protected:

private:
    /**
     * Stores the name of the WDataSet this MetaInfo is associated with
     */
    std::string m_name;

    /**
     * Stores the name of the input file from which this DataSet gets its data.
     */
    std::string m_fileName;

    /**
     * The technical data will contain most of the nifti header for now.
     */

    /**
     *
     */
    int m_dataType;

    /**
     * count voxels in x direction
     */
    int m_nx;

    /**
     * count voxels in y direction
     */
    int m_ny;

    /**
     * count voxels in z direction
     */
    int m_nz;

    /**
     * voxel size in x direction
     */
    float m_dx;

    /**
     * voxel size in y direction
     */
    float m_dy;

    /**
     * voxel size in z direction
     */
    float m_dz;

    /*
     * TODO do we want to keep these fields?
     */

    /**
     * code of the scale unit used
     */
    int m_xyzUnits;

    /**
     * name of the scale unit used
     */
    std::string m_xyzUnitsName;

    /*
     * TODO qforms will take some learning on how to use them
     * for now we just keep them, better explanation will follow
     */

    /**
     * qform code
     */
    int m_qformCode;

    /**
     * qform code name
     */
    std::string m_qformCodeName;

    /**
     * qform xyz matrix
     */
    std::vector< float >m_qtoXyzMatrix;

    /**
     * qform ijk matrix
     */
    std::vector< float >m_qtoIjkMatrix;

    /**
     * qform quaternion b
     */
    float m_quaternB;

    /**
     * qform quaternion c
     */
    float m_quaternC;

    /**
     * qform quaternion dm_slice_dim
     */
    float m_quaternD;

    /**
     * qform offset x
     */
    float m_qoffsetX;

    /**
     * qform offset y
     */
    float m_qoffsetY;

    /**
     * qform offset z
     */
    float m_qoffsetZ;

    /**
     * qform qfac
     */
    float m_qfac;

    /**
     * qform i orientation
     */
    int m_qformOrientationI;

    /**
     * qform j orientation
     */
    int m_qformOrientationJ;

    /**
     * qform k orientation
     */
    int m_qformOrientationK;

    /**
     * freq dim
     */
    int m_freqDim;

    /**
     * phase dim
     */
    int m_phaseDim;

    /**
     * slice dim
     */
    int m_sliceDim;
};

#endif  // WMETAINFO_H
