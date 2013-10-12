uniform sampler2D texColor;
uniform sampler2D texNormal;
uniform sampler2D texPos;

void main()
{
    vec3 ambient = vec3(0.1, 0.1, 0.1);
    vec3 specular = vec3(0.2, 0.2, 0.2);
    vec3 light = vec3(5, 1, 5);
    float specPow = 2.0;

    vec2 texpos = gl_FragCoord.xy/vec2(1200, 800);
    vec3 color = texture(texColor, texpos).xyz;
    vec3 N = texture(texNormal, texpos).xyz;
    vec3 v = texture(texPos, texpos).xyz;
    
    vec3 L = normalize(light - v);
    vec3 E = normalize(-v);
    vec3 R = normalize(reflect(-L,N));  

    vec3 diffuse = clamp(color * max(dot(N, L), 0.0), 0.0, 1.0) ;
    vec3 spec = clamp(specular * pow(max(dot(R,E),0.0),0.3*specPow), 0.0, 1.0);

    gl_FragColor = vec4(ambient + diffuse + spec, 1.0);
}

