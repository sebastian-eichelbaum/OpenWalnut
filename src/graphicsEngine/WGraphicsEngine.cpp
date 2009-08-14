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

#include <iostream>

#include "exceptions/WGEInitFailed.h"
#include "../kernel/WKernel.h"
#include "WGraphicsEngine.h"

WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    std::cout << "Initializing Graphics Engine" << std::endl;

    // initialize members
    m_RootNode = new WGEScene();
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    std::cout << "Shutting down Graphics Engine" << std::endl;
    delete m_RootNode;
}

WGraphicsEngine::WGraphicsEngine( const WGraphicsEngine& other )
{
    *this = other;
}

WGEScene* WGraphicsEngine::getScene()
{
    return m_RootNode;
}

void WGraphicsEngine::threadMain()
{
    while( !m_FinishRequested )
    {
        // currently a dummy
        // TODO(ebaum): add drawing and event handling here
        sleep( 1 );
    }
}

