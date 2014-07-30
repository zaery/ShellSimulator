#version 400
in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
out vec3 FragColor;
uniform vec3 ambientLight;
uniform vec4 LightPosition;
uniform vec3 lightColor;
uniform vec3 ambientMat;
uniform vec3 diffuseMat;
uniform vec3 specularMat;
uniform float Shininess;
uniform vec3 eyePos;
void main() {
	vec4 alphaMap = texture( AlphaTex, TexCoord );
	if (alphaMap.a < 0.15){
		discard;
	}else{
		//Ambient:
		vec3 ambient = ambientLight * ambientMat;

		//Diffuse:
		vec3 norm = Normal;

		vec3 mP = Position;
		vec3 s = normalize(vec3(LightPosition) - mP);
		float sDotN = dot(s, norm);
		vec3 diffuse = diffuseMat * lightColor * max(0.0, sDotN);

		//Specular:
		vec3 lightVector = normalize(vec3(LightPosition) - Position);
		vec3 lightReflectedVector = reflect(-lightVector, Normal);
		vec3 eyeVector = normalize(eyePos - Position);
		float reflectedAmount = dot(lightReflectedVector, eyeVector);
		reflectedAmount = clamp(pow(reflectedAmount, Shininess), 0.0f, 1.0f);

		vec3 spec = specularMat * vec3(reflectedAmount);
	
		//End:
		vec4 tex1Color = texture( Tex1, TexCoord );
		vec4 tex2Color = texture(Tex2, TexCoord);
		vec4 texColor = mix(tex1Color, tex2Color, tex2Color.a);
		FragColor = vec3(texColor) * (ambient + diffuse) + spec;
	}
}