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

#ifndef WGECOLORMAPPING_UNIFORMS_GLSL
#define WGECOLORMAPPING_UNIFORMS_GLSL

#version 120

// All the uniforms needed. These uniforms get set by the WGEColormapping class for your Node.


/////////////////////////////////////////////////////////////////////////////
// Colormap 0
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap0Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap0Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap0Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap0Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap0ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap0Colormap;

//!< True if the colormap is active.
uniform bool u_colormap0Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap0Sampler;

//!< The size in voxel dimensions
uniform int u_colormap0SizeX;
uniform int u_colormap0SizeY;
uniform int u_colormap0SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 1
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap1Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap1Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap1Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap1Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap1ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap1Colormap;

//!< True if the colormap is active.
uniform bool u_colormap1Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap1Sampler;

//!< The size in voxel dimensions
uniform int u_colormap1SizeX;
uniform int u_colormap1SizeY;
uniform int u_colormap1SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 2
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap2Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap2Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap2Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap2Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap2ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap2Colormap;

//!< True if the colormap is active.
uniform bool u_colormap2Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap2Sampler;

//!< The size in voxel dimensions
uniform int u_colormap2SizeX;
uniform int u_colormap2SizeY;
uniform int u_colormap2SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 3
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap3Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap3Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap3Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap3Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap3ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap3Colormap;

//!< True if the colormap is active.
uniform bool u_colormap3Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap3Sampler;

//!< The size in voxel dimensions
uniform int u_colormap3SizeX;
uniform int u_colormap3SizeY;
uniform int u_colormap3SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 4
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap4Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap4Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap4Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap4Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap4ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap4Colormap;

//!< True if the colormap is active.
uniform bool u_colormap4Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap4Sampler;

//!< The size in voxel dimensions
uniform int u_colormap4SizeX;
uniform int u_colormap4SizeY;
uniform int u_colormap4SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 5
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap5Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap5Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap5Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap5Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap5ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap5Colormap;

//!< True if the colormap is active.
uniform bool u_colormap5Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap5Sampler;

//!< The size in voxel dimensions
uniform int u_colormap5SizeX;
uniform int u_colormap5SizeY;
uniform int u_colormap5SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 6
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap6Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap6Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap6Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap6Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap6ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap6Colormap;

//!< True if the colormap is active.
uniform bool u_colormap6Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap6Sampler;

//!< The size in voxel dimensions
uniform int u_colormap6SizeX;
uniform int u_colormap6SizeY;
uniform int u_colormap6SizeZ;

/////////////////////////////////////////////////////////////////////////////
// Colormap 7
/////////////////////////////////////////////////////////////////////////////

//!< For unscaling the data: the minimum.
uniform float u_colormap7Min;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap7Scale;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap7Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap7Threshold;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap7ThresholdEnabled;

//!< The index of the colormap to use
uniform int u_colormap7Colormap;

//!< True if the colormap is active.
uniform bool u_colormap7Active;

//!< The sampler for texture access.
uniform sampler3D u_colormap7Sampler;

//!< The size in voxel dimensions
uniform int u_colormap7SizeX;
uniform int u_colormap7SizeY;
uniform int u_colormap7SizeZ;

#endif // WGECOLORMAPPING_UNIFORMS_GLSL

