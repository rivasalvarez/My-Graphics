varying vec2 f_texture;
varying vec3 f_N;
varying vec3 f_E;
varying vec3 f_L;

uniform sampler2D gSampler;

uniform vec4 ambientProduct;
uniform vec4 diffuseProduct;
uniform vec4 specularProduct;
uniform vec4 lightPosition;
uniform float shininess;

void main(void){
   // Normalize the input lighting vectors
   vec3 N = normalize(f_N);
   vec3 E = normalize(f_E);
   vec3 L = normalize(f_L);

   // Ambient
   vec3 H = normalize(L + E);   
   vec4 ambient = ambientProduct;

   // Diffuse
   float Kd = max(dot(L, N), 0.0);
   vec4 diffuse = Kd * diffuseProduct;
    
   // Specular
   float Ks = pow(max(dot(N, H), 0.0), shininess);
   vec4 specular = Ks * specularProduct;

   // Discard the specular highlight if the light's behind the vertex
   if(dot(L, N) < 0.0) {
  	specular = vec4(0.0, 0.0, 0.0, 1.0);
   }
    
   gl_FragColor = ambient + diffuse + specular;
   gl_FragColor.a = 1.0;
   
   // Texture
   gl_FragColor *= texture2D(gSampler, f_texture.xy);
}
