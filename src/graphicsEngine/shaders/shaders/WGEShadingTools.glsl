//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/** 
 * Function to calculate lighting based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 *
 * \param ambient   materials ambient color
 * \param diffuse   materials diffuse color
 * \param specular  materials specular color
 * \param shininess material shininess
 * \param lightColor  the light color
 * \param ambientLight the ambient light color
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 * 
 * \return the color.
 */
vec4 blinnPhongIllumination( vec3 ambient, vec3 diffuse, vec3 specular, float shininess,
                             vec3 lightColor, vec3 ambientLight,
                             vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
  vec3 H =  normalize( lightDir + viewDir );

  // compute ambient term
  vec3 ambientV = ambient * ambientLight;

  // compute diffuse term
  float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
  vec3 diffuseV = diffuse * diffuseLight;

  // compute specular term
  float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
  if( diffuseLight <= 0.) specularLight = 0.;
  vec3 specularV = specular * specularLight;

  return vec4( ambientV + ( diffuseV + specularV ) * lightColor, 1.0 );
}

/**
 * Function to calculate lighting intensitybased on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param ambient   materials ambient intensity
 * \param diffuse   materials diffuse intensity
 * \param specular  materials specular intensity
 * \param shininess material shininess
 * \param lightIntensity  the light intensity
 * \param ambientIntensity the ambient light intensity
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the light intensity.
 */
float blinnPhongIlluminationIntensity( float ambient, float diffuse, float specular, float shininess,
                                       float lightIntensity, float ambientIntensity,
                                       vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
  vec3 H =  normalize( lightDir + viewDir );

  // compute ambient term
  float ambientV = ambient * ambientIntensity;

  // compute diffuse term
  float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
  float diffuseV = diffuse * diffuseLight;

  // compute specular term
  float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
  if( diffuseLight <= 0.) specularLight = 0.;
  float specularV = specular * specularLight;

  return ambientV + ( diffuseV + specularV ) * lightIntensity;
}

