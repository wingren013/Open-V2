#include "open_gl_setup.h"
#define GLEW_STATIC

#include "glew.h"
#include "wglew.h"
#include <Windows.h>
#include <wingdi.h>
#include <string>
#include <atomic>
#include <thread>

#include <chrono>

#include "v2_window.hpp"

#ifdef _DEBUG
void debug_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* param) {

	std::string source_str;
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			source_str = "OpenGL API call"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			source_str = "Window system API"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			source_str = "Shading language compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			source_str = "Application associated with OpenGL"; break;
		case GL_DEBUG_SOURCE_APPLICATION:
			source_str = "User generated"; break;
		case GL_DEBUG_SOURCE_OTHER:
			source_str = "Unknown source"; break;
	}
	std::string error_type;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			error_type = "General error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			error_type = "Deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			error_type = "Undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY:
			error_type = "Portability issue"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			error_type = "Performance issue"; break;
		case GL_DEBUG_TYPE_MARKER:
			error_type = "Command stream annotation"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			error_type = "Error group push"; break;
		case GL_DEBUG_TYPE_POP_GROUP:
			error_type = "Error group pop"; break;
		case GL_DEBUG_TYPE_OTHER:
			error_type = "Uknown error type"; break;
	}
	std::string severity_str;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			severity_str = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severity_str = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW:
			severity_str = "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severity_str = "Notification"; break;
	}
	std::string full_message("OpenGL error ");
	full_message += std::to_string(id);
	full_message += " ";
	full_message += " source: ";
	full_message += source_str;
	full_message += " type: ";
	full_message += error_type;
	full_message += " severity: ";
	full_message += severity_str;
	full_message += "; ";
	full_message += message;
	full_message += "\n";

	OutputDebugStringA(full_message.c_str());
}
#endif


char tquad_vertex_shader[] =
"#version 430 core\n"
"layout (location = 0) in vec2 vertex_position;\n"
"layout (location = 1) in vec2 v_tex_coord;\n"
"\n"
"out vec2 tex_coord;\n"
"\n"
"uniform float screen_width;\n"
"uniform float screen_height;\n"
"uniform float rect_top;\n"
"uniform float rect_left;\n"
"uniform float rect_width;\n"
"uniform float rect_height;\n"
""
"void main() {\n"
"	gl_Position = vec4(-1.0 + (2.0 * ((vertex_position.x * rect_width)  + rect_left) / screen_width), "
"1.0 - (2.0 * ((vertex_position.y * rect_height)  + rect_top) / screen_height), "
"0.0, 1.0);\n"
"	tex_coord = v_tex_coord;\n"
"}\n";


char tquad_fragment_shader[] =
"#version 430 core\n"
"\n"
"subroutine vec4 color_function_class(vec4 color_in);\n"
"subroutine uniform color_function_class coloring_function;\n"
"\n"
"in vec2 tex_coord;\n"
"layout (location = 0) out vec4 frag_color;"
"\n"
"layout (binding = 0) uniform sampler2D texture_sampler;"
"uniform float edge_blur;\n"
"uniform float border_size;\n"
"uniform vec3 inner_color;\n"
"uniform vec3 border_color;\n"
"\n"
"subroutine(color_function_class)\n"
"vec4 enabled_color(vec4 color_in) {\n"
"	return color_in;\n"
"}\n"
"\n"
"subroutine(color_function_class)\n"
"vec4 border_filter(vec4 color_in) {\n"
"	if(color_in.r > 0.5) {"
"		return vec4(inner_color, 1.0);\n"
"	} else if(color_in.r > 0.5 - border_size / 4.0) {"
"		float sm_val = smoothstep(0.5 - border_size / 4.0, 0.5, color_in.r);\n"
"		return vec4(mix(border_color, inner_color, sm_val), 1.0);"
"	} else {\n"
"		float sm_val = smoothstep(0.5 - border_size * 5.0 / 4.0, 0.5 - border_size, color_in.r);\n"
"		return vec4(border_color, sm_val);\n"
"	}\n"
"}\n"
"\n"
"subroutine(color_function_class)\n"
"vec4 color_filter(vec4 color_in) {\n"
"	float sm_val = smoothstep(0.5 - edge_blur, 0.5, color_in.r);\n"
"	return vec4(inner_color, sm_val);\n"
"}\n"
"\n"
"subroutine(color_function_class)\n"
"vec4 disabled_color(vec4 color_in) {\n"
"	const float amount = (color_in.x + color_in.y + color_in.z) / 4.0;\n"
"	return vec4(amount, amount, amount, color_in.w);\n"
"}\n"
"\n"
"void main() {\n"
"	frag_color = coloring_function(texture(texture_sampler, tex_coord));\n"
"}\n";

/*
glActiveTexture(GL_TEXTURE0);

to unbind ... 
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, nullptr);
*/

/*
//texture to ...
int loc = glGetUniformLocation(programHandle, "texture_sampler");

glActiveTexture(GL_TEXTURE0);

glUniform1i(loc, 0); <- bind texture in slot zero to sampler
*/

GLuint general_shader = 0;

GLuint global_square_vao = 0;
GLuint global_sqaure_buffer = 0;

GLuint general_parameter_screen_width = 0;
GLuint general_parameter_screen_height = 0;
GLuint general_parameter_rect_top = 0;
GLuint general_parameter_rect_left = 0;
GLuint general_parameter_rect_width = 0;
GLuint general_parameter_rect_height = 0;

GLuint general_parameter_inner_color = 0;
GLuint general_parameter_border_color = 0;
GLuint general_parameter_border_size = 0;
GLuint general_parameter_edge_blur = 0;

GLuint general_parameter_enabled = 0;;
GLuint general_parameter_disabled = 0;;
GLuint general_parameter_border_filter = 0;;
GLuint general_parameter_filter = 0;;

GLfloat global_square_data[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

GLuint sub_sqaure_buffers[64] = { 0 };

void create_global_square() {
	glGenBuffers(1, &global_sqaure_buffer);

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, global_sqaure_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &global_square_vao);
	glBindVertexArray(global_square_vao);
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //texture coordinates

	glBindVertexBuffer(0, global_sqaure_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0); //position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2); //texture coordinates
	glVertexAttribBinding(0, 0); //position -> to array zero
	glVertexAttribBinding(1, 0); //texture coordinates -> to array zero 

	glGenBuffers(64, sub_sqaure_buffers);
	for (uint32_t i = 0; i < 64; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, sub_sqaure_buffers[i]);

		const float cell_x = static_cast<float>(i & 7) / 8.0f;
		const float cell_y = static_cast<float>((i >> 3) & 7) / 8.0f;

		GLfloat global_sub_square_data[] = {
			0.0f, 0.0f, cell_x, cell_y,
			0.0f, 1.0f, cell_x, cell_y + 1.0f / 8.0f,
			1.0f, 1.0f, cell_x + 1.0f / 8.0f, cell_y + 1.0f / 8.0f,
			1.0f, 0.0f, cell_x + 1.0f / 8.0f, cell_y
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_sub_square_data, GL_STATIC_DRAW);
	}
}

void create_shaders() {
	GLuint general_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint general_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

#ifdef _DEBUG
	if (general_vertex_shader == 0 || general_fragment_shader == 0) {
		MessageBox(nullptr, L"shader creation failed", L"OpenGL error", MB_OK);
		std::abort();
	}
#endif
	const GLchar* array_a[] = { tquad_vertex_shader };

	glShaderSource(general_vertex_shader, 1, array_a, nullptr);
	glCompileShader(general_vertex_shader);

#ifdef _DEBUG
	GLint result;
	glGetShaderiv(general_vertex_shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		MessageBox(nullptr, L"vertex shader compilation failed", L"OpenGL error", MB_OK);

		GLint logLen;
		glGetShaderiv(general_vertex_shader, GL_INFO_LOG_LENGTH, &logLen);
		
		char * log = new char[logLen];
		GLsizei written;
		glGetShaderInfoLog(general_vertex_shader, logLen, &written, log);

		MessageBoxA(nullptr, log, "OpenGL error", MB_OK);
		delete[] log;

		std::abort();
	}
#endif


	const GLchar* array_b[] = { tquad_fragment_shader };

	glShaderSource(general_fragment_shader, 1, array_b, nullptr);
	glCompileShader(general_fragment_shader);

#ifdef _DEBUG
	glGetShaderiv(general_fragment_shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		MessageBox(nullptr, L"fragment shader compilation failed", L"OpenGL error", MB_OK);

		GLint logLen;
		glGetShaderiv(general_fragment_shader, GL_INFO_LOG_LENGTH, &logLen);

		char * log = new char[logLen];
		GLsizei written;
		glGetShaderInfoLog(general_fragment_shader, logLen, &written, log);

		MessageBoxA(nullptr, log, "OpenGL error", MB_OK);
		delete[] log;

		std::abort();
	}
#endif

	general_shader = glCreateProgram();

#ifdef _DEBUG
	if (general_shader == 0) {
		MessageBox(nullptr, L"shader program creation failed", L"OpenGL error", MB_OK);
		std::abort();
	}
#endif

	glAttachShader(general_shader, general_vertex_shader);
	glAttachShader(general_shader, general_fragment_shader);
	glLinkProgram(general_shader);

#ifdef _DEBUG
	glGetProgramiv(general_shader, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		MessageBox(nullptr, L"shader program linking failed", L"OpenGL error", MB_OK);

		GLint logLen;
		glGetProgramiv(general_shader, GL_INFO_LOG_LENGTH, &logLen);

		char * log = new char[logLen];
		GLsizei written;
		glGetProgramInfoLog(general_shader, logLen, &written, log);
		MessageBoxA(nullptr, log, "OpenGL error", MB_OK);

		delete[] log;
		std::abort();
	}
#endif
	glDeleteShader(general_vertex_shader);
	glDeleteShader(general_fragment_shader);

	glUseProgram(general_shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	general_parameter_screen_width = glGetUniformLocation(general_shader, "screen_width");
	general_parameter_screen_height = glGetUniformLocation(general_shader, "screen_height");
	general_parameter_rect_top = glGetUniformLocation(general_shader, "rect_top");
	general_parameter_rect_left = glGetUniformLocation(general_shader, "rect_left");
	general_parameter_rect_width = glGetUniformLocation(general_shader, "rect_width");
	general_parameter_rect_height = glGetUniformLocation(general_shader, "rect_height");


	general_parameter_inner_color = glGetUniformLocation(general_shader, "inner_color");
	general_parameter_border_color = glGetUniformLocation(general_shader, "border_color");
	general_parameter_border_size = glGetUniformLocation(general_shader, "border_size");
	general_parameter_edge_blur = glGetUniformLocation(general_shader, "edge_blur");

	general_parameter_enabled = glGetSubroutineIndex(general_shader, GL_FRAGMENT_SHADER, "enabled_color");
	general_parameter_disabled = glGetSubroutineIndex(general_shader, GL_FRAGMENT_SHADER, "disabled_color");
	general_parameter_filter = glGetSubroutineIndex(general_shader, GL_FRAGMENT_SHADER, "color_filter");
	general_parameter_border_filter = glGetSubroutineIndex(general_shader, GL_FRAGMENT_SHADER, "border_filter");
}

HGLRC setup_opengl_context(HWND hwnd, HDC window_dc) {

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	const int pixel_format = ChoosePixelFormat(window_dc, &pfd);
	SetPixelFormat(window_dc, pixel_format, &pfd);

	auto handle_to_ogl_dc = wglCreateContext(window_dc);
	wglMakeCurrent(window_dc, handle_to_ogl_dc);

	
	glewExperimental = GL_TRUE;

	if (glewInit() != 0) {
		MessageBox(hwnd, L"GLEW failed to initialize", L"GLEW error", MB_OK);
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 
#ifdef _DEBUG
		WGL_CONTEXT_DEBUG_BIT_ARB | 
#endif
		0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") != 1) {
		MessageBox(hwnd, L"WGL_ARB_create_context not supported", L"OpenGL error", MB_OK);
	} else {
		auto new_context = wglCreateContextAttribsARB(window_dc, NULL, attribs);

		wglMakeCurrent(window_dc, NULL);
		wglDeleteContext(handle_to_ogl_dc);
		wglMakeCurrent(window_dc, new_context);

#ifdef _DEBUG
		glDebugMessageCallback(debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

		if (wglewIsSupported("WGL_EXT_swap_control_tear") == 1) {
			wglSwapIntervalEXT(-1);
		} else if (wglewIsSupported("WGL_EXT_swap_control") == 1) {
			wglSwapIntervalEXT(1);
		} else {
			MessageBox(hwnd, L"WGL_EXT_swap_control_tear and WGL_EXT_swap_control not supported", L"OpenGL error", MB_OK);
		}

		handle_to_ogl_dc = new_context;
	}
	

	create_shaders();
	create_global_square();

	return handle_to_ogl_dc;
}

void release_opengl_context(void* _hwnd, HGLRC context) {
	HWND hwnd = (HWND)_hwnd;
	HDC window_dc = GetDC(hwnd);

	wglMakeCurrent(window_dc, NULL);
	wglDeleteContext(context);

	ReleaseDC(hwnd, window_dc);
}

class _open_gl_wrapper {
public:
	HGLRC context;
	HDC window_dc;
	std::thread render_thread;

	uint32_t viewport_x = 0;
	uint32_t viewport_y = 0;
	std::atomic<bool> update_viewport = false;
	std::atomic<bool> active = true;
};

open_gl_wrapper::open_gl_wrapper() : impl(std::make_unique<_open_gl_wrapper>()) {}
open_gl_wrapper::~open_gl_wrapper() {}

void open_gl_wrapper::setup(void* hwnd, window_base* base) {
	impl->render_thread = std::thread([hwnd, base, _this = this]() {
		const auto iptr = _this->impl.get();

		iptr->window_dc = GetDC((HWND)hwnd);
		iptr->context = setup_opengl_context((HWND)hwnd, iptr->window_dc);

		while (iptr->active.load(std::memory_order::memory_order_acquire)) {
			//if we want to slow down to 30 fps ...
			//const auto start_time = std::chrono::high_resolution_clock::now();

			_this->clear();
			base->render_dispatch(base);
			_this->display();

			//const auto end_time = std::chrono::high_resolution_clock::now();
			//const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
			//if (elapsed < (1000 / 30)) {
			//	Sleep((1000 / 30) - elapsed);
			//}
		}
	});
}

void open_gl_wrapper::destory(void* hwnd) {
	impl->active.store(false, std::memory_order::memory_order_release);
	impl->render_thread.join();
	release_opengl_context((HWND)hwnd, impl->context);
	ReleaseDC((HWND)hwnd, impl->window_dc);
}

void open_gl_wrapper::bind_to_thread() {
	wglMakeCurrent(impl->window_dc, impl->context);
}

void open_gl_wrapper::display() {
	SwapBuffers(impl->window_dc);
}

void open_gl_wrapper::clear() {
	if (impl->update_viewport.load(std::memory_order::memory_order_acquire)) {
		impl->update_viewport.store(false, std::memory_order::memory_order_release);
		glViewport(0, 0, impl->viewport_x, impl->viewport_y);
		glDepthRange(-1.0, 1.0);
	}

	//glUseProgram(...)
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void open_gl_wrapper::render_textured_rect(bool enabled, float x, float y, float width, float height, texture& t) {
	//glBindBuffer(GL_ARRAY_BUFFER, global_sqaure_buffer);
	
	glBindVertexArray(global_square_vao);

	glBindVertexBuffer(0, global_sqaure_buffer, 0, sizeof(GLfloat) * 4);

	glUniform1f(general_parameter_screen_width, impl->viewport_x);
	glUniform1f(general_parameter_screen_height, impl->viewport_y);
	glUniform1f(general_parameter_rect_top, y);
	glUniform1f(general_parameter_rect_left, x);
	glUniform1f(general_parameter_rect_height, height);
	glUniform1f(general_parameter_rect_width, width);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	if(enabled)
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &general_parameter_enabled); // must set all subroutines in one call
	else
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &general_parameter_disabled);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void open_gl_wrapper::render_character(char16_t codepoint, bool enabled, float x, float y, float size, font& f) {
	const auto g = f.get_glyph(codepoint);

	glBindVertexArray(global_square_vao);
	glBindVertexBuffer(0, sub_sqaure_buffers[g.buffer], 0, sizeof(GLfloat) * 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g.texture);

	glUniform1f(general_parameter_screen_width, impl->viewport_x);
	glUniform1f(general_parameter_screen_height, impl->viewport_y);
	glUniform1f(general_parameter_rect_top, y);
	glUniform1f(general_parameter_rect_left, x);
	glUniform1f(general_parameter_rect_height, size);
	glUniform1f(general_parameter_rect_width, size);


	glUniform3f(general_parameter_inner_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(general_parameter_border_color, 1.0f, 1.0f, 1.0f);
	glUniform1f(general_parameter_border_size, 0.08 );

	glUniform1f(general_parameter_edge_blur, 0.02f + 2.0f / size);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &general_parameter_border_filter); // must set all subroutines in one call
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &general_parameter_enabled); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void open_gl_wrapper::set_viewport(uint32_t width, uint32_t height) {
	impl->viewport_x = width;
	impl->viewport_y = height;
	impl->update_viewport.store(true, std::memory_order::memory_order_release);
}