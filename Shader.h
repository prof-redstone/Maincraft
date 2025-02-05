#pragma once

#include <string>

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_rendererID;
public:
	Shader(const std::string& filename);
	~Shader();
	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
private:
	unsigned int GetUniformLocation(const std::string& name);
	bool CompileShader();
};
