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
Shader* depthShader = NULL;
Shader* particleShader = NULL;
Shader* billboardShader = NULL;

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


//unsigned int depthCubemap;
//unsigned int depthMapFBO;
const unsigned int shadowWidth = 1024;
const unsigned int shadowHeight = 1024;

constexpr unsigned int numLights = 2;
unsigned int depthCubemapList[numLights];
unsigned int depthMapFBOList[numLights];

struct Particle {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.1f);
    glm::vec4 color = glm::vec4(1.0f);
    float life = 0.0f;
};

constexpr int maxParticles = 500;
Particle particles[maxParticles];
unsigned int particleTextureId;

float billboardQuad[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
};
unsigned int billboardVAO;
unsigned int billboardVBO;

void renderEnvironment(Shader* _shader);

void spawnParticle(Particle& particle, glm::vec3 position)
{
    float random = ((rand() % 100) - 50) / 100.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    float randX = ((rand() % 100) - 50) / 100.0f;
    float randY = ((rand() % 100) - 50) / 100.0f;
    float randZ = ((rand() % 100) - 50) / 100.0f;
    particle.position = glm::vec3(position.x + randX, position.y + randY, position.z + randZ);
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.0f;
    particle.velocity = glm::vec3(0.1f);
}

void spawnParticleSpherical(Particle& particle, glm::vec3 origin) {
    float maxR = 0.1;
    float r = ((rand() % 100) - 50) / 1000.0f;
    float phi = glm::radians(((rand() % 3600) - 1800) / 10.0f);
    float theta = glm::radians(((rand() % 3600) - 1800) / 10.0f);

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);
    particle.position = glm::vec3(origin.x + x, origin.y + y, origin.z + z);

    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.0f;
    particle.velocity = glm::vec3(0.1f);
}

void updateParticles(double dTime) {
    static const int maxNewParticlesPerFrame = 1;
    int spawnedParticlesThisFrame = 0;
    static int spawnedParticles = 0;
    
    glm::vec3 particleSpawnPos = glm::vec3(2.0f, 1.0f, 1.0f);

    for (int i = 0; i < maxParticles; ++i) {
        Particle& p = particles[i];
        if (p.life <= 0.0f) {
            if (spawnedParticles < maxParticles) {
                if (spawnedParticlesThisFrame < maxNewParticlesPerFrame) {
                    // can spawn a particle
                    spawnParticleSpherical(p, particleSpawnPos);
                    spawnedParticles += 1;
                    spawnedParticlesThisFrame += 1;
                }
            }
            else {
                spawnedParticles -= 1;
            }
        }
        else {
            p.life -= dTime;
            if (p.life > 0.0f) {
                p.position.y += p.velocity.y * (float)dTime;
                p.color.a -= dTime * 0.5f; // reduce the alpha over time
            }
        }
    }
}

bool initWindow(const char* windowTitle ) {

	SDL_Init(SDL_INIT_EVERYTHING);

	g_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	if (g_window == NULL) {
		std::cout << "FAILED TO INIT WINDOW" << std::endl;
		return false;
	}
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	

	g_context = SDL_GL_CreateContext(g_window);
    SDL_GL_MakeCurrent(g_window, g_context);

	
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
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_CaptureMouse(SDL_TRUE);


    std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout <<  glGetString(GL_VERSION) << std::endl;
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    printf("OpenGL version: %d.%d\n", major, minor);

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
    depthShader = new Shader("shaders/depthVert.glsl", "shaders/depthFrag.glsl", "shaders/depthGeom.glsl");
    particleShader = new Shader("shaders/particleVert.glsl", "shaders/particleFrag.glsl");
    billboardShader = new Shader("shaders/billboardVert.glsl", "shaders/billboardFrag.glsl");

    for (int i = 0; i < numLights; ++i) {
        glGenFramebuffers(1, &depthMapFBOList[i]);

        glGenTextures(1, &depthCubemapList[i]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapList[i]);
        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight,
                0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOList[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapList[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    particleTextureId = loadTexture("assets/textures/fire_particle.png");

    glGenVertexArrays(1, &billboardVAO);
    glGenBuffers(1, &billboardVBO);
    glBindVertexArray(billboardVAO);
    glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardQuad), billboardQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // should stride be 0?
    glBindVertexArray(0);


}

void exitRendering() {
    delete shader;
    delete depthShader;
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
        modelBank[modelId] = new Model(path.c_str());
    }
}

void renderGame() {

    updateParticles(g_gameState.dTime);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);



    //// Set the directional light
    //shader->setVec3("dirLight.direction", -0.3f, -1.0f, -1.0f);
    //shader->setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
    //shader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);

    // set the point light
    //static float t = 0.0f;
    //static const float tinc = 0.00025f;
    //static bool dir = true;
    //if (dir) {
    //    t += tinc;
    //    if (t >= 1.0) {
    //        dir = false;
    //    }
    //}
    //else {
    //    t -= tinc;
    //    if (t <= 0.0) {
    //        dir = true;
    //    }
    //}
    //float lerp = 1.0 * (1 - t) + 3.0f * t;
    
    
    glm::vec3 light1Pos = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 light2Pos = glm::vec3(2.0, 1.0, 1.0);
    glm::vec3 lightPositions[numLights] = { light1Pos, light2Pos };
    //glm::vec3 lightPos = g_gameState.player->position;


    //shader->setVec3("pointLights[1].position", 5.0f, 1.0f, 5.0f);
    //shader->setVec3("pointLights[1].ambient", 0.0f, 0.0f, 0.0f);
    //shader->setVec3("pointLights[1].diffuse", 0.5f, 0.5f, 0.5f);

    //shader->setFloat("pointLights[1].constant", 1.0f);
    //shader->setFloat("pointLights[1].linear", 0.09f);
    //shader->setFloat("pointLights[1].quadratic", 0.032f);


    // Setup depth cubemap transformation matricies

    float aspect = (float)shadowWidth / (float)shadowHeight;
    float near = 0.1f;
    float far = 25.0f;
    glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);

    for (int i = 0; i < numLights; ++i) {
        // just change this to an array since its always 6;
        glm::mat4 shadowTransforms[6];
        shadowTransforms[0] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[1] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[2] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shadowTransforms[3] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        shadowTransforms[4] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[5] = shadowProjection * glm::lookAt(lightPositions[i], lightPositions[i] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        // render the scene to the depth cubemap



        glViewport(0, 0, shadowWidth, shadowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOList[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader->use();
        for (int i = 0; i < 6; ++i) {
            depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        depthShader->setFloat("far_plane", far);
        depthShader->setVec3("lightPos", lightPositions[i]);
        glCullFace(GL_FRONT);
        renderEnvironment(depthShader);
        glCullFace(GL_BACK);
    }



    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render scene
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    shader->use();

    cameraFront = g_gameState.player->direction;

    glm::vec3 finalCameraPos = g_gameState.player->position;

    glm::mat4 view = glm::lookAt(finalCameraPos, finalCameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(g_gameState.cameraFov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 50.0f);
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec3("viewPos", finalCameraPos);
    shader->setFloat("far_plane", far);

    shader->setInt("pointLightCount", numLights);

    for (int i = 0; i < numLights; ++i) {
        shader->setVec3("pointLights[" + std::to_string(i) +  "].position", lightPositions[i]);
        shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.1f, 0.1f, 0.1f);
        shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.6f, 0.4f, 0.2f);

        shader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);

        shader->use();
        
        shader->setInt("depthMaps[" + std::to_string(i) + "]", i + 1);

        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapList[i]);
        
    }
    shader->setInt("material.diffuse", 0);

    renderEnvironment(shader);

    // render test billboard
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    billboardShader->use();
    billboardShader->setMat4("view", view);
    billboardShader->setMat4("projection", projection);
    billboardShader->setVec3("CameraRight_worldspace", view[0][0], view[1][0], view[2][0]);
    billboardShader->setVec3("CameraUp_worldspace", view[0][1], view[1][1], view[2][1]);
    billboardShader->setFloat("scale", 0.006f);
    billboardShader->setInt("sprite", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, particleTextureId);
    glBindVertexArray(billboardVAO);
    for (int i = 0; i < maxParticles; ++i) {
        Particle& p = particles[i];
        billboardShader->setVec3("position", p.position);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    
    // render player viewmodel
    shader->use();
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


    //glm::vec3 cameraPosition(glm::inverse(view)[3]); // Do i need to do this - can I just use the pos directly. print to check



    //// Render particles?
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //particleShader->use();
    //particleShader->setMat4("view", view);
    //particleShader->setMat4("projection", projection);
    //
    //for (int i = 0; i < maxParticles; ++i) {
    //    Particle& p = particles[i];
    //    if (p.life > 0.0f) {
    //        particleShader->setVec3("offset", p.position);
    //        particleShader->setVec4("color", p.color);
    //        particleShader->setInt("sprite", 0);
    //        glActiveTexture(GL_TEXTURE0);
    //        glBindTexture(GL_TEXTURE_2D, particleTextureId);

    //        glBindVertexArray(particleVAO);
    //        glDrawArrays(GL_TRIANGLES, 0, 6);
    //        glBindVertexArray(0);
    //    }
    //}
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    renderGUI();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(g_window);
}

void renderEnvironment(Shader* _shader) {

    // Render the tiles
    int tilesOnScreenX = 50;
    int tilesOnScreenY = 50;


    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE); // ?

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
                        _shader->setMat4("model", model);
                        modelBank[100]->draw(*_shader);
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
                            _shader->setMat4("model", model);
                            modelBank[101]->draw(*_shader);
                        }
                        if (drawFront) {
                            glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y + 0.5f);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                            _shader->setMat4("model", model);
                            modelBank[101]->draw(*_shader);
                        }
                        if (drawRight) {
                            glm::vec3 position = glm::vec3((float)x + 0.5, (float)0.5f, (float)y);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                            _shader->setMat4("model", model);
                            modelBank[101]->draw(*_shader);

                        }
                        if (drawBack) {
                            glm::vec3 position = glm::vec3((float)x, (float)0.5f, (float)y - 0.5f);
                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, position);
                            _shader->setMat4("model", model);
                            modelBank[101]->draw(*_shader);
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
                        _shader->setMat4("model", model);
                        modelBank[102]->draw(*_shader);
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
        _shader->setMat4("model", model);
        modelBank[entity3d->modelId]->draw(*_shader);
    }
}