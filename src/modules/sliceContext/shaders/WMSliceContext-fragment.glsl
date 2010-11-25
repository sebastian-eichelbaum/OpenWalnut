varying vec4 myColor;

/*
 * simple fragment shader that uses a texture on fibers
 */
void onlyNearFibers()
{
    float distance = gl_TexCoord[0].x;
    // float maxDist = gl_TexCoord[0].y;
    // float normalizedDistance = distance / maxDist;
    // normalizedDistance = clamp ( normalizedDistance, 0.0, 1.0 );
    vec4 color = myColor;
    color.a = .1;
    if( distance > 1 )
        color.a = 0;
    gl_FragColor = color;
}


void main()
{
    vec4 color = myColor;
    float distance = gl_TexCoord[0].x;
    float maxDist = gl_TexCoord[0].y;
    distance = clamp( distance, 0, maxDist);
    float alpha = 1 - ( distance / maxDist);
    alpha *= alpha;
    alpha *= alpha;
    alpha *= alpha;
    alpha *= alpha;
    alpha *= alpha;
    float darkness = clamp( alpha *.3, 0.0, 1.0);
    //color = vec4( darkness, clamp( .5 - darkness, 0.0, 1.0), 1.0, .01 + alpha );
    color.a = .01 + alpha;

    gl_FragColor = color;

//    onlyNearFibers();
}
