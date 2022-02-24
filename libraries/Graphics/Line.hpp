#ifndef LINE_H
#define LINE_H

#include "../SRW/SRW.hpp"
#include "../Math/Math.hpp"

class Line{
public:
	Line(vec3<float> parm_color){
		color = parm_color;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4<float>) * 2, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4<float>), reinterpret_cast<void*>(0));
		glBindVertexArray(0);
		program = SRW::genProgramShader("../libraries/Graphics/vs_2D.glsl", "../libraries/Graphics/fs.glsl");
	}
	Line(vec4<float> A, vec4<float> B, vec3<float> parm_color){
		color = parm_color;
		vec4<float> line[] = {A, B};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4<float>), reinterpret_cast<void*>(0));
		glBindVertexArray(0);
		program = SRW::genProgramShader("../shaders/debug/vs_debug.glsl", "../shaders/ui/fs_xico.glsl");
	}

	void render2D(vec4<float> A, vec4<float> B){
		glUseProgram(program);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		vec4<float> line[] = {A, B};
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), &line[0]);
		glUniform3fv(glGetUniformLocation(program, "color"), 1, color.address());
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render(){
		glUseProgram(program);
		glBindVertexArray(VAO);
		for(uint i = 0; i < SRW::gUBO.size(); i++)
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
		glUniformMatrix4fv(glGetUniformLocation(program, "debug_depth_model"), 1, GL_FALSE, &model[0][0]);
		glUniform3fv(glGetUniformLocation(program, "color"), 1, color.address());
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
		glUseProgram(0);
	}
private:
	mat4<float> model;
	vec3<float> color;
	uint program;
	uint VAO;
	uint VBO;
};


#endif