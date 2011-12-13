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

#ifndef WTRANSFERABLE_H
#define WTRANSFERABLE_H

#include "../common/WPrototyped.h"
#include "WExportCommon.h"

/**
 * Class building the interface for classes that might be transferred using WModuleConnector.
 * \see WModuleConnector. All transferable objects need a prototype.
 *
 * \ingroup Kernel
 */
class OWCOMMON_EXPORT WTransferable: public WPrototyped
{
public:
    /**
     * Default constructor.
     */
    WTransferable();

    /**
     * Destructor.
     */
    virtual ~WTransferable();

protected:
private:
};

#endif  // WTRANSFERABLE_H

