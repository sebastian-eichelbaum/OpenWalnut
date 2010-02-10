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
#include <vector>

#include "WDataHandler.h"
#include "WSubject.h"
#include "exceptions/WDHNoSuchDataSet.h"

WDataHandler::WDataHandler()
{
}

void WDataHandler::addSubject( boost::shared_ptr< WSubject > newSubject )
{
    // add the subject to the list ( ensure mutual exclusive write )
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_subjectsLock );
    //m_subjects.push_back( newSubject );
    lock.unlock();
}

