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

#ifndef WDATATEXTURE3D_H
#define WDATATEXTURE3D_H

#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include <osg/Texture3D>

#include "WDataHandlerEnums.h"
#include "WValueSetBase.h"
#include "WGridRegular3D.h"

class WCondition;

/**
 * Class encapsulating a 3D texture. It is able to use a value set and grid to create an OpenSceneGraph texture, that can be used
 * directly by modules. The texture values get scaled to a range of [0,1] to ensure the texture used its maximum precision. The values each get
 * scaled from [min,max] to [0,1]. The values min and max can be retrieved by getMinValue and getMaxValue. Your shader should get them as
 * uniforms to unscale the texture to have the real value.
 */
class WDataTexture3D
{
public:
    /**
     * Constructor. Creates the texture. Just run it after graphics engine was initialized.
     *
     * \param valueSet  the value set to use
     * \param grid the grid to use
     */
    explicit WDataTexture3D( boost::shared_ptr<WValueSetBase> valueSet, boost::shared_ptr<WGrid> grid );

    /**
     * Destructor.
     */
    virtual ~WDataTexture3D();

    /**
     * Gives the alpha value for this texture.
     *
     * \return the alpha value.
     */
    float getAlpha() const;

    /**
     * Sets the alpha value. The value must be in [0,1]. Otherwise nothing will happen.
     *
     * \param alpha the alpha value.
     */
    void setAlpha( float alpha );

    /**
     * Sets the opacity value. The value must be in [0,100]. Otherwise nothing will happen.
     *
     * \param opacity the opacity value.
     */
    void setOpacity( float opacity );

    /**
     * Returns the currently set threshold.
     *
     * \return the threshold.
     */
    float getThreshold() const;

    /**
     * Sets the threshold to use.
     *
     * \param threshold the threshold.
     */
    void setThreshold( float threshold );

    /**
     * Is this texture globally active and used for colormapping?
     *
     * \return true if active.
     */
    bool isGloballyActive();

    /**
     * Sets whether the texture is active globally.
     *
     * \param active true if active
     */
    void setGloballyActive( bool active = true );

    /**
     * Returns the data type of the texture.
     *
     * \return the type.
     */
    dataType getDataType();

    /**
     * getter for the texture object
     *
     * \return the texture
     */
    osg::ref_ptr< osg::Texture3D > getTexture();

    /**
     * Gets the condition which is fired whenever the texture gets some kind of dirty (threshold, opacity, ...)
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getChangeCondition();

    /**
     * The texture got created using this grid. It can help to translate coordinates into the texture coordinate space relating to
     * the grid.
     *
     * \return the grid.
     */
    boost::shared_ptr< WGridRegular3D > getGrid() const;

    /**
     * Gets the minimum value in the texture.
     *
     * \return the min.
     */
    float getMinValue();

    /**
     * Gets the maximum value in the texture.
     *
     * \return the maximum
     */
    float getMaxValue();

    /**
     * Gets the scaling factor to unscale [0,1] to [0, max-min]
     *
     * \return the scaling factor.
     */
    float getMinMaxScale();

    /**
     * getter
     */
    bool isInterpolated();

    /**
     * setter
     *
     * \param interpol
     */
    void setInterpolation( bool interpol );

    /**
     * getter
     */
    int getSelectedColormap();

    /**
     * setter
     *
     * \param cmap
     */
    void setSelectedColormap( int cmap );

protected:

    /**
     * Creates a 3d texture from a dataset. This function will be overloaded for the
     * various data types. A template function is not recommended due to the different commands
     * in the image creation.
     *
     *
     * \param source Pointer to the raw data of a dataset
     * \param components Number of values used in a Voxel, usually 1, 3 or 4
     */
    osg::ref_ptr< osg::Image > createTexture3D( unsigned char* source, int components = 1 );

    /**
     * Creates a 3d texture from a dataset. This function will be overloaded for the
     * various data types. A template function is not recommended due to the different commands
     * in the image creation.
     *
     *
     * \param source Pointer to the raw data of a dataset
     * \param components Number of values used in a Voxel, usually 1, 3 or 4
     */
    osg::ref_ptr< osg::Image > createTexture3D( int16_t* source, int components = 1 );

    /**
     * Creates a 3d texture from a dataset. This function will be overloaded for the
     * various data types. A template function is not recommended due to the different commands
     * in the image creation.
     *
     *
     * \param source Pointer to the raw data of a dataset
     * \param components Number of values used in a Voxel, usually 1, 3 or 4
     */
    osg::ref_ptr< osg::Image > createTexture3D( float* source, int components = 1 );

    /**
     * Creates a 3d texture from a dataset. This function will be overloaded for the
     * various data types. A template function is not recommended due to the different commands
     * in the image creation.
     *
     *
     * \param source Pointer to the raw data of a dataset
     * \param components Number of values used in a Voxel, usually 1, 3 or 4
     */
    osg::ref_ptr< osg::Image > createTexture3D( double* source, int components = 1 );

    /**
     * Notifies about changes. Mainly this will be used by the textures whenever the threshold/opacity change.
     */
    void notifyChange();

    /**
     * Creates a 3D texture for the data set.
     */
    void createTexture();

    /**
     * Alpha value. Used for blending in/out textures.
     */
    float m_alpha;

    /**
     * Threshold used for to exclude values.
     */
    float m_threshold;

    /**
     * The actual texture.
     */
    osg::ref_ptr< osg::Texture3D > m_texture;

    /**
     * The value set from which the texture gets created.
     */
    boost::shared_ptr<WValueSetBase> m_valueSet;

    /**
     * The grid used to set up the texture.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * The condition which is fired whenever the dataset gets some kind of dirty (threshold, opacity, ...)
     */
    boost::shared_ptr< WCondition > m_changeCondition;

    /**
     * Flag denotes whether this texture should be used by surfaces/slides for surface colormapping.
     */
    bool m_globalActive;

    /**
     * flag denotes whether the texture filter should active or not
     */
    bool m_interpolation;

    /**
     * indicates which colormap to use
     */
    int m_cmap;

    /**
     * This method finds the minimum and maximum value of a dataset. These values get used to scale the texture to use the maximum precision.
     *
     * \param source the data
     * \param components the number of components
     */
    virtual void findMinMax( float* source, int components );

    /**
     * This method finds the minimum and maximum value of a dataset. These values get used to scale the texture to use the maximum precision.
     *
     * \param source the data
     * \param components the number of components
     */
    virtual void findMinMax( double* source, int components );

    /**
     * This method finds the minimum and maximum value of a dataset. These values get used to scale the texture to use the maximum precision.
     *
     * \param source the data
     * \param components the number of components
     */
    virtual void findMinMax( unsigned char* source, int components );

    /**
     * This method finds the minimum and maximum value of a dataset. These values get used to scale the texture to use the maximum precision.
     *
     * \param source the data
     * \param components the number of components
     */
    virtual void findMinMax( int16_t* source, int components );

    /**
     * The smallest value inside the dataset
     */
    float m_minValue;

    /**
     * The largest value inside the dataset
     */
    float m_maxValue;

    /**
     * The scaling factor to apply to unscale a [0,1] to [0,max-min]
     */
    float m_scale;

    /**
     * Scales the specified value to the interval [0,1] using m_maxValue and m_minValue.
     *
     * \param value the value to scale
     *
     * \return the value scaled to [0,1]
     */
    float scaleInterval( float value ) const;

private:
};

#endif  // WDATATEXTURE3D_H

