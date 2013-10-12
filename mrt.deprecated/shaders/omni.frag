struct Light {
    vec3 color;
    vec3 position;
    float radius;
};

uniform sampler2D texColor;
uniform sampler2D texNormal;
uniform sampler2D texPos;

uniform Light light;

float k = 1.5;

float get_attenuation(float r, float d, float beta){
  float o = (-1.0/r)*d+1; //0->1, d->0
  o = pow(k*o, beta);
  return o;
}

void main()
{
  vec3 specular = vec3(0.8, 0.8, 0.8);
  float shininess = 3.0;
  float beta = 2.0;

  //FIXME: hardcoded
  vec2 texpos = gl_FragCoord.xy/vec2(1200, 800);
  vec3 material_color = texture(texColor, texpos).xyz;
  vec3 N = texture(texNormal, texpos).xyz;
  vec3 v = texture(texPos, texpos).xyz;
  
  vec3 light_dir = light.position - v;
  float distance = length(light_dir);
  vec3 L = normalize(light_dir);
  vec3 E = normalize(-v);
  vec3 R = normalize(reflect(-L,N));  
  float lambert = max(dot(N, L), 0.0);

  float attenuation = get_attenuation(light.radius, distance, beta);
  
  vec3 diffuse = material_color * light.color * lambert * attenuation;

  float specContrib = pow(max(dot(R,E),0.0), shininess);
  vec3 spec = specular * specContrib * attenuation;

  gl_FragColor = vec4(diffuse + spec, 1.0);
}


