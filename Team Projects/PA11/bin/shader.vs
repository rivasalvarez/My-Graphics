const int numLights = 5;

attribute vec3 v_position;
attribute vec2 v_texture;
attribute vec3 v_normal;

varying vec2 f_texture;
varying vec3 f_N;
varying vec3 f_E;
varying vec3 f_L[numLights];

uniform vec4 lightPosition[numLights];
uniform mat4 mvpMatrix;


void main(void){
   // Vertex position and texture
   gl_Position = mvpMatrix * vec4(v_position, 1.0);
   f_texture = v_texture;
   
   // Normal and vertex position
   f_N = v_normal;
   f_E = v_position;
   
   // Light position for each light source
   for(int i=0; i<numLights; i++) {
      f_L[i] = lightPosition[i].xyz;
     
      // Directional light if w == 0, else spotlight
      if(lightPosition[i].w != 0.0) {
         f_L[i] = lightPosition[i].xyz - v_position;
      }
   }
}
