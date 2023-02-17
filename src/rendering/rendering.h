#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

extern SDL_Surface* g_guiSurface;

bool initWindow(const char* windowTitle);
void initRendering();
void exitRendering();

GLuint loadTexture(const char* path);
SDL_Surface* loadSDLSurface(const char* path);

void initTextures(const char* path);
void initModels(const char* path);

void renderGame();