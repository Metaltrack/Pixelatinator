#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB/stb_image.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int screenwidth = SCREEN_WIDTH;
int screenheight = SCREEN_HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	screenheight = height;
	screenwidth = width;
}

std::string get_file_contents(const char* filepath) {
	std::ifstream in(filepath, std::ios::binary);

	std::string contents;
	in.seekg(0, std::ios::end);
	std::streamsize size = in.tellg();

	if (size < 0) {
		return "";
	}

	contents.resize(static_cast<size_t>(size));
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());

	return contents;
}

void Check_Shader_status(unsigned int shader) {
	int isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return;
	}
	else {
		std::cout << "The Shader made it!" << std::endl;
	}
}

//If any problem regarding texture happens, it happens here..
void Initialize_texture(std::string texturepath, unsigned int* texture_ID, unsigned char* data) {
	stbi_set_flip_vertically_on_load(true);

	const char* filepath = texturepath.c_str();
	int width, height, nChannels;

	data = stbi_load(filepath, &width, &height, &nChannels, 0);
	glGenTextures(1, texture_ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = GL_RGB;
	if (nChannels == 4) format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

int main() {
	//-----------  Initialization and stuff  ----------//
	std::cout << "It works!! God damnn...." << std::endl;

	if (!glfwInit()) {
		std::cerr << "The bloody GLFW failed!!";
	}
	else {
		std::cout << "GLFW made it!!" << std::endl;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ze Pixelinator", NULL, NULL);
	if (window == nullptr) {
		std::cout << "What is up with the window its a bloody: " << window << std::endl;
	}
	else {
		std::cout << "Hell Yeah!! The canvas is ready!" << std::endl;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL()) {
		std::cerr << "Im not glad cause GLAD didnt make it...";
	}
	else {
		std::cout << "Im Glad GLAD made it" << std::endl;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//---------- Here we have our canvas ------------//
	float vertices[] = {
		// x, y
		-1.0f, -1.0f,  // bottom-left
		 1.0f, -1.0f,  // bottom-right
		 1.0f,  1.0f,  // top-right

		-1.0f, -1.0f,  // bottom-left
		 1.0f,  1.0f,  // top-right
		-1.0f,  1.0f   // top-left
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glfwSetWindowSizeCallback(window, framebuffer_size_callback);

	//---------- Shader Pipeline -----------//
	std::string VertexShaderSrc = get_file_contents(R"(C:\Visual Studio\Projects\GL\Pixel_Shader\Pixelatinator\vertex.glsl)");
	std::string FragmentShaderSrc = get_file_contents(R"(C:\Visual Studio\Projects\GL\Pixel_Shader\Pixelatinator\fragment.glsl)");

	const char* VertexSrc = VertexShaderSrc.c_str();
	const char* FragmentSrc = FragmentShaderSrc.c_str();

	unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexSrc, NULL);
	glCompileShader(VertexShader);
	Check_Shader_status(VertexShader);

	unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentSrc, NULL);
	glCompileShader(FragmentShader);
	Check_Shader_status(FragmentShader);

	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, VertexShader);
	glAttachShader(shader_program, FragmentShader);
	glLinkProgram(shader_program);
	
	//------------ Texture Stuff ------------//
	unsigned char* data = nullptr;
	unsigned int ID;
	std::string texturepath = R"(C:\Visual Studio\Projects\GL\Pixel_Shader\Pixelatinator\Textures\kitten.png)";

	Initialize_texture(texturepath, &ID, data);

	GLuint texUnit = glGetUniformLocation(shader_program, "tex");

	glUseProgram(shader_program);
	glUniform1i(texUnit, 0);

	//------------ UI stuff ----------------//
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	//---------- Ze Loop -------------------//
	std::cout << "ahhh shit.. Here we go again!" << std::endl;

	float levels = 16.0;
	static char tex_path[256];
	bool bnw = false;
	bool old_posterizer = false;
	float pixel_size = 1.0;

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("tweak panel");
		ImGui::DragFloat("Levels: ", &levels, 0.02, 0, 24);
		ImGui::DragFloat("Pixelation: ", &pixel_size, 0.02, 1, 16);
		ImGui::InputText("texture path: ", tex_path, (size_t)255);
		if (ImGui::Button("Apply texture")) {
			Initialize_texture(tex_path, &ID, data);
			glUseProgram(shader_program);
			glUniform1i(texUnit, 0);
		}
		if (ImGui::Button("Black'n White")) {
			glUseProgram(shader_program);
			if (bnw) {
				glUniform1i(glGetUniformLocation(shader_program, "bnw"), 0);
				bnw = false;
			}
			else {
				glUniform1i(glGetUniformLocation(shader_program, "bnw"), 1);
				bnw = true;
			}
		}
		ImGui::Text("DEBUG>>");
		if (ImGui::Button("use_old_posterzation")) {
			glUseProgram(shader_program);
			if (old_posterizer) {
				old_posterizer = false;
				glUniform1i(glGetUniformLocation(shader_program, "use_old_posterizer"), false);
			}
			else {
				old_posterizer = true;
				glUniform1i(glGetUniformLocation(shader_program, "use_old_posterizer"), true);
			}
		}
		ImGui::Text("OLD_POSTERIZER: %d", old_posterizer);
		ImGui::End();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniform2f(glGetUniformLocation(shader_program, "u_resolution"), (float)screenwidth, (float)screenheight);
		glUniform1f(glGetUniformLocation(shader_program, "levels"), (float)levels);
		glUniform1f(glGetUniformLocation(shader_program, "pixel_size"), (float)pixel_size);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ID);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
