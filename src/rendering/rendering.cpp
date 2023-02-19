#include "rendering.h"
#include "shader.h"
#include "../GUI/simpleGUI.h"
#include "worldCellMesh.h"
#include "model.h"
#include "../gameState.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using json = nlohmann::json;

SDL_Surface* g_guiSurface = NULL;
SDL_Window* g_window = NULL;
SDL_GLContext g_context;
Shader* shader = NULL;

std::map<unsigned int, GLuint> textureIdMap;
std::unordered_map<unsigned int, Model*> modelBank;

unsigned int VBO;
unsigned int VAO;

//glm::vec3 cameraPos = glm::vec3(0.0f, 18.5f, 18.5f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;

static constexpr float c_maxPitch = 89.0f;
static constexpr float c_minPitch = -89.0f;

static const float FOV = 80.0f;

bool initWindow(const char* windowTitle ) {

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	g_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	if (g_window == NULL) {
		std::cout << "FAILED TO INIT WINDOW" << std::endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	g_context = SDL_GL_CreateContext(g_window);
	
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "FAILED TO INIT GLAD" << std::endl;
		return 0;
	}

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(g_window, g_context);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_CaptureMouse(SDL_TRUE);
	return true;
}

void initRendering() {
    initVertData();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, fullVertData.size() * sizeof(float), &fullVertData[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // position attrib
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // tex coord attrib
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader = new Shader("shaders/phongvert.glsl", "shaders/phongfrag.glsl");

        

}

void exitRendering() {
    delete shader;
    glDeleteVertexArrays(0, &VAO);
    glDeleteBuffers(1, &VBO);
    SDL_FreeSurface(g_guiSurface);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyWindow(g_window);
    try {
        SDL_Quit();
    }
    catch (char* e) {
        std::cout << "Exception Caught: " << e << std::endl;
    }
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    SDL_Surface* surface = IMG_Load(path);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    int mode = GL_RGB;

    if (surface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_FreeSurface(surface);
    return textureID;
}

SDL_Surface* loadSDLSurface(const char* path) {
    return IMG_Load(path);
}

void initTextures(const char* path) {
    std::ifstream f(path);
    json data = json::parse(f);

    for (json::iterator it = data["textures"].begin(); it != data["textures"].end(); ++it) {
        json texture = *it;
        unsigned int txId = texture["id"];
        std::string txPath = texture["path"];
        
        GLuint glTextureId = loadTexture(txPath.c_str());

        textureIdMap[txId] = glTextureId;
    }    
}

void initModels(const char* path) {
    std::ifstream f(path);
    json data = json::parse(f);

    for (json::iterator it = data["models"].begin(); it != data["models"].end(); ++it) {
        json modelData = *it;

        std::filesystem::path p = modelData["model_path"];
        unsigned int modelId = modelData["id"];
        std::string path = modelData["model_path"];
        //modelBank[modelId] = new Model(std::filesystem::absolute(p).generic_string().c_str());
        modelBank[modelId] = new Model(path.c_str());
    }
}

void renderGame() {
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    shader->use();

    cameraFront = g_gameState.player->direction;

    glm::vec3 finalCameraPos = g_gameState.player->position;

    glm::mat4 view = glm::lookAt(finalCameraPos, finalCameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(g_gameState.cameraFov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 50.0f);
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec3("viewPos", finalCameraPos);

    // Set the directional light
    shader->setVec3("dirLight.direction", -0.3f, -1.0f, -1.0f);
    shader->setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
    shader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);

    // set the point light
    glm::vec3 lightPos(2.0f, 1.0f, 2.0f);
    shader->setVec3("pointLight.position", lightPos);
    shader->setVec3("pointLight.ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);

    shader->setFloat("pointLight.constant", 1.0f);
    shader->setFloat("pointLight.linear", 0.09f);
    shader->setFloat("pointLight.quadratic", 0.032f);


    // Render the tiles
    int tilesOnScreenX = 50;
    int tilesOnScreenY = 50;


    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    for (int y = g_gameState.player->position.z - (tilesOnScreenY / 2); y < g_gameState.player->position.z + (tilesOnScreenY / 2); ++y) {
        for (int x = g_gameState.player->position.x - (tilesOnScreenX / 2); x < g_gameState.player->position.x + (tilesOnScreenX / 2); ++x) {
            if (y >= 0 && y < g_gameState.worldYMax && x >= 0 && x < g_gameState.worldXMax) {
                WorldCell_Simple cell = g_gameState.worldArray_simple[y][x];
                if (!cell.abyss) {
                    // render the floor
                    if (cell.floorId != 0) {
                        //glActiveTexture(GL_TEXTURE0);
                        //glBindTexture(GL_TEXTURE_2D, textureIdMap[cell.floorId]);
                        //shader->setInt("material.diffuse", 0);

                        //glm::vec3 position = glm::vec3((float)x, (float)0, (float)y);
                        //glm::mat4 model = glm::mat4(1.0f);
                        //model = glm::translate(model, position);
                        //shader->setMat4("model", model);
                        //glDrawArrays(GL_TRIANGLES, cubeTop.meshStart, 6);
                        
                        glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y);
                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, position);
                        /*model = glm::rotate(model, angel, axis);*/
                        shader->setMat4("model", model);
                        modelBank[100]->draw(*shader);
                    }
                    // render the wall
                    if (cell.wallId != 0) {
                        //glActiveTexture(GL_TEXTURE0);
                        //glBindTexture(GL_TEXTURE_2D, textureIdMap[cell.wallId]);
                        //shader->setInt("material.diffuse", 0);


                        bool drawLeft = false;
                        bool drawRight = false;
                        bool drawFront = false;
                        bool drawBack = false;
                        if ((x > 0 && !g_gameState.worldArray_simple[y][x - 1].fullCell && !g_gameState.worldArray_simple[y][x - 1].abyss) || x == 0) {
                            drawLeft = true;
                        }
                        if ((y < g_gameState.worldYMax - 1 && !g_gameState.worldArray_simple[y + 1][x].fullCell && !g_gameState.worldArray_simple[y + 1][x].abyss) || y == g_gameState.worldYMax - 1) {
                            drawFront = true;
                        }
                        if ((y > 0 && !g_gameState.worldArray_simple[y - 1][x].fullCell && !g_gameState.worldArray_simple[y - 1][x].abyss) || y == 0) {
                            drawBack = true;
                        }
                        if ((x < g_gameState.worldXMax - 1 && !g_gameState.worldArray_simple[y][x + 1].fullCell && !g_gameState.worldArray_simple[y][x + 1].abyss) || x == g_gameState.worldXMax - 1) {
                            drawRight = true;
                        }
                        if (drawLeft) {
                            glm::vec3 position = glm::vec3((float)x - 0.5, (float)0.5f, (float)y);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                            shader->setMat4("model", model);
                            modelBank[101]->draw(*shader);
                        }
                        if (drawFront) {
                            glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y + 0.5f);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                            shader->setMat4("model", model);
                            modelBank[101]->draw(*shader);
                        }
                        if (drawRight) {
                            glm::vec3 position = glm::vec3((float)x + 0.5, (float)0.5f, (float)y);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                            shader->setMat4("model", model);
                            modelBank[101]->draw(*shader);
                            
                        }
                        if (drawBack) {
                            glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y-0.5f);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            shader->setMat4("model", model);
                            modelBank[101]->draw(*shader);
                        }
                    }

                    if (cell.ceilingId != 0) {
                    //    glActiveTexture(GL_TEXTURE0);
                    //    glBindTexture(GL_TEXTURE_2D, textureIdMap[cell.ceilingId]);
                    //    shader->setInt("material.diffuse", 0);

                    //    glm::vec3 position = glm::vec3((float)x, (float)2, (float)y);
                    //    glm::mat4 model = glm::mat4(1.0f);
                    //    model = glm::translate(model, position);
                    //    shader->setMat4("model", model);
                    //    glDrawArrays(GL_TRIANGLES, cubeBottom.meshStart, 6);
                        glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y);
                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, position);
                        /*model = glm::rotate(model, angel, axis);*/
                        shader->setMat4("model", model);
                        modelBank[102]->draw(*shader);
                    }
                }            
            }
        }
    }



    // render 3d entities
    for (auto entity3d : g_gameState.entityList) {
        glm::vec3 position = entity3d->position;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        /*model = glm::rotate(model, angel, axis);*/
        shader->setMat4("model", model);
        modelBank[entity3d->modelId]->draw(*shader);
    }

    // render player viewmodel

    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 cubeCameraModel = glm::mat4(1.0);
    cubeCameraModel = glm::translate(cubeCameraModel, glm::vec3(0.16f, -0.12f, -0.25f));
    cubeCameraModel = glm::rotate(cubeCameraModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cubeCameraModel = glm::rotate(cubeCameraModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cubeCameraModel = glm::rotate(cubeCameraModel, glm::radians(12.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    

    // Design idea: make this into a function that just returns a model matrix
    // or takes a model matrix as input and some other params and returns a matrix
    // then can be a function property of the equipped item - then specify different anims for different items?
    // e.g. spear thrust
    if (g_gameState.player->isAttacking) {
        float t = g_gameState.player->attackAnimTimer / g_gameState.player->attackAnimDuration;
        float lerp1 = 0 * (1 - t) + 85.0f * t;
        float lerp2 = 0 * (1 - t) + 30.0f * t;
        float lerp3 = 0 * (1 - t) + 0.225f * t;
        float lerp4 = 0 * (1 - t) + 0.05f * t;
        cubeCameraModel = glm::rotate(cubeCameraModel, glm::radians(lerp1), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeCameraModel = glm::rotate(cubeCameraModel, glm::radians(lerp2), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeCameraModel = glm::translate(cubeCameraModel, glm::vec3(0.0f, lerp3, 0.0f));
        cubeCameraModel = glm::translate(cubeCameraModel, glm::vec3(-lerp4, 0.0f, 0.0f));
    }

    shader->setMat4("view", view);
    shader->setMat4("model", glm::inverse(view)* cubeCameraModel);
    modelBank[g_gameState.player->equipped->modelId]->draw(*shader);
  
    renderGUI();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(g_window);
}