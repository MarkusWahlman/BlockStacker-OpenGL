#pragma once


#include "GL/glew.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include "debug.h"

/*Deals with shaders, and includes them*/

namespace Shader
{
	int CreateShader();

	void SetUniform1i(std::string uniformName, unsigned int m_ShaderID, int value);
	void SetUniform1f(std::string uniformName, unsigned int m_ShaderID, float value);
	void SetUniform4f(std::string uniformName, unsigned int m_ShaderID, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& uniformName, unsigned int m_ShaderID, const glm::mat4& matrix);
};

/* https://tomeko.net/online_tools/cpp_text_escape.php?lang=en */

inline std::string s_BasicShaderVertex =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 a_Position;\n"
"layout(location = 1) in vec4 a_Color;\n"
"\n"
"uniform mat4 u_MVP;\n"
"\n"
"out vec4 v_Color;\n"
"\n"
"void main()\n"
"{\n"
"\tv_Color = a_Color;\n"
"\tgl_Position= u_MVP * vec4(a_Position, 1.0);\n"
"}";

inline std::string s_BasicShaderFragment =
"#version 330 core\n"
"\n"
"layout (location = 0) out vec4 o_Color;\n"
"\n"
"in vec4 v_Color;\n"
"\n"
"void main()\n"
"{\n"
"\to_Color = v_Color;\n"
"}";