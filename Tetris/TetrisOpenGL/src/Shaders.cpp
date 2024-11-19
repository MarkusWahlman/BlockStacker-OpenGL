#include "Shaders.h"

static int GetUniformLocation(std::string uniformName, unsigned int m_ShaderID)
{
	//@todo caching

	int uniformLocation = glGetUniformLocation(m_ShaderID, uniformName.c_str());
	if (uniformLocation == -1)
		Debug::Message("Couldn't find uniform ", uniformName);
	return uniformLocation;
}

static int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int compileStatus;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));	//@todo changed from alloca to malloca?
		glGetShaderInfoLog(id, length, &length, message);
		Debug::Message("Couldn't create shader!");

		glDeleteShader(id);
		return 0;
	}
	return id;
}

static int CreateFromSourceShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//could / should call glDetachShader
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int Shader::CreateShader()
{
	return CreateFromSourceShader(s_BasicShaderVertex, s_BasicShaderFragment);
}

void Shader::SetUniform1i(std::string uniformName, unsigned int m_ShaderID, int value)
{
	glUniform1i(GetUniformLocation(uniformName, m_ShaderID), value);
}

void Shader::SetUniform1f(std::string uniformName, unsigned int m_ShaderID, float value)
{
	glUniform1f(GetUniformLocation(uniformName, m_ShaderID), value);
}

void Shader::SetUniform4f(std::string uniformName, unsigned int m_ShaderID, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(uniformName, m_ShaderID), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& uniformName, unsigned int m_ShaderID, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(uniformName, m_ShaderID), 1, GL_FALSE, &matrix[0][0]);
}