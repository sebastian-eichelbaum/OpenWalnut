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

#include "WDataSetHistogram1D.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetHistogram1D::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetHistogram1D::WDataSetHistogram1D( boost::shared_ptr< WHistogramBasic const > const& histo )
    : WDataSet(),
      m_histogram( new WHistogramBasic( *histo ) )
{
}

WDataSetHistogram1D::WDataSetHistogram1D()
    : WDataSet(),
      m_histogram()
{
}

WDataSetHistogram1D::~WDataSetHistogram1D()
{
}

std::string const WDataSetHistogram1D::getName() const
{
    return "WDataSetHistogram1D";
}

std::string const WDataSetHistogram1D::getDescription() const
{
    return "A dataset that contains some 1D histogram.";
}

boost::shared_ptr< WPrototyped > WDataSetHistogram1D::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetHistogram1D() );
    }

    return m_prototype;
}

boost::shared_ptr< WHistogramBasic const > const& WDataSetHistogram1D::getHistogram() const
{
    return m_histogram;
}

