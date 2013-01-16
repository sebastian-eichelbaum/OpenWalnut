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
uniform float u_colormap0Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap0Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap0Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap0ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap0ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap0ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap0WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap0Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap0Colormap;

//!< True if the colormap is active.
uniform bool u_colormap0Active = false;

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
uniform float u_colormap1Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap1Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap1Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap1ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap1ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap1ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap1WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap1Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap1Colormap;

//!< True if the colormap is active.
uniform bool u_colormap1Active = false;

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
uniform float u_colormap2Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap2Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap2Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap2ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap2ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap2ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap2WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap2Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap2Colormap;

//!< True if the colormap is active.
uniform bool u_colormap2Active = false;

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
uniform float u_colormap3Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap3Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap3Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap3ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap3ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap3ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap3WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap3Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap3Colormap;

//!< True if the colormap is active.
uniform bool u_colormap3Active = false;

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
uniform float u_colormap4Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap4Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap4Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap4ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap4ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap4ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap4WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap4Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap4Colormap;

//!< True if the colormap is active.
uniform bool u_colormap4Active = false;

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
uniform float u_colormap5Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap5Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap5Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap5ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap5ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap5ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap5WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap5Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap5Colormap;

//!< True if the colormap is active.
uniform bool u_colormap5Active = false;

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
uniform float u_colormap6Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap6Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap6Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap6ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap6ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap6ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap6WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap6Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap6Colormap;

//!< True if the colormap is active.
uniform bool u_colormap6Active = false;

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
uniform float u_colormap7Min = 0;

//!< For unscaling the data: the scaling factor.
uniform float u_colormap7Scale = 1;

//!< The alpha value for this colormap. Is in [0,1].
uniform float u_colormap7Alpha;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap7ThresholdLower;

//!< The threshold value for this colormap. Is in [Min,Scale+Min]
uniform float u_colormap7ThresholdUpper;

//!< Flag denoting whether to use the threshold value for clipping or not.
uniform bool u_colormap7ThresholdEnabled = false;

//!< Flag denoting whether to use the windowing values for scaling or not.
uniform bool u_colormap7WindowEnabled = false;

//!< Contains the lower and upper window level.
uniform vec2 u_colormap7Window = vec2( 0.0, 1.0 );

//!< The index of the colormap to use
uniform int u_colormap7Colormap;

//!< True if the colormap is active.
uniform bool u_colormap7Active = false;

//!< The sampler for texture access.
uniform sampler3D u_colormap7Sampler;

//!< The size in voxel dimensions
uniform int u_colormap7SizeX;
uniform int u_colormap7SizeY;
uniform int u_colormap7SizeZ;

#endif // WGECOLORMAPPING_UNIFORMS_GLSL

