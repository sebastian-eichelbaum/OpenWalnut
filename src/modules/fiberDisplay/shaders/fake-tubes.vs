varying float tangent_dot_view;
varying vec3 tangentR3;
varying float s_param;
varying vec4 myColor;
uniform float u_thickness;
varying vec4 VaryingTexCoord0;

void main()
{
    VaryingTexCoord0 = gl_Vertex;
    
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; //< make clipping planes work

	vec3 tangent;

	float thickness = 0.01 * u_thickness/ 100.;

	tangentR3 = gl_Normal;
	tangent = (gl_ModelViewProjectionMatrix * vec4(gl_Normal,0.)).xyz; //< transform our tangent vector
	s_param = gl_MultiTexCoord0.x; //< store texture coordinate for shader

	vec3 offsetNN = cross( normalize(tangent.xyz), vec3(0.,0.,-1.));
	vec3 offset = normalize(offsetNN);
	tangent_dot_view = length(offsetNN);

	vec4 pos = ftransform(); //< transform position to eye space

	offset.x *= thickness;
	offset.y *= thickness;

	pos.xyz += offset * (s_param); //< add offset in y-direction (eye-space)

	myColor = gl_Color;

	gl_Position = pos; //< store final position
}
