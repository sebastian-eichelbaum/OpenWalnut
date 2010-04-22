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

#include <string>

#include "../common/WAssert.h"
#include "../common/WCondition.h"
#include "../common/WTransferable.h"
#include "exceptions/WDHException.h"
#include "WDataSet.h"
#include "WDataSetVector.h"
#include "WDataTexture3D.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSet::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSet::WDataSet()
    : WTransferable(),
    m_fileName( "" )
{
}

void WDataSet::setFileName( const std::string fileName )
{
    WAssert( fileName != "", "No filename set for data set." );
    m_fileName = fileName;
}

std::string WDataSet::getFileName() const
{
    return m_fileName;
}

bool WDataSet::isTexture() const
{
    return false;
}

boost::shared_ptr< WDataTexture3D > WDataSet::getTexture()
{
    throw WDHException( "This dataset does not provide a texture." );
}

const std::string WDataSet::getName() const
{
    return "WDataSet";
}

const std::string WDataSet::getDescription() const
{
    return "Encapsulates the whole common feature set of all datasets.";
}

boost::shared_ptr< WPrototyped > WDataSet::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSet() );
    }

    return m_prototype;
}

boost::shared_ptr< WCondition > WDataSet::getChangeCondition()
{
    // this just forwards to the texture condition. In the future maybe datasets may also change so we need an separate condition in every
    // dataset.
    if ( isTexture() )
    {
        return getTexture()->getChangeCondition();
    }

    return boost::shared_ptr< WCondition >();
}

boost::shared_ptr< WDataSetVector > WDataSet::isVectorDataSet()
{
    return boost::shared_ptr< WDataSetVector >();
}
