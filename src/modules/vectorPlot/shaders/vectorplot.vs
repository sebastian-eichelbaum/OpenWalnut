varying vec4 _color;

void main()
{
    _color = gl_Color;
    gl_Position = ftransform();
}
