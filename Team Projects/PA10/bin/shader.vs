attribute vec3 v_position;
attribute vec2 v_texture;
attribute vec3 v_normal;

varying vec2 f_texture;
varying vec3 f_N;
varying vec3 f_E;
varying vec3 f_L;

uniform mat4 mvpMatrix;
uniform vec4 lightPosition;

void main(void){

   gl_Position = mvpMatrix * vec4(v_position, 1.0);
   f_texture = v_texture;
  
   // Lighting
   f_N = v_normal;
   f_E = v_position;
   f_L = lightPosition.xyz;
  
  if(lightPosition.w != 0.0) {
      f_L = lightPosition.xyz - v_position;
  }
}
