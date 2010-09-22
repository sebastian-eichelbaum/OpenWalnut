varying vec4 myColor;

/*
 * simple fragment shader that uses a texture on fibers
 */
void main()
{
    vec4 color = myColor;
    float alpha = 1 - ( gl_TexCoord[0].x / 100. );
    alpha = clamp( pow( alpha, 10. ) + 0.1, 0.0, 1.0);
    color = vec4( 0.0, 0.0, 1.0, alpha );

    gl_FragColor = color;
}

