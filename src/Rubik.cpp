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

#include <SDL2/SDL.h>
#include <cmath>

namespace Rubik {

Rubik::Rubik() {
    this->window = nullptr;
    this->context = nullptr;

    this->running = true;
    this->width = 800;
    this->height = 600;
    this->frameTime = 0.0f;
}

int Rubik::exec() {
    if (!this->setUp()) {
        this->tearDown();
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
                    if (this->mouseButtonStates[SDL_BUTTON_RIGHT]) {
                        this->rotateCube(Math::Vec3(event.motion.xrel, event.motion.yrel, 0.0f));
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    this->mouseButtonStates[event.button.button] = true;
                    break;

                case SDL_MOUSEBUTTONUP:
                    this->mouseButtonStates[event.button.button] = false;
                    break;

                case SDL_KEYDOWN:
                    this->keyboardButtonStates[event.key.keysym.scancode] = true;
                    break;

                case SDL_KEYUP:
                    this->keyboardButtonStates[event.key.keysym.scancode] = false;
                    break;
            }
        }

        this->update();
        this->render();

        this->frameTime = (SDL_GetTicks() - beginFrame) / 1000.0f;
    }

    this->tearDown();
    return ERROR_OK;
}

bool Rubik::setUp() {
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Setting up...");

    if (!this->initSDL() || !this->initOpenGL()) {
        return false;
    }

    this->camera = std::unique_ptr<Game::Camera>(new Game::Camera());
    this->camera->setAspectRatio(this->width / (this->height / 1.0f));
    this->camera->setFov(75.0f);
    this->camera->setPosition(-2.0f, 2.0f, -6.0f);
    this->camera->lookAt(2.0f, -2.0f, 6.0f);

    this->cube = std::unique_ptr<Game::Cube>(new Game::Cube());
    return true;
}

void Rubik::tearDown() {
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Tearing down...");

    if (this->context) {
        SDL_GL_DeleteContext(this->context);
    }

    if (this->window) {
        SDL_DestroyWindow(this->window);
    }

    IMG_Quit();
    SDL_Quit();
}

bool Rubik::initSDL() {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_Init() failed: %s", SDL_GetError());
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

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Initializing %d x %d viewport", this->width, this->height);
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

void Rubik::rotateCube(const Math::Vec3& direction) {
    if (fabsf(direction.get(Math::Vec3::X)) > fabsf(direction.get(Math::Vec3::Y))) {
        if (direction.get(Math::Vec3::X) > 0) {
            this->cube->setState(Game::Cube::CubeState::STATE_RIGHT_ROTATION);
        } else {
            this->cube->setState(Game::Cube::CubeState::STATE_LEFT_ROTATION);
        }
    } else {
        if (direction.get(Math::Vec3::Y) > 0) {
            this->cube->setState(Game::Cube::CubeState::STATE_UP_ROTATION);
        } else {
            this->cube->setState(Game::Cube::CubeState::STATE_DOWN_ROTATION);
        }
    }
}

void Rubik::update() {
    if (this->keyboardButtonStates[SDL_SCANCODE_ESCAPE]) {
        this->running = false;
    }

    this->cube->animate(this->frameTime);
}

void Rubik::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->cube->render(this->camera->getProjection() *
                       this->camera->getRotation() *
                       this->camera->getTranslation());

    SDL_GL_SwapWindow(this->window);
}

}  // namespace Rubik
