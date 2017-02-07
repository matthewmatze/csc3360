attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vColor;

varying vec4 fColor;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 mv;
uniform mat4 p;
uniform vec4 LightPosition;
uniform float Shininess;
void main()
{
   vec4 ambient, diffuse, specular;
   gl_Position = p*mv*vPosition;

   vec3 N = normalize(vNormal.xyz);
   vec3 L = normalize(LightPosition.xyz - (mv*vPosition).xyz);
   vec3 E = -normalize((mv*vPosition).xyz);
   vec3 H = normalize(L+E);
   float Kd = max(dot(L, N), 0.0);
   float Ks = pow(max(dot(N, H), 0.0), Shininess);
   //ambient = AmbientProduct;
   ambient = vColor;
   diffuse = Kd*DiffuseProduct;
   specular = max(pow(max(dot(N, H), 0.0), Shininess)*SpecularProduct, 0.0);
   fColor = vec4((ambient + diffuse + specular).xyz, 1.0);
   
  // fColor = vColor;

}
