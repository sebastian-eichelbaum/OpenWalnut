/** 
 * Function to calculate lighting based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 *
 * @param ambient   materials ambient color
 * @param diffuse   materials diffuse color
 * @param specular  materials specular color
 * @param shininess material shininess
 * @param lightColor  the light color
 * @param ambientLight the ambient light color
 * @param normalDir the normal
 * @param viewDir   viewing direction
 * @param lightDir  light direction
 * 
 * @return the color.
 */
vec4 blinnPhongIllumination( vec3 ambient, vec3 diffuse, vec3 specular, float shininess, 
							 vec3 lightColor, vec3 ambientLight, 
							 vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
  vec3 H =  normalize( lightDir + viewDir );
  
  // compute ambient term
  vec3 ambientV = ambient * ambientLight;

  // compute diffuse term
  float diffuseLight = max(dot(lightDir, normalDir), 0.);
  vec3 diffuseV = diffuse * diffuseLight;

  // compute specular term
  float specularLight = pow(max(dot(H, normalDir), 0.), shininess);
  if( diffuseLight <= 0.) specularLight = 0.;
  vec3 specularV = specular * specularLight;

  return vec4(ambientV + (diffuseV + specularV)*lightColor, 1.);
}

/** 
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need the intensity.
 *
 * @param ambient   materials ambient intensity
 * @param diffuse   materials diffuse intensity
 * @param specular  materials specular intensity
 * @param shininess material shininess
 * @param lightIntensity  the light intensity
 * @param ambientIntensity the ambient light intensity
 * @param normalDir the normal
 * @param viewDir   viewing direction
 * @param lightDir  light direction
 * 
 * @return the light intensity.
 */
float blinnPhongIlluminationIntensity(  float ambient, float diffuse, float specular, float shininess, 
                                        float lightIntensity, float ambientIntensity, 
                                        vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
  vec3 H =  normalize( lightDir + viewDir );
  
  // compute ambient term
  float ambientV = ambient * ambientIntensity;

  // compute diffuse term
  float diffuseLight = max(dot(lightDir, normalDir), 0.);
  float diffuseV = diffuse * diffuseLight;

  // compute specular term
  float specularLight = pow(max(dot(H, normalDir), 0.), shininess);
  if( diffuseLight <= 0.) specularLight = 0.;
  float specularV = specular * specularLight;

  return ambientV + (diffuseV + specularV)*lightIntensity;
}

