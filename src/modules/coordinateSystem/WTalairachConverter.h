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

#include <boost/shared_ptr.hpp>

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
    WTalairachConverter( WVector3D ac, WVector3D pc, WVector3D ihp );

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
    WVector3D Canonical2ACPC( const WVector3D point );

    /**
     * ACPC2Canonical
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    WVector3D ACPC2Canonical( const WVector3D point );

    /**
     * Canonical2Talairach
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    WVector3D Canonical2Talairach( const WVector3D point );

    /**
     * Talairach2Canonical
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    WVector3D Talairach2Canonical( const WVector3D point );

    /**
     * ACPC2Talairach
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    WVector3D ACPC2Talairach( const WVector3D point );

    /**
     * Talairach2ACPC
     *
     * \param point the coordinates to transform
     *
     * \return vector
     */
    WVector3D Talairach2ACPC( const WVector3D point );

    /**
     * getter for ac
     *
     * \return vector
     */
    WVector3D getAc() const;

    /**
     * setter for ac
     *
     * \param ac coordinate of point
     */
    void setAc( WVector3D ac );

    /**
     * getter for pc
     *
     * \return vector
     */
    WVector3D getPc() const;

    /**
     * setter for pc
     *
     * \param pc coordinate of point
     */
    void setPc( WVector3D pc );

    /**
     * getter for ihp
     *
     * \return vector
     */
    WVector3D getIhp() const;

    /**
     * setter for ihp
     *
     * \param ihp coordinate of point
     */
    void setIhp( WVector3D ihp );

    /**
     * getter for ap
     *
     * \return vector
     */
    WVector3D getAp() const;

    /**
     * setter for ap
     *
     * \param ap coordinate of point
     */
    void setAp( WVector3D ap );

    /**
     * getter for pp
     *
     * \return vector
     */
    WVector3D getPp() const;

    /**
     * setter for pp
     *
     * \param pp coordinate of point
     */
    void setPp( WVector3D pp );

    /**
     * getter for sp
     *
     * \return vector
     */
    WVector3D getSp() const;

    /**
     * setter for sp
     *
     * \param sp coordinate of point
     */
    void setSp( WVector3D sp );

    /**
     * getter for ip
     *
     * \return vector
     */
    WVector3D getIp() const;

    /**
     * setter for ip
     *
     * \param ip coordinate of point
     */
    void setIp( WVector3D ip );

    /**
     * getter for rp
     *
     * \return vector
     */
    WVector3D getRp() const;

    /**
     * setter for rp
     *
     * \param rp coordinate of point
     */
    void setRp( WVector3D rp );

    /**
     * getter for lp
     *
     * \return vector
     */
    WVector3D getLp() const;

    /**
     * setter for lp
     *
     * \param lp coordinate of point
     */
    void setLp( WVector3D lp );

    /**
     * getter for the rotation matrix
     *
     * \return the rotation matrix
     */
    WMatrix<double> getRotMat();

    /**
     * getter for the inverted rotation matrix
     *
     * \return the inverted rotation matrix
     */
    WMatrix<double> getInvRotMat();

    /**
     * projected interhemispherical point
     */
    WVector3D m_ihp_proj;
protected:
private:
    /**
     * helper routine to create the rotation matrix from the given points
     */
    void defineRotationMatrix();

    WMatrix<double> m_rotMat; //!< the rotation matrix
    WMatrix<double> m_rotMatInvert; //!< the inverted rotation matrix

    WVector3D m_ac; //!< anterior commisure
    WVector3D m_pc; //!< inferior commisure
    WVector3D m_ihp; //!< inter hemispherical point

    WVector3D m_ap; //!< anterior point
    WVector3D m_pp; //!< posterior point
    WVector3D m_sp; //!< superior point
    WVector3D m_ip; //!< inferior point
    WVector3D m_rp; //!< right point
    WVector3D m_lp; //!< left point
};

#endif  // WTALAIRACHCONVERTER_H
