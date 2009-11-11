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

#include "../../math/WVector3D.h"
#include "../../math/WMatrix.hpp"

/**
 * class to provide conversions between world or voxel space coordinates und talairach coordinates
 */
class WTalairachConverter
{
public:
    /**
     * constructor
     */
    WTalairachConverter( wmath::WVector3D ac, wmath::WVector3D pc, wmath::WVector3D ihp );

    /**
     * destructor
     */
    virtual ~WTalairachConverter();

    wmath::WVector3D Canonical2ACPC( const wmath::WVector3D point );
    wmath::WVector3D ACPC2Canonical( const wmath::WVector3D point );

    wmath::WVector3D Canonical2Talairach( const wmath::WVector3D point );
    wmath::WVector3D Talairach2Canonical( const wmath::WVector3D point );

    wmath::WVector3D ACPC2Talairach( const wmath::WVector3D point );
    wmath::WVector3D Talairach2ACPC( const wmath::WVector3D point );

    wmath::WVector3D getAc() const;
    void setAc(wmath::WVector3D ac);

    wmath::WVector3D getPc() const;
    void setPc(wmath::WVector3D pc);

    wmath::WVector3D getIhp() const;
    void setIhp(wmath::WVector3D ihp);

    wmath::WVector3D getAp() const;
    void setAp(wmath::WVector3D ap);

    wmath::WVector3D getPp() const;
    void setPp(wmath::WVector3D pp);

    wmath::WVector3D getSp() const;
    void setSp(wmath::WVector3D sp);

    wmath::WVector3D getIp() const;
    void setIp(wmath::WVector3D ip);

    wmath::WVector3D getRp() const;
    void setRp(wmath::WVector3D rp);

    wmath::WVector3D getLp() const;
    void setLp(wmath::WVector3D lp);

protected:
private:
    void defineRotationMatrix();

    wmath::WMatrix<double> m_rotMat;

    wmath::WVector3D m_ac;
    wmath::WVector3D m_pc;
    wmath::WVector3D m_ihp;

    wmath::WVector3D m_ap;
    wmath::WVector3D m_pp;
    wmath::WVector3D m_sp;
    wmath::WVector3D m_ip;
    wmath::WVector3D m_rp;
    wmath::WVector3D m_lp;
};

#endif  // WTALAIRACHCONVERTER_H
