const int numLights = 5;

varying vec2 f_texture;
varying vec3 f_N;
varying vec3 f_E;
varying vec3 f_L[numLights];

uniform sampler2D gSampler;

uniform vec4 ambientProduct;
uniform vec4 diffuseProduct;
uniform vec4 specularProduct;
uniform float shininess;
uniform float aAttenuationParam;
uniform float bAttenuationParam;
uniform vec4 lightPosition[numLights];
uniform vec3 spotlightDirection[numLights];
uniform float spotlightCutoff[numLights]; // Angle in degrees
uniform float minYLight[numLights]; // Minimum y-coordinate that light can go

void main(void){
   // Normalize the input lighting vectors
   vec3 N = normalize(f_N); // Normal
   vec3 E = normalize(f_E); // Vertex position

   // Initialize frag color
   gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);

   // Calculate each light source
   for(int i=0; i<numLights; i++) {
      // Normalize light position
      vec3 L = normalize(f_L[i]);

      // Halfway vector
      vec3 H = normalize(L + E);   

      // Ambient
      vec4 ambient = ambientProduct;

      // Attenuation
      float attenuation = 1.0;
      
      // Point light or spotlight
      if(lightPosition[i].w != 0.0) {
         // Calculate attenuation
         float distance = length(L);
         attenuation = 1.0 / ( 1.0 + 
                               aAttenuationParam * distance + 
                               bAttenuationParam * pow(distance, 2) );
                    
         // If spotlight
         if(spotlightCutoff[i] <= 90.0) {
            // Calculate angle between spotlight direction vector and light                          
            float lightAngle = max(0.0, dot(-L, normalize(spotlightDirection[i])));
            
            // No light if angle outside spotlight cutoff
            if(acos(lightAngle) > radians(spotlightCutoff[i])) {
               attenuation = 0.0; 
            }
         }
      }
      
      // No light if distance exceeds minimum y position
      if(minYLight[i] > f_E.y) {
         attenuation = 0.0;
      }
      
      // Diffuse
	   float Kd = max(dot(L, N), 0.0);
      vec4 diffuse = Kd * diffuseProduct * attenuation;
       
      // Specular
      float Ks = pow(max(dot(N, H), 0.0), shininess);
      vec4 specular = Ks * specularProduct * attenuation;

      // Discard the specular highlight if the light's behind the vertex
      if(dot(L, N) < 0.0) {
      	specular = vec4(0.0, 0.0, 0.0, 1.0);
	   }
       
      gl_FragColor += ambient + diffuse + specular;
   }
   
   gl_FragColor.a = 1.0;
   
   // Texture
   gl_FragColor *= texture2D(gSampler, f_texture.xy);
}
