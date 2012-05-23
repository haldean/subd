/* vim: set filetype=c : */
varying vec3 N;
varying vec3 v;

#define MAX_LIGHTS 3 
void main (void) { 
  vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);

  for (int i=0;i<MAX_LIGHTS;i++) {
    vec3 L = normalize(gl_LightSource[i].position.xyz - v); 
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) 
    vec3 R = normalize(-reflect(L,N)); 

    //calculate Ambient Term: 
    vec4 Iamb = gl_FrontLightProduct[i].ambient; 
    //calculate Diffuse Term: 
    vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0); 

    // calculate Specular Term:
    vec4 Ispec = gl_FrontLightProduct[i].specular 
      * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 

    finalColor += Iamb + Idiff + Ispec;
  }

  finalColor += gl_FrontLightModelProduct.sceneColor;

  // calculate apparent brightness of color
  float bright = (finalColor[0] + finalColor[1] + finalColor[2]) / 3.0;

  float alpha = finalColor[3];
  if (dot(N,normalize(v)) > -0.35 || bright < 1e-5)
    gl_FragColor = vec4(0., 0., 0., alpha);
  else if (bright < 0.2)
    gl_FragColor = vec4(.4, .2, 0., alpha);
  else if (bright < 0.5)
    gl_FragColor = vec4(.8, .4, 0., alpha);
  else if (bright < 0.7)
    gl_FragColor = vec4(1., .6, 0., alpha);
  else if (bright < 0.9)
    gl_FragColor = vec4(1., .8, 0., alpha);
  else
    gl_FragColor = vec4(1., 1., 0., alpha);
}
