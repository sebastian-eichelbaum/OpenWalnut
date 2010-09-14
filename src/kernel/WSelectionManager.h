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

#ifndef WSELECTIONMANAGER_H
#define WSELECTIONMANAGER_H

#include <boost/shared_ptr.hpp>

#include "WCrosshair.h"

#include "WExportKernel.h"

typedef enum
{
    PAINTMODE_NONE = 0,
    PAINTMODE_PAINT = 1
}
WPaintMode;

/**
 * manages the several selection tools
 */
class OWKERNEL_EXPORT WSelectionManager // NOLINT
{
public:
    /**
     * standard constructor
     */
    WSelectionManager();

    /**
     * destructor
     */
    virtual ~WSelectionManager();

    /**
     * getter
     */
    boost::shared_ptr< WCrosshair >getCrosshair();

    /**
     * function returns an index of the direction one is currently looking at the scene
     *
     * \return index
     */
    int getFrontSector();

    /**
     * setter for paint mode, also forwards it to the graphics engine
     *
     * \param mode
     */
    void setPaintMode( WPaintMode mode );

    /**
     * getter for paint mode
     *
     * \return the mode
     */
    WPaintMode getPaintMode();

    /**
     * setter for texture and grid
     *
     * \param texture
     * \param grid
     */
    void setTexture( osg::ref_ptr< osg::Texture3D > texture, boost::shared_ptr< WGridRegular3D >grid );

    /**
     * getter
     * \return texture
     */
    osg::ref_ptr< osg::Texture3D > getTexture();

    /**
     * getter
     * \return grid
     */
    boost::shared_ptr< WGridRegular3D >getGrid();

    /**
     * setter
     * \param flag
     */
    void setUseTexture( bool flag = true );

    /**
     * getter
     * \return flag
     */
    bool getUseTexture();

    /**
     * getter
     * \return the opacity
     */
    float getTextureOpacity();

    /**
     * setter
     * \param value the new opacity to use with the texture
     */
    void setTextureOpacity( float value );

protected:
private:
    boost::shared_ptr< WCrosshair >m_crosshair; //!< stores pointer to crosshair

    WPaintMode m_paintMode; //!< stores the currently selected paint mode

    /**
     * stores a pointer to a texture 3d, this is used to provide a faster texture generation process
     * than creating a new dataset for every texture change
     */
    osg::ref_ptr< osg::Texture3D > m_texture;

    /**
     * stores a pointer to the grid to be used together with the texture
     */
    boost::shared_ptr< WGridRegular3D >m_textureGrid;

    /**
     * the texture opacity
     */
    float m_textureOpacity;

    /**
     * flag indicating if this additional texture should be used.
     */
    bool m_useTexture;
};

#endif  // WSELECTIONMANAGER_H
