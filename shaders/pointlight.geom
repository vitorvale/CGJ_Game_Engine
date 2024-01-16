#version 330 core
layout (triangles) in;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;

	vec2 tex_coord;
	vec3 skyboxTexCoord;

	float terrainHeight;
} DataIn[];

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_Model; 
uniform mat3 m_normal;


layout (triangle_strip, max_vertices = 3) out;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;

	vec2 tex_coord;
	vec3 skyboxTexCoord;

	float terrainHeight;
} DataOut;

uniform float timeTotal;
uniform int grassMode;
uniform int shaking;
uniform int pauseMode;

vec4 shakeDisplacement(vec4 pos){

		float strength = 0.001;
        pos.x += cos(timeTotal * 10) * strength;        
        pos.z += cos(timeTotal * 15) * strength;

		return pos;
}

vec4 calculateWindDisplacement(vec4 pos){
	float val = sin(timeTotal);

	if(pos.y >= 0.1 && pos.y <= 0.2){
		pos.x = pos.x + val/30.0;
	}
	else if(pos.y >= 0.2 && pos.y <= 0.3){
		pos.x = pos.x + val/20.0;
	}
	else if(pos.y >= 0.3 && pos.y <= 0.4){
		pos.x = pos.x + val/15.0;
	}
	else if(pos.y >= 0.4 && pos.y <= 0.5){
		pos.x = pos.x + val/11.0;
	}
	else if(pos.y >= 0.5 && pos.y <= 0.6){
		pos.x = pos.x + val/10.0;
	}
	else if(pos.y >= 0.6 && pos.y <= 0.7){
		pos.x = pos.x + val/9.0;
	}
	else if(pos.y >= 0.7 && pos.y <= 0.8){
		pos.x = pos.x + val/8.0;
	}
	else if (pos.y >= 0.8 && pos.y <= 0.9){
		pos.x = pos.x + val/7.0;
	}
	else if (pos.y >= 0.9 && pos.y <= 1.1){
		pos.x = pos.x + val/6.0;
	}

	return pos;
}

void main() {
	DataOut.normal = DataIn[0].normal;
	DataOut.eye = DataIn[0].eye;
	DataOut.lightDir = DataIn[0].lightDir;
	DataOut.position = DataIn[0].position;
	DataOut.tex_coord = DataIn[0].tex_coord;
	DataOut.skyboxTexCoord = DataIn[0].skyboxTexCoord;
	DataOut.terrainHeight = DataIn[0].terrainHeight;


	vec4 aux_pos =  gl_in[0].gl_Position;

	if (grassMode == 1){
		aux_pos = calculateWindDisplacement(aux_pos);
	}

	if (shaking == 1){
		aux_pos = shakeDisplacement(aux_pos);
	}
	
	DataOut.position = m_viewModel *  aux_pos;
    gl_Position = m_pvm * aux_pos; 
    EmitVertex();

	DataOut.normal = DataIn[1].normal;
	DataOut.eye = DataIn[1].eye;
	DataOut.lightDir = DataIn[1].lightDir;
	DataOut.position = DataIn[1].position;
	DataOut.tex_coord = DataIn[1].tex_coord;
	DataOut.skyboxTexCoord = DataIn[1].skyboxTexCoord;
	DataOut.terrainHeight = DataIn[1].terrainHeight;

	aux_pos =  gl_in[1].gl_Position;

	if (grassMode == 1){
		aux_pos = calculateWindDisplacement(aux_pos);
	}

	if (shaking == 1){
		aux_pos = shakeDisplacement(aux_pos);
	}

	DataOut.position = m_viewModel *  aux_pos;
    gl_Position = m_pvm * aux_pos; 
    EmitVertex();

	DataOut.normal = DataIn[2].normal;
	DataOut.eye = DataIn[2].eye;
	DataOut.lightDir = DataIn[2].lightDir;
	DataOut.position = DataIn[2].position;
	DataOut.tex_coord = DataIn[2].tex_coord;
	DataOut.skyboxTexCoord = DataIn[2].skyboxTexCoord;
	DataOut.terrainHeight = DataIn[2].terrainHeight;

	aux_pos =  gl_in[2].gl_Position;

	if (grassMode == 1){
		aux_pos = calculateWindDisplacement(aux_pos);
	}

	if (shaking == 1){
		aux_pos = shakeDisplacement(aux_pos);
	}

	DataOut.position = m_viewModel *  aux_pos;
    gl_Position = m_pvm * aux_pos; 
    EmitVertex();
    EndPrimitive();
}  