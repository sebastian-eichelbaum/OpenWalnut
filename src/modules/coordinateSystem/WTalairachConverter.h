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

#ifndef WTALAIRACHCONVERTER_H
#define WTALAIRACHCONVERTER_H

#include "../../common/math/WVector3D.h"
#include "../../common/math/WMatrix.h"

/**
 * class to provide conversions between world or voxel space coordinates und talairach coordinates
 */
class WTalairachConverter
{
public:
    /**
     * constructor
     *
     * \param ac anterior point
     * \param pc posterior point
     * \param ihp interhemispherical plane
     */
    WTalairachConverter( wmath::WVector3D ac, wmath::WVector3D pc, wmath::WVector3D ihp );

    /**
     * destructor
     */
    virtual ~WTalairachConverter();

    /**
     * Canonical2ACPC
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D Canonical2ACPC( const wmath::WVector3D point );

    /**
     * ACPC2Canonical
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D ACPC2Canonical( const wmath::WVector3D point );

    /**
     * Canonical2Talairach
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D Canonical2Talairach( const wmath::WVector3D point );

    /**
     * Talairach2Canonical
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D Talairach2Canonical( const wmath::WVector3D point );

    /**
     * ACPC2Talairach
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D ACPC2Talairach( const wmath::WVector3D point );

    /**
     * Talairach2ACPC
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    wmath::WVector3D Talairach2ACPC( const wmath::WVector3D point );

    /**
     * getter for ac
     *
     * \return vector
     */
    wmath::WVector3D getAc() const;

    /**
     * setter for ac
     *
     * \param ac coordinate of point
     */
    void setAc( wmath::WVector3D ac );

    /**
     * getter for pc
     *
     * \return vector
     */
    wmath::WVector3D getPc() const;

    /**
     * setter for pc
     *
     * \param pc coordinate of point
     */
    void setPc( wmath::WVector3D pc );

    /**
     * getter for ihp
     *
     * \return vector
     */
    wmath::WVector3D getIhp() const;

    /**
     * setter for ihp
     *
     * \param ihp coordinate of point
     */
    void setIhp( wmath::WVector3D ihp );

    /**
     * getter for ap
     *
     * \return vector
     */
    wmath::WVector3D getAp() const;

    /**
     * setter for ap
     *
     * \param ap coordinate of point
     */
    void setAp( wmath::WVector3D ap );

    /**
     * getter for pp
     *
     * \return vector
     */
    wmath::WVector3D getPp() const;

    /**
     * setter for pp
     *
     * \param pp coordinate of point
     */
    void setPp( wmath::WVector3D pp );

    /**
     * getter for sp
     *
     * \return vector
     */
    wmath::WVector3D getSp() const;

    /**
     * setter for sp
     *
     * \param sp coordinate of point
     */
    void setSp( wmath::WVector3D sp );

    /**
     * getter for ip
     *
     * \return vector
     */
    wmath::WVector3D getIp() const;

    /**
     * setter for ip
     *
     * \param ip coordinate of point
     */
    void setIp( wmath::WVector3D ip );

    /**
     * getter for rp
     *
     * \return vector
     */
    wmath::WVector3D getRp() const;

    /**
     * setter for rp
     *
     * \param rp coordinate of point
     */
    void setRp( wmath::WVector3D rp );

    /**
     * getter for lp
     *
     * \return vector
     */
    wmath::WVector3D getLp() const;

    /**
     * setter for lp
     *
     * \param lp coordinate of point
     */
    void setLp( wmath::WVector3D lp );

    /**
     * getter for the rotation matrix
     *
     * \return the rotation matrix
     */
    wmath::WMatrix<double> getRotMat();

    /**
     * getter for the inverted rotation matrix
     *
     * \return the inverted rotation matrix
     */
    wmath::WMatrix<double> getInvRotMat();

    /**
     * projected interhemispherical point
     */
    wmath::WVector3D m_ihp_proj;
protected:
private:
    /**
     * helper routine to create the rotation matrix from the given points
     */
    void defineRotationMatrix();

    /**
     * the rotation matrix
     */
    wmath::WMatrix<double> m_rotMat;

    /**
     * the inverted rotation matrix
     */
    wmath::WMatrix<double> m_rotMatInvert;

    /**
     * ac
     */
    wmath::WVector3D m_ac;

    /**
     * pc
     */
    wmath::WVector3D m_pc;

    /**
     * ihp
     */
    wmath::WVector3D m_ihp;

    /**
     * ap
     */
    wmath::WVector3D m_ap;

    /**
     * pp
     */
    wmath::WVector3D m_pp;

    /**
     * sp
     */
    wmath::WVector3D m_sp;

    /**
     * ip
     */
    wmath::WVector3D m_ip;

    /**
     * rp
     */
    wmath::WVector3D m_rp;

    /**
     * lp
     */
    wmath::WVector3D m_lp;
};

#endif  // WTALAIRACHCONVERTER_H
