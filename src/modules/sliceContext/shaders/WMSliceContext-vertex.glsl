varying vec4 myColor;

void main()
{
    myColor = gl_Color;

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    gl_Position = ftransform(); //< store final position
}
