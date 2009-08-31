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

#ifndef WRECORDING_H
#define WRECORDING_H

#include "WDataSet.h"

/**
 * TODO(wiebel): Document this!
 */
class WRecording : public WDataSet
{
public:
    /**
     * TODO(wiebel): Document this!
     */
    explicit WRecording( boost::shared_ptr< WMetaInfo > metaInfo )
        : WDataSet( metaInfo )
    {
    }

protected:
private:
};

#endif  // WRECORDING_H
