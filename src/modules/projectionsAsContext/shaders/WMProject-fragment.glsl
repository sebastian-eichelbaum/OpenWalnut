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

#version 130

#include "WGEColormapping-fragment.glsl"

// If this uniform is true, the plane should not discard fragments
uniform bool u_showComplete;
uniform vec3 maxCoords; // contains the size of the 3d texture
uniform sampler1D u_transferFunctionSampler; // contains the transfer function histogram

/*
  Values are the same as in the enum Type PlaneType
  in WMProject.h e. g. 
  0: None
  1: MIP
  2: Compositing F2B
  3: Compositing B2F
*/
uniform int axialTop;
uniform int axialBottom;
uniform int coronalTop;
uniform int coronalBottom;
uniform int sagittalTop;
uniform int sagittalBottom;

uniform float earlyRayTerminationAlpha;

varying vec3 normal;

void doMIP(vec3 initialPosition, int direction, float maxSize)
{
	float maxColor = 0.0;
	for(float n = 0.0; n < 1.0; n += 1.0/maxSize)
	{
		initialPosition[direction] = n;
		if(texture3D( u_colormap0Sampler, initialPosition ).r > maxColor)
			maxColor = texture3D( u_colormap0Sampler, initialPosition ).r;
	}
	gl_FragColor = vec4(maxColor,maxColor,maxColor,1.0);
}

void doCompositingF2B(vec3 initialPosition, int direction, float maxSize)
{
	vec3 finalColor = texture1D( u_transferFunctionSampler, texture3D( u_colormap0Sampler, initialPosition ).r ).rgb;
	float finalAlpha = texture1D( u_transferFunctionSampler, texture3D( u_colormap0Sampler, initialPosition ).r ).a;
  
	bool front = initialPosition[direction] == 0.0;
	float n;
	bool run = true;
	if(front)
	{
		n = 1.0/maxSize;
	}
	else
	{
		n = 1.0 - 1.0/maxSize; // invert
	}
  
	while(run)
	{
		initialPosition[direction] = n;
		vec4 nextPlane = texture1D( u_transferFunctionSampler, texture3D( u_colormap0Sampler, initialPosition ).r );
		nextPlane = vec4(nextPlane.rgb*nextPlane.a,nextPlane.a);
		
		// C_out = (1 - a_in)*C + C_in
		finalColor = (1-finalAlpha)*nextPlane.rgb+finalColor;
		
		// a_out = (1 - a_in)*a + a_in
		finalAlpha = (1-finalAlpha)*nextPlane.a+finalAlpha;
		
		if(finalAlpha > earlyRayTerminationAlpha)
		{
			run = false; // Early Ray Termination
		}else{
			if(front)
			{
				n += 1.0/maxSize;
				if(n > 1.0)
				{
					run = false;
				}
			}
			else
			{
				n -= 1.0/maxSize;
				if(n < 0.0)
				{
					run = false;
				}
			}
		}
	}
	gl_FragColor = vec4(finalColor, 1.0);
}

void doCompositingB2F(vec3 initialPosition, int direction, float maxSize)
{
	bool front = initialPosition[direction] == 0.0;
	float n;
	bool run = true;
	vec3 finalColor = vec3(0.0);
	if(front)
	{
		initialPosition[direction] = 1.0;
		n = 1.0 - 1.0/maxSize;
	}
	else
	{
		initialPosition[direction] = 0.0;
		n = 1.0/maxSize;
	}
  
	finalColor = texture1D( u_transferFunctionSampler, texture3D( u_colormap0Sampler, initialPosition ).r ).rgb;
  
	while(run)
	{
		initialPosition[direction] = n;
		vec4 nextPlane = texture1D( u_transferFunctionSampler, texture3D( u_colormap0Sampler, initialPosition ).r );
    
		// C_out = (1 - a)*C_in + C*a; with C_in = C_out in the next step
		finalColor = nextPlane.rgb * nextPlane.a + finalColor * (1 - nextPlane.a);
    
		if(!front)
		{
			n += 1.0/maxSize;
			if(n > 1.0)
			{
				run = false;
			}
		}
		else
		{
			n -= 1.0/maxSize;
			if(n < 0.0)
			{
				run = false;
			}
		}
	}
	gl_FragColor = vec4(finalColor, 1.0);
}

/**
Check which algorithm has to be used to the corresponding plane (see comment for the uniforms @top)
Knowing the normal, we'll verify which fragment belongs to which plane
this can be achieved by examination of the expected component of the normal vector 
(e. g. for axial planes we've got normals (0,1,0) or (0,-1,0), so there is a direction along the ordinate)
*/
void main()
{
	// Check for MIP
	if(coronalTop == 1 && normal.z == 1 || coronalBottom == 1 && normal.z == -1)
		doMIP(vec3(v_colormap0TexCoord.xy,0.0),2,maxCoords.z);
   
	else if(axialTop == 1 && normal.y == 1 || axialBottom == 1 && normal.y == -1)
		doMIP(vec3(v_colormap0TexCoord.x,0.0,v_colormap0TexCoord.z),1,maxCoords.y);

	else if(sagittalTop == 1 && normal.x == 1 || sagittalBottom == 1 && normal.x == -1)
		doMIP(vec3(0.0,v_colormap0TexCoord.yz),0,maxCoords.x);
      
	// Check for Compositing F2B
	else if(coronalTop == 2 && normal.z == 1)
		doCompositingF2B(vec3(v_colormap0TexCoord.xy,0.0),2,maxCoords.z);
	else if(coronalBottom == 2 && normal.z == -1)
		doCompositingF2B(vec3(v_colormap0TexCoord.xy,maxCoords.z),2,maxCoords.z);      
      
	else if(axialTop == 2 && normal.y == 1)
		doCompositingF2B(vec3(v_colormap0TexCoord.x,0.0,v_colormap0TexCoord.z),1,maxCoords.y);
	else if(axialBottom == 2 && normal.y == -1)
		doCompositingF2B(vec3(v_colormap0TexCoord.x,maxCoords.y,v_colormap0TexCoord.z),1,maxCoords.y);      

	else if(sagittalTop == 2 && normal.x == 1)
		doCompositingF2B(vec3(0.0,v_colormap0TexCoord.yz),0,maxCoords.x);
	else if(sagittalBottom == 2 && normal.x == -1)
		doCompositingF2B(vec3(maxCoords.x,v_colormap0TexCoord.yz),0,maxCoords.x);
        
	// Check for Compositing B2F
	else if(coronalTop == 3 && normal.z == 1)
		doCompositingB2F(vec3(v_colormap0TexCoord.xy,0.0),2,maxCoords.z);
	else if(coronalBottom == 3 && normal.z == -1)
		doCompositingB2F(vec3(v_colormap0TexCoord.xy,maxCoords.z),2,maxCoords.z);      
      
	else if(axialTop == 3 && normal.y == 1)
		doCompositingB2F(vec3(v_colormap0TexCoord.x,0.0,v_colormap0TexCoord.z),1,maxCoords.y);
	else if(axialBottom == 3 && normal.y == -1)
		doCompositingB2F(vec3(v_colormap0TexCoord.x,maxCoords.y,v_colormap0TexCoord.z),1,maxCoords.y);      

	else if(sagittalTop == 3 && normal.x == 1)
		doCompositingB2F(vec3(0.0,v_colormap0TexCoord.yz),0,maxCoords.x);
	else if(sagittalBottom == 3 && normal.x == -1)
		doCompositingB2F(vec3(maxCoords.x,v_colormap0TexCoord.yz),0,maxCoords.x);  
}

