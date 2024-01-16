#version 330

out vec4 colorOut;
out vec4 BrightColor;

struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	vec4 position;
	vec3 halfVector;
	vec4 coneDirection;
	float spotCosCutoff;
	float spotExponent;
};

const int MaxLights = 9; // 9 number of lights in scene
uniform LightProperties Lights[MaxLights];

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;
uniform sampler2D texmap4;
uniform sampler2D normalMap;
uniform samplerCube cubeMap;

uniform int terrainMode;
uniform int texMode;
uniform mat4 m_View;

uniform int toggleFog;
uniform bool shadowMode;
uniform int togglePbr;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
	vec4 albedo;
	float metallic;
	float roughness;
	float ao;
};

uniform Materials mat;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;

	vec2 tex_coord;
	vec3 skyboxTexCoord;

	float terrainHeight;
} DataIn;

const float reflect_factor = 0.1;
const float gamma = 2.2;
const float PI = 3.14159265359;

uniform int diffMapCount;
uniform bool specularMap;
uniform bool useNormalMap;

uniform vec3 camPos;

float map(float value, float min1, float max1, float min2, float max2) {
	// Convert the current value to a percentage
	// 0% - min1, 100% - max1
	float perc = (value - min1) / (max1 - min1);

	// Do the same operation backwards with min2 and max2
	float v = perc * (max2 - min2) + min2;
	return v;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

void main() {
 vec3 N;
 vec3 WorldPos = vec3(DataIn.position);
 vec3 e = normalize(DataIn.eye);
 vec3 V = e;
 //vec3 V = normalize(camPos - WorldPos);

 //--------Blinn-Phong------------
 vec3 spec = vec3(0.0);


 float totalIntensity = 0.0;
 vec4 totalSpec = vec4(0.0), cube_texel;
 //--------Blinn-Phong------------
 
 
 // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
 // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
 vec3 F0 = vec3(0.04); 
 F0 = mix(F0, mat.albedo.rgb, mat.metallic);

 //--------------------------------------------------------------------------------------------------------------------------------------------------
 // reflectance equation
 vec3 Lo = vec3(0.0);
 for (int lightId = 0; lightId < MaxLights; ++lightId) {
	LightProperties light = Lights[lightId];
	
	if (!light.isEnabled)
		continue;
	
	vec3 lightDirection = vec3(-light.position); // for directional light

	if(texMode == 6 || useNormalMap)  // lookup normal from normal map, move from [0,1] to [-1, 1] range, normalize
		N = normalize(2.0 * texture(normalMap, DataIn.tex_coord).rgb - 1.0);
	else
		N = normalize(DataIn.normal);

	// calculate per-light radiance
    vec3 L = normalize(vec3(light.position) - WorldPos);
    vec3 H = normalize(V + L);
    float dist = length(vec3(light.position) - WorldPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = vec3(1.5, 1.5, 1.5);


 //--------Blinn-Phong------------
	vec3 l = normalize(vec3(light.position - DataIn.position));
	float intensity = 0.0f;
	float intSpec = 0.0f;
	vec4 spec = vec4(0.0);
 //--------Blinn-Phong------------


	if (light.isLocal && light.isSpot) // spotlight
	{
		float att = 0; // attenuation

		vec3 sd = normalize(vec3(-light.coneDirection));

		float spotCos = dot(l, sd);
		if(spotCos > light.spotCosCutoff)  {	//inside cone?
			att = pow(spotCos, light.spotExponent);

			//--------Blinn-Phong------------
			intensity = max(dot(N,l), 0.0) * att;
			if (intensity > 0.0) {
				vec3 h = normalize(l + e);
				intSpec = max(dot(h,N), 0.0);
				spec = mat.specular * pow(intSpec, mat.shininess) * att;
			}
			//--------Blinn-Phong------------

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, mat.roughness);   
			float G   = GeometrySmith(N, V, L, mat.roughness);      
			vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
			vec3 numerator    = NDF * G * F; 
			float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
			vec3 specular = numerator / denominator;
        
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0 - mat.metallic;	  

			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);        

			// add to outgoing radiance Lo
			Lo += (kD * mat.albedo.rgb / PI + specular) * radiance * att * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		}
	}
	else if (light.isLocal && !light.isSpot) // point light
	{
		//--------Blinn-Phong------------
		intensity = max(dot(N,l), 0.0);
		if (intensity > 0.0) {
			vec3 h = normalize(l + e);
			intSpec = max(dot(h,N), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess);
		}
		//--------Blinn-Phong------------

		// Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, mat.roughness);   
        float G   = GeometrySmith(N, V, L, mat.roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - mat.metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * mat.albedo.rgb / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}
	else if (!light.isLocal) // directional light same has point but origin is point in infinity so no direction calc
    {
		//--------Blinn-Phong------------
        intensity = max(dot(N,lightDirection), 0.0);
        if (intensity > 0.0) {
            vec3 h = normalize(vec3(0.0, -1.0, 0.0)); // FIXME hardcoded half-vector
            intSpec = max(dot(h,N), 0.0);
            spec = mat.specular * pow(intSpec, mat.shininess);
        }
		//--------Blinn-Phong------------

		// Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, mat.roughness);   
        float G   = GeometrySmith(N, V, lightDirection, mat.roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - mat.metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * mat.albedo.rgb / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    //float dist = length(light.position - DataIn.position);
	//float attenuation = 1.0 / dist * dist;

	//--------Blinn-Phong------------
	intensity *= attenuation;
	spec *= attenuation;

	totalIntensity += intensity;
	totalSpec += spec;
	//--------Blinn-Phong------------
 }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

    vec3 ambient = vec3(0.03) * mat.albedo.rgb * mat.ao;
    vec3 color = ambient + Lo;
	
 	vec4 texel, texel1; 

	if(shadowMode)
		colorOut = vec4(vec3(0.5), 1.0);
	else {
		if(texMode == 0 || texMode == 6) // no texture applied
		{
			if (togglePbr == 0)
				colorOut = max(totalIntensity * mat.diffuse + totalSpec, mat.ambient);
			else
				colorOut = vec4(color, mat.albedo.a);
		}
		if(texMode == 1) // modulate diffuse color with texel color
		{
			texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
			if (togglePbr == 0)
				colorOut = max(totalIntensity * mat.diffuse * texel + totalSpec,0.07 * texel);
			else
				colorOut = max(vec4(color, mat.albedo.a) * texel,0.07 * texel);
		}
		else if (texMode == 2) // diffuse color is replaced by texel color, with specular area or ambient (0.1*texel)
		{
			texel = texture(texmap, DataIn.tex_coord);  // texel from stone.tga
			if (togglePbr == 0)
				colorOut = max(totalIntensity*texel + totalSpec, 0.07*texel);
			else {
				color = ambient / mat.albedo.rgb + Lo;
				colorOut = max(vec4(color, mat.albedo.a)*texel, 0.07*texel);
			}
		}
		else if (texMode == 3) // multitexturing
		{
			texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
			texel1 = texture(texmap1, DataIn.tex_coord);  // texel from checker.tga
			if (togglePbr == 0)
				colorOut = vec4(max(totalIntensity * texel.rgb * texel1.rgb + totalSpec.rgb, 0.07 * texel.rgb * texel1.rgb), 0.80);
			else {
				color = ambient / mat.albedo.rgb + Lo;
				colorOut = vec4(max(color * texel.rgb * texel1.rgb, 0.07 * texel.rgb * texel1.rgb), 0.80);
			}
		}
		else if (texMode == 4) // billboards
		{
			texel = texture(texmap3, DataIn.tex_coord);  // texel from tree.tga

			if(texel.a == 0.0) discard;
			else {
				if (togglePbr == 0)
					colorOut = max(totalIntensity*texel + totalSpec, 0.1*texel);
				else {
					color = ambient / mat.albedo.rgb + Lo;
					colorOut = max(vec4(color, mat.albedo.a)*texel, 0.1*texel);
				}
			}
		}
		else if (texMode == 5) // cubemap
		{
			colorOut = texture(cubeMap, DataIn.skyboxTexCoord);
		}
		else if(texMode == 7) // Environmental cube mapping
		{
			vec3 reflected1 = vec3 (transpose(m_View) * vec4 (vec3(reflect(-e, N)), 0.0)); //reflection vector in world coord
			reflected1.x = -reflected1.x;   
			cube_texel = texture(cubeMap, reflected1);

			texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
			texel1 = texture(texmap1, DataIn.tex_coord);  // texel from checker.tga

			vec4 aux_color = mix(texel, cube_texel, reflect_factor);
			vec4 aux_color1 = mix(texel1, cube_texel, reflect_factor);

			color = ambient / mat.albedo.rgb + Lo;

			if (togglePbr == 0)
				aux_color = max(totalIntensity*aux_color*aux_color1 + totalSpec, 0.1*aux_color*aux_color1);
			else
				aux_color = max(vec4(color, mat.albedo.a)*aux_color*aux_color1, 0.1*aux_color*aux_color1);

			colorOut = vec4(aux_color.rgb, 1.0);
		}
		else if (texMode == 8)
		{
			texel = texture(texmap, DataIn.tex_coord);  // texel from tree.tga

			if(texel.a == 0.0 || (mat.diffuse.a == 0.0) ) discard;
			else {
				if (togglePbr == 0)
					colorOut = max(mat.diffuse*texel, 0.1*texel);
				else
					colorOut = max(vec4(color, mat.albedo.a)*texel, 0.1*texel);
			}
		}
		else if (texMode == 9) // obj from assimp
		{
			vec4 diff, auxSpec;
			vec4 c;
			if(mat.texCount == 0) {
				if (togglePbr == 0) {
					diff = mat.diffuse;
					auxSpec = mat.specular;
				}
				else
					diff = vec4(color, mat.albedo.a);
			}
			else {
				if(diffMapCount == 0) {
					if (togglePbr == 0)
						diff = mat.diffuse;
					else
						diff = vec4(color, mat.albedo.a);
				}
				else if(diffMapCount == 1) {
					if (togglePbr == 0)
						diff = mat.diffuse * texture(texmap, DataIn.tex_coord);
					else
						diff = texture(texmap, DataIn.tex_coord);
				}
				else {
					if (togglePbr == 0)
						diff = mat.diffuse * texture(texmap, DataIn.tex_coord) * texture(texmap1, DataIn.tex_coord);
					else
						diff = texture(texmap, DataIn.tex_coord) * texture(texmap1, DataIn.tex_coord);
				}
				if(specularMap) {
					if (togglePbr == 0)
						auxSpec = mat.specular * texture(texmap2, DataIn.tex_coord);
					else
						auxSpec = texture(texmap2, DataIn.tex_coord);
				}
				else
					auxSpec = mat.specular;
			}

			if (togglePbr == 0)
				colorOut = vec4((max(totalIntensity * diff, diff*0.15) + (totalSpec * auxSpec)).rgb, 1.0);
			else
				colorOut = vec4(diff.rgb, 1.0); // I have no idea what you are supposed to do here
		}
	}


	if (terrainMode == 1) {
		float shift = 5.0;
		float scale = 64.0;
		float h = (DataIn.terrainHeight + shift)/scale;
		vec3 color = vec3(1.0, 0.0, 0.0);
		vec3 baseColor = vec3(0.0, 0.32, 0.1);
		vec3 midColor = vec3(0.12, 0.08, 0.0);
		vec3 midColor2 = vec3(0.1, 0.02, 0.0);
		vec3 topColor = vec3(0.98, 0.98, 0.98);
		float i;
		if (h < 0.1) {
			i = map(h, 0, 0.1, 0, 1);
			color = mix(baseColor, midColor, i);
		}
		else if (h > 0.1 && h < 0.4) {
			i = map(h, 0.1, 0.4, 0, 1);
			color = mix(midColor, midColor2, i);
		}
		else if (h > 0.4 && h < 0.65) {
			i = map(h, 0.4, 0.65, 0, 1);
			color = mix(midColor2, topColor, i);
		} else if (h > 0.65) {
			color = topColor;
		}
		colorOut = vec4(color, 1.0);
//		colorOut = totalIntensity * vec4(color, 1.0) + totalSpec;
	}

	//Fog effect
	if (toggleFog == 1)
    {
        vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);
        float fogAmount = exp(-length(DataIn.position) * 0.05); // position in camera space
        colorOut = mix(fogColor, colorOut, fogAmount);
    }

	colorOut.rgb = pow(colorOut.rgb, vec3(1.0/gamma)); //gamma correction

	// check whether result is higher than some threshold, if so, output as bloom threshold color
	float brightness = dot(colorOut.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0 && texMode != 4)
        BrightColor = vec4(colorOut.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

	
}