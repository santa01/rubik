/*
 * Copyright (c) 2013 Pavlo Lavrenenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Rubik.h"
#include "Logger.h"
#include "Vec3.h"
#include "ResourceManager.h"

#include <fontconfig/fontconfig.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace Rubik {

Rubik::Rubik() {
    this->window = nullptr;
    this->context = nullptr;

    this->width = 800;
    this->height = 600;
    this->movesCounter = 0;
    this->shuffles = 20;
    this->frameTime = 0.0f;

    this->running = true;
    this->state = STATE_RUNNING;

    for (auto& buttonState: this->mouseButtonStates) {
        buttonState = SDL_RELEASED;
    }

    for (auto& keyState: this->keyboardButtonStates) {
        keyState = SDL_RELEASED;
    }
}

int Rubik::exec() {
    if (!this->initialize()) {
        this->shutdown();
        return ERROR_SETUP;
    }

    SDL_Event event;
    while (this->running) {
        unsigned int beginFrame = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    this->running = false;
                    break;

                case SDL_MOUSEMOTION:
                    this->onMouseMotionEvent(event.motion);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    this->mouseButtonStates[event.button.button] = event.button.state;
                    this->onMouseButtonEvent(event.button);
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    this->keyboardButtonStates[event.key.keysym.scancode] = event.key.state;
                    this->onKeyboardEvent(event.key);
                    break;
            }
        }

        this->update();
        this->render();

        this->frameTime = (SDL_GetTicks() - beginFrame) / 1000.0f;
    }

    this->shutdown();
    return ERROR_OK;
}

bool Rubik::initialize() {
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Initializing...");

    if (!this->initSDL() || !this->initOpenGL() || !this->initFontConfig()) {
        return false;
    }

    this->camera.setAspectRatio(this->width / (this->height / 1.0f));
    this->camera.setFov(75.0f);
    this->camera.setPosition(-2.0f, 2.0f, -6.0f);
    this->camera.lookAt(2.0f, -2.0f, 6.0f);

    this->cube = std::unique_ptr<Game::Cube>(new Game::Cube());
    this->cube->setTexture(Utils::ResourceManager::getInstance().makeTexture("textures/subcube.png"));
    this->cube->shuffle(this->shuffles);

    this->defaultEffect = Utils::ResourceManager::getInstance().makeEffect("shaders/default.shader");
    this->defaultEffect->setUniform("textureSampler", 0);

    this->pickupEffect = Utils::ResourceManager::getInstance().makeEffect("shaders/pickup.shader");
    this->frameBuffer = std::unique_ptr<Opengl::FrameBuffer>(new Opengl::FrameBuffer(this->width, this->height));

    return true;
}

void Rubik::shutdown() {
    this->frameBuffer.reset();
    this->pickupEffect.reset();
    this->defaultEffect.reset();
    this->cube.reset();

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Cleaning caches...");
    Utils::ResourceManager::getInstance().purgeTextureCache();
    Utils::ResourceManager::getInstance().purgeEffectCache();

    if (this->context) {
        SDL_GL_DeleteContext(this->context);
    }

    if (this->defaultFont) {
        TTF_CloseFont(this->defaultFont);
    }

    if (this->window) {
        SDL_DestroyWindow(this->window);
    }

    FcFini();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Shutting down...");
}

bool Rubik::initSDL() {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_Init() failed: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init()) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "TTF_Init() failed: %s", TTF_GetError());
        return false;
    }

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "IMG_Init() failed: %s", IMG_GetError());
        return false;
    }

    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "SDL version: %d.%d.%d",
            sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    const SDL_version *sdlTtfVersion = TTF_Linked_Version();
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "SDL_ttf version: %d.%d.%d",
            sdlTtfVersion->major, sdlTtfVersion->minor, sdlTtfVersion->patch);

    const SDL_version *sdlImageVersion = IMG_Linked_Version();
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "SDL_image version: %d.%d.%d",
            sdlImageVersion->major, sdlImageVersion->minor, sdlImageVersion->patch);

    return true;
}

bool Rubik::initOpenGL() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Initializing %d x %d viewport",
            this->width, this->height);
    this->window = SDL_CreateWindow("Rubik's Cube", 0, 0, this->width, this->height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!this->window) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_CreateWindow() failed: %s", SDL_GetError());
        return false;
    }

    this->context = SDL_GL_CreateContext(this->window);
    if (!this->context) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_GL_CreateContext() failed: %s", SDL_GetError());
        return false;
    }

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "OpenGL vendor: %s", glGetString(GL_VENDOR));
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "OpenGL version: %s", glGetString(GL_VERSION));

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    return true;
}

bool Rubik::initFontConfig() {
    if (!FcInit()) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "FcInit() failed");
        return false;
    }

    int fcVersion = FcGetVersion();
    int fcMajor = fcVersion / 10000;
    int fcMinor = (fcVersion - fcMajor * 10000) / 100;
    int fcRevision = fcVersion - fcMajor * 10000 - fcMinor * 100;
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Fontconfig version: %d.%d.%d",
            fcMajor, fcMinor, fcRevision);

    FcPattern* pattern = FcNameParse((const FcChar8*)"sans");
    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult result;
    FcPattern* match = FcFontMatch(nullptr, pattern, &result);
    FcPatternDestroy(pattern);

    if (!match) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "FcFontMatch() failed: no `sans' font found");
        return false;
    }

    FcChar8* fontPath = FcPatternFormat(match, (const FcChar8*)"%{file}");
    FcChar8* fontName = FcPatternFormat(match, (const FcChar8*)"%{family}");

    this->defaultFont = TTF_OpenFont((const char*)fontPath, 12);
    if (!this->defaultFont) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "TTF_OpenFont failed: %s", TTF_GetError());
        return false;
    }

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Using `%s' family font", fontName);

    FcStrFree(fontName);
    FcStrFree(fontPath);
    FcPatternDestroy(match);

    return true;
}

void Rubik::onMouseMotionEvent(SDL_MouseMotionEvent& event) {
    Math::Vec3 selection(Game::Cube::DUMMY_SELECTION);

    switch (this->state) {
        case STATE_RUNNING:
            if (this->mouseButtonStates[SDL_BUTTON_LEFT] == SDL_PRESSED ||
                    this->mouseButtonStates[SDL_BUTTON_RIGHT] == SDL_PRESSED) {
                this->frameBuffer->bind();

                float data[4];
                glReadPixels(event.x, this->height - event.y, 1, 1, GL_RGBA, GL_FLOAT, data);
                selection = Math::Vec3(roundf(data[0] * 100.0f), roundf(data[1] * 100.0f), roundf(data[2] * 100.0f));
            }

            if (this->mouseButtonStates[SDL_BUTTON_LEFT] == SDL_PRESSED &&
                    this->mouseButtonStates[SDL_BUTTON_RIGHT] == SDL_RELEASED) {
                if (selection.get(Math::Vec3::Z) == 0.0f) {  // Front facet
                    this->rotateCube(Math::Vec3(event.xrel, event.yrel, 0.0f), selection);
                    this->movesCounter++;
                }
            } else if (this->mouseButtonStates[SDL_BUTTON_RIGHT] == SDL_PRESSED &&
                    this->mouseButtonStates[SDL_BUTTON_LEFT] == SDL_RELEASED) {
                if (selection != Game::Cube::DUMMY_SELECTION) {
                    this->rotateCube(Math::Vec3(event.xrel, event.yrel, 0.0f), Game::Cube::DUMMY_SELECTION);
                }
            }

            break;

        default:
            break;
    }
}

void Rubik::onMouseButtonEvent(SDL_MouseButtonEvent& event) {
}

void Rubik::onKeyboardEvent(SDL_KeyboardEvent& event) {
    static bool pauseButtonReleased = true;
    static float defaultRotationSpeed = 0.0f;

    switch (this->state) {
        case STATE_RUNNING:
            if (event.keysym.scancode == SDL_SCANCODE_S) {
                if (event.state == SDL_PRESSED) {
                    if (defaultRotationSpeed == 0.0f) {
                        std::srand(std::time(0));
                        defaultRotationSpeed = this->cube->getRotationSpeed();
                        this->cube->setRotationSpeed(500.0f);
                    }
                } else {
                    this->cube->setRotationSpeed(defaultRotationSpeed);
                    defaultRotationSpeed = 0.0f;
                }
            }
            // Fall through

        case STATE_PAUSED:
            if (event.keysym.scancode == SDL_SCANCODE_P) {
                if (event.state == SDL_PRESSED) {
                    if (pauseButtonReleased) {
                        this->state = (this->state == STATE_RUNNING) ? STATE_PAUSED : STATE_RUNNING;
                        pauseButtonReleased = false;
                    }
                } else {
                    pauseButtonReleased = true;
                }
            }
            break;

        default:
            break;
    }
}

void Rubik::rotateCube(const Math::Vec3& direction, const Math::Vec3& position) {
    if (direction.length() < 1.5f || this->state == STATE_PAUSED) {
        return;
    }

    this->cube->selectSubCube(position);

    if (fabsf(direction.get(Math::Vec3::X)) > fabsf(direction.get(Math::Vec3::Y))) {
        if (direction.get(Math::Vec3::X) > 0) {
            this->cube->setState(Game::Cube::CubeState::STATE_RIGHT_ROTATION);
        } else {
            this->cube->setState(Game::Cube::CubeState::STATE_LEFT_ROTATION);
        }
    } else {
        if (direction.get(Math::Vec3::Y) > 0) {
            this->cube->setState(Game::Cube::CubeState::STATE_DOWN_ROTATION);
        } else {
            this->cube->setState(Game::Cube::CubeState::STATE_UP_ROTATION);
        }
    }
}

void Rubik::update() {
    switch (this->state) {
        case STATE_RUNNING:
            if (this->keyboardButtonStates[SDL_SCANCODE_S]) {
                int row = std::rand() / (RAND_MAX / 1.0f) * 3;
                int column = std::rand() / (RAND_MAX / 1.0f) * 3;

                this->cube->selectSubCube(Math::Vec3(row, column, 0.0f));
                this->cube->setState(static_cast<Game::Cube::CubeState>(std::rand() / (RAND_MAX / 1.0f) * 4 + 1));
            }

            if (this->keyboardButtonStates[SDL_SCANCODE_ESCAPE]) {
                this->state = STATE_QUIT;
            }

            if (this->cube->inOrder()) {
                this->state = STATE_FINISHED;
            }

            this->cube->animate(this->frameTime);
            break;

        case STATE_QUIT:
            if (this->keyboardButtonStates[SDL_SCANCODE_Y]) {
                this->running = false;
            } else if (this->keyboardButtonStates[SDL_SCANCODE_N]) {
                this->state = STATE_RUNNING;
            }
            break;

        case STATE_FINISHED:
            if (this->keyboardButtonStates[SDL_SCANCODE_N]) {
                this->running = false;
            } else if (this->keyboardButtonStates[SDL_SCANCODE_Y]) {
                this->cube->shuffle(this->shuffles);
                this->state = STATE_RUNNING;
            }
            break;

        default:
            break;
    }
}

void Rubik::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->defaultEffect != nullptr) {
        this->defaultEffect->setUniform("mvp", this->camera.getProjection() *
                                               this->camera.getRotation() *
                                               this->camera.getTranslation());
        this->cube->setEffect(this->defaultEffect);
        this->cube->render();
    }

    this->frameBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->pickupEffect != nullptr) {
        this->pickupEffect->setUniform("mvp", this->camera.getProjection() *
                                              this->camera.getRotation() *
                                              this->camera.getTranslation());
        this->cube->setEffect(this->pickupEffect);
        this->cube->render();
    }

    SDL_GL_SwapWindow(this->window);
}

}  // namespace Rubik
