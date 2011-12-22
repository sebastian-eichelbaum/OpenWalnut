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

#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/WProperties.h"
#include "WCrosshair.h"

#include "WExportKernel.h"

class WGridRegular3D;

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
     * Return the current position of the point selection
     *
     * \return the crosshair
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

    /**
     * The property controlling the current axial position of slices.
     *
     * \return property as double
     */
    WPropDouble getPropAxialPos();

    /**
     * The property controlling the current coronal position of slices.
     *
     * \return property as double
     */
    WPropDouble getPropCoronalPos();

    /**
     * The property controlling the current sagittal position of slices.
     *
     * \return property as double
     */
    WPropDouble getPropSagittalPos();


    /**
     * The property controlling the current axial visible-flag.
     *
     * \return property as bool
     */
    WPropBool getPropAxialShow();

    /**
     * The property controlling the current coronal visible-flag.
     *
     * \return property as bool
     */
    WPropBool getPropCoronalShow();

    /**
     * The property controlling the current sagittal visible-flag.
     *
     * \return property as bool
     */
    WPropBool getPropSagittalShow();

    /**
     * setter for the shader index to be used with the custom texture
     * \param shader the index of the shader
     */
    void setShader( int shader );

    /**
     * getter
     * \return shader index
     */
    int getShader();

protected:
private:
    /**
     * Updates the crosshair position
     */
    void updateCrosshairPosition();

    boost::shared_ptr< WCrosshair >m_crosshair; //!< stores pointer to crosshair

    /**
     * Contains the slice related properties
     */
    WProperties::SPtr m_sliceGroup;

    /**
     * Axial slice position.
     */
    WPropDouble m_axialPos;

    /**
     * Coronal slice position.
     */
    WPropDouble m_coronalPos;

    /**
     * Sagittal slice position.
     */
    WPropDouble m_sagittalPos;

    /**
     * Axial visible-flag.
     */
    WPropBool m_axialShow;

    /**
     * Coronal visible-flag.
     */
    WPropBool m_coronalShow;

    /**
     * Sagittal visible-flag.
     */
    WPropBool m_sagittalShow;

    /**
     * The connection for the axial property
     */
    boost::signals2::connection m_axialUpdateConnection;

    /**
     * The connection for the coronal property
     */
    boost::signals2::connection m_coronalUpdateConnection;

    /**
     * The connection for the sagittal property
     */
    boost::signals2::connection m_sagittalUpdateConnection;

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

    /**
     * index of the shader to use with the texture
     */
    int m_shader;
};

#endif  // WSELECTIONMANAGER_H
