#pragma once
#include <memory>
#include "texture.h"
#include "text.h"

class _open_gl_wrapper;

class window_base;

class open_gl_wrapper {
public:
	std::unique_ptr<_open_gl_wrapper> impl;

	open_gl_wrapper();
	~open_gl_wrapper();

	void setup(void* hwnd, window_base* base);
	void destory(void* hwnd);
	void bind_to_thread();
	void clear();
	void display();
	void set_viewport(uint32_t width, uint32_t height);
	void render_textured_rect(bool enabled, float x, float y, float width, float height, texture& t);
	void render_character(char16_t codepoint, bool enabled, float x, float y, float size, font& f);
};