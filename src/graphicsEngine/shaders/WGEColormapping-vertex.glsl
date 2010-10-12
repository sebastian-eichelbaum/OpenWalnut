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

#include "WGEColormapping-uniforms.glsl"
#include "WGEColormapping-varyings.glsl"

/**
 * This method prepares some needed internal variables. Please call this in your vertex shader.
 * Be aware that this only works with the WGEColormapping class.
 */
void colormapping()
{
#ifdef Colormap0Enabled
    v_colormap0TexCoord = ( gl_TextureMatrix[ Colormap0Unit ] * gl_MultiTexCoord0 ).xyz;
#endif
#ifdef Colormap1Enabled
    v_colormap1TexCoord = ( gl_TextureMatrix[ Colormap1Unit ] * gl_MultiTexCoord0 ).xyz;
#endif
#ifdef Colormap2Enabled
    v_colormap2TexCoord = ( gl_TextureMatrix[ Colormap2Unit ] * gl_MultiTexCoord0 ).xyz;
#endif
#ifdef Colormap3Enabled
    v_colormap3TexCoord = ( gl_TextureMatrix[ Colormap3Unit ] * gl_MultiTexCoord0 ).xyz; 
#endif
#ifdef Colormap4Enabled
    v_colormap4TexCoord = ( gl_TextureMatrix[ Colormap4Unit ] * gl_MultiTexCoord0 ).xyz;
#endif
#ifdef Colormap5Enabled
    v_colormap5TexCoord = ( gl_TextureMatrix[ Colormap5Unit ] * gl_MultiTexCoord0 ).xyz; 
#endif
#ifdef Colormap6Enabled
    v_colormap6TexCoord = ( gl_TextureMatrix[ Colormap6Unit ] * gl_MultiTexCoord0 ).xyz; 
#endif
#ifdef Colormap7Enabled
    v_colormap7TexCoord = ( gl_TextureMatrix[ Colormap7Unit ] * gl_MultiTexCoord0 ).xyz; 
#endif
}

