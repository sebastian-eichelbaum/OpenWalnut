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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>

#include "WGEScene.h"

WGEScene::WGEScene():
    osg::Group()
{
    std::cout << "Initializing OpenSceneGraph Root Node" << std::endl;

    // initialize members
}

WGEScene::~WGEScene()
{
    std::cout << "Shutting down OpenSceneGraph Root Node" << std::endl;

    // cleanup
}


