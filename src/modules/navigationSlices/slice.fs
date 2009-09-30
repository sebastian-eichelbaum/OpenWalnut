varying vec4 VaryingTexCoord0;

uniform sampler3D tex0;

void main()
{
    vec4 color = texture3D(tex0, VaryingTexCoord0);
    if ( ( color.r + color.g + color.b ) < 0.01 )
        discard;
    gl_FragColor = color;
}
