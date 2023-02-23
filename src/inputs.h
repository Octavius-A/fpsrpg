#pragma once

#include "gameState.h"
#include "imgui_impl_sdl.h"

void handleInputs() {
	SDL_Event event;

	SDL_GetRelativeMouseState(&g_inputState.mouseState.mouseX, &g_inputState.mouseState.mouseY);

	ImGui_ImplSDL2_ProcessEvent(&event);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: g_inputState.quit = true; break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_q: g_inputState.quit = true; break;
			case SDLK_w: g_inputState.w = true; break;
			case SDLK_a: g_inputState.a = true; break;
			case SDLK_s: g_inputState.s = true; break;
			case SDLK_d: g_inputState.d = true; break;
			case SDLK_e: g_inputState.e = true; break;
			case SDLK_F3: g_inputState.f3 = true; break;
			case SDLK_F4: g_inputState.f4 = true; break;
			default: break;
			} break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_w: g_inputState.w = false; break;
			case SDLK_a: g_inputState.a = false; break;
			case SDLK_s: g_inputState.s = false; break;
			case SDLK_d: g_inputState.d = false; break;
			case SDLK_e: g_inputState.e = false; break;
			case SDLK_F3: g_inputState.f3 = false; break;
			case SDLK_F4: g_inputState.f4 = false; break;
			default: break;
			}
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT: g_inputState.mouseState.leftButtonDown = true; break;
			case SDL_BUTTON_RIGHT: g_inputState.mouseState.rightButtonDown = true; break;
			default: break;
			} break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT: g_inputState.mouseState.leftButtonDown = false; break;
			case SDL_BUTTON_RIGHT: g_inputState.mouseState.rightButtonDown = false; break;
			} break;
		//case SDL_MOUSEMOTION:
		//	g_inputState.mouseState.mouseX = event.motion.x;
		//	g_inputState.mouseState.mouseY = event.motion.y;
		//	break;
		default: break;
		}
		ImGui_ImplSDL2_ProcessEvent(&event);
	}
}