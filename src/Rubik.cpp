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
#include "ResourceManager.h"
#include "Config.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <Mat4.h>
#include <Vec4.h>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>

namespace Rubik {

Rubik::Rubik(int argc, char** argv) {
    this->window = nullptr;
    this->context = nullptr;
    this->argc = argc;
    this->argv = argv;

    this->movesCounter = 0;
    this->frameTime = 0.0f;
    this->gameTime = 0.0f;

    this->running = true;
    this->gameState = STATE_RUNNING;

    for (auto& buttonState: this->mouseButtonStates) {
        buttonState = SDL_RELEASED;
    }

    for (auto& keyState: this->keyboardButtonStates) {
        keyState = SDL_RELEASED;
    }
}

int Rubik::exec() {
    if (!this->parseCLI()) {
        return ERROR_SETUP;
    }

    if (this->arguments.isSet("help") || this->arguments.isSet("version")) {
        return ERROR_OK;
    }

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

        this->updateScene();
        this->updateUI();
        this->render();

        this->frameTime = (SDL_GetTicks() - beginFrame) / 1000.0f;
        float maxFrameTime = 1.0f / this->maxFps;

        if (this->frameTime < maxFrameTime) {
            SDL_Delay((maxFrameTime - this->frameTime) * 1000);
            this->frameTime = maxFrameTime;
        }

        if (this->gameState == STATE_RUNNING && this->keyboardButtonStates[SDL_SCANCODE_S] == SDL_RELEASED) {
            this->gameTime += this->frameTime;
        }
    }

    this->shutdown();
    return ERROR_OK;
}

bool Rubik::initialize() {
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Initializing...");

    if (!this->initSDL() || !this->initOpenGL()) {
        return false;
    }

    this->camera.setAspectRatio(this->width / (this->height / 1.0f));
    this->camera.setFov(this->fov);
    this->camera.setPosition(-1.8f, 1.8f, -4.0f);
    this->camera.lookAt(2.0f, -2.2f, 4.0f);

    this->frameBuffer = std::unique_ptr<Opengl::FrameBuffer>(new Opengl::FrameBuffer(this->width, this->height));

    this->defaultEffect = Utils::ResourceManager::getInstance().makeEffect(RUBIK_DATADIR "/shaders/default.shader");
    this->pickupEffect = Utils::ResourceManager::getInstance().makeEffect(RUBIK_DATADIR "/shaders/pickup.shader");
    if (this->defaultEffect == nullptr || this->pickupEffect == nullptr) {
        return false;
    }

    this->defaultEffect->setUniform("textureSampler", 0);

    this->cube = std::unique_ptr<Game::Cube>(new Game::Cube());
    if (!this->cube->load(Utils::ResourceManager::getInstance().makeMesh(RUBIK_DATADIR "/meshes/cube.mesh"))) {
        return false;
    }

    this->timeLabel = std::unique_ptr<Game::Label>(new Game::Label());
    if (!this->timeLabel->load(Utils::ResourceManager::getInstance().makeMesh(RUBIK_DATADIR "/meshes/quad.mesh"))) {
        return false;
    }

    this->movesLabel = std::unique_ptr<Game::Label>(new Game::Label());
    if (!this->movesLabel->load(Utils::ResourceManager::getInstance().makeMesh(RUBIK_DATADIR "/meshes/quad.mesh"))) {
        return false;
    }

    this->promptLabel = std::unique_ptr<Game::Label>(new Game::Label());
    if (!this->promptLabel->load(Utils::ResourceManager::getInstance().makeMesh(RUBIK_DATADIR "/meshes/quad.mesh"))) {
        return false;
    }

    auto texture = Utils::ResourceManager::getInstance().makeTexture(RUBIK_DATADIR "/textures/cubepart.png");
    if (texture == nullptr) {
        return false;
    }

    auto font = Utils::ResourceManager::getInstance().makeFont(RUBIK_DATADIR "/fonts/dejavu-sans.ttf", 14);
    if (font == nullptr) {
        return false;
    }

    this->cube->setTexture(texture);
    this->cube->shuffle(this->shuffles);

    this->timeLabel->setEffect(this->defaultEffect);
    this->timeLabel->setFont(font);

    this->movesLabel->setEffect(this->defaultEffect);
    this->movesLabel->setFont(font);

    this->promptLabel->setEffect(this->defaultEffect);
    this->promptLabel->setFont(font);

    Math::Mat4 ndc;
    ndc.set(0, 0, 2.0f / (this->width / 1.0f));
    ndc.set(0, 3, -1.0f);
    ndc.set(1, 1, -2.0f / (this->height / 1.0f));
    ndc.set(1, 3, 1.0f);

    this->camera.setProjectionType(Game::Camera::TYPE_ORTHOGRAPHIC);
    this->camera.setNearPlane(-8.0f);
    this->camera.setFarPlane(8.0f);

    Math::Mat4 world(this->camera.getProjection());
    world.invert();

    Math::Vec4 timePosition = (world * ndc) * Math::Vec4(20.0f, 30.0f, 0.0f, 1.0f);
    this->timeLabel->setPosition(timePosition.extractVec3());
    this->timeLabel->setProjection(this->camera.getProjection());

    Math::Vec4 movesPosition = (world * ndc) * Math::Vec4(20.0f, 50.0f, 0.0f, 1.0f);
    this->movesLabel->setPosition(movesPosition.extractVec3());
    this->movesLabel->setProjection(this->camera.getProjection());

    Math::Vec4 promptPosition = (world * ndc) * Math::Vec4(20.0f, this->height - 20.0f, 0.0f, 1.0f);
    this->promptLabel->setPosition(promptPosition.extractVec3());
    this->promptLabel->setProjection(this->camera.getProjection());

    return true;
}

void Rubik::shutdown() {
    this->timeLabel.reset();
    this->movesLabel.reset();
    this->promptLabel.reset();
    this->cube.reset();

    this->frameBuffer.reset();
    this->pickupEffect.reset();
    this->defaultEffect.reset();

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Cleaning caches...");
    Utils::ResourceManager::getInstance().purgeCaches();

    if (this->context) {
        SDL_GL_DeleteContext(this->context);
    }

    if (this->window) {
        SDL_DestroyWindow(this->window);
    }

    IMG_Quit();
    SDL_Quit();

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Shutting down...");
}

bool Rubik::parseCLI() {
    this->arguments.addArgument('v', "vsync", "vertical sync",
            Utils::ArgumentParser::ArgumentType::TYPE_BOOL);
    this->arguments.addArgument('s', "shuffles", "initial cube shuffles",
            Utils::ArgumentParser::ArgumentType::TYPE_INT);
    this->arguments.addArgument('S', "samples", "MSAA samples",
            Utils::ArgumentParser::ArgumentType::TYPE_INT);
    this->arguments.addArgument('F', "fps", "maximum fps limit",
            Utils::ArgumentParser::ArgumentType::TYPE_FLOAT);
    this->arguments.addArgument('f', "fov", "camera field of view",
            Utils::ArgumentParser::ArgumentType::TYPE_FLOAT);
    this->arguments.addArgument('h', "height", "viewport height",
            Utils::ArgumentParser::ArgumentType::TYPE_INT);
    this->arguments.addArgument('w', "width", "viewport width",
            Utils::ArgumentParser::ArgumentType::TYPE_INT);

    this->arguments.setDescription(RUBIK_DESCRIPTION);
    this->arguments.setVersion(RUBIK_VERSION);

    if (!this->arguments.parse(this->argc, this->argv)) {
        return false;
    }

    this->vsync = this->arguments.isSet("vsync");
    this->shuffles = this->arguments.isSet("shuffles") ? atoi(this->arguments.getOption("shuffles").c_str()) : 20;
    this->samples = this->arguments.isSet("samples") ? atoi(this->arguments.getOption("samples").c_str()) : 0;
    this->maxFps = this->arguments.isSet("fps") ? atof(this->arguments.getOption("fps").c_str()) : 100.0f;
    this->fov = this->arguments.isSet("fov") ? atof(this->arguments.getOption("fov").c_str()) : 75.0f;
    this->height = this->arguments.isSet("height") ? atoi(this->arguments.getOption("height").c_str()) : 480;
    this->width = this->arguments.isSet("width") ? atoi(this->arguments.getOption("width").c_str()) : 640;

    return true;
}

bool Rubik::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)) {
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

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (this->samples > 0) ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, this->samples);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "Initializing %d x %d viewport",
            this->width, this->height);
    this->window = SDL_CreateWindow("Rubik's Cube", 0, 0, this->width, this->height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (this->window == nullptr) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_CreateWindow() failed: %s", SDL_GetError());
        return false;
    }

    this->context = SDL_GL_CreateContext(this->window);
    if (this->context == nullptr) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "SDL_GL_CreateContext() failed: %s", SDL_GetError());
        return false;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "glewInit() failed: %s",
                glewGetErrorString(glewError));
        return false;
    }

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "GLEW version: %s", glewGetString(GLEW_VERSION));

    if (this->vsync && SDL_GL_SetSwapInterval(1)) {
        Utils::Logger::getInstance().log(Utils::Logger::LOG_WARNING, "SDL_GL_SetSwapInterval() failed: %s",
                SDL_GetError());
    }

    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "OpenGL vendor: %s", glGetString(GL_VENDOR));
    Utils::Logger::getInstance().log(Utils::Logger::LOG_INFO, "OpenGL version: %s", glGetString(GL_VERSION));

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.8f, 0.9f, 0.8f, 1.0f);
    glViewport(0, 0, this->width, this->height);

    return true;
}

void Rubik::onMouseMotionEvent(SDL_MouseMotionEvent& event) {
    Math::Vec3 selection(Game::Cube::DUMMY_SELECTION);

    switch (this->gameState) {
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

void Rubik::onMouseButtonEvent(SDL_MouseButtonEvent& /*event*/) {
}

void Rubik::onKeyboardEvent(SDL_KeyboardEvent& event) {
    static bool pauseButtonReleased = true;
    static float defaultRotationSpeed = 0.0f;

    switch (this->gameState) {
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
                        this->gameState = (this->gameState == STATE_RUNNING) ? STATE_PAUSED : STATE_RUNNING;
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
    if (direction.length() < 1.5f || this->gameState == STATE_PAUSED) {
        return;
    }

    if (this->cube->getState() == Game::Cube::STATE_IDLE && position != Game::Cube::DUMMY_SELECTION) {
        this->movesCounter++;
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

void Rubik::updateScene() {
    switch (this->gameState) {
        case STATE_RUNNING:
            if (this->keyboardButtonStates[SDL_SCANCODE_S]) {
                int row = std::rand() / (RAND_MAX / 1.0f) * 3;
                int column = std::rand() / (RAND_MAX / 1.0f) * 3;

                this->cube->selectSubCube(Math::Vec3(row, column, 0.0f));
                this->cube->setState(static_cast<Game::Cube::CubeState>(std::rand() / (RAND_MAX / 1.0f) * 4 + 1));
            }

            if (this->keyboardButtonStates[SDL_SCANCODE_ESCAPE]) {
                this->gameState = STATE_QUIT;
            }

            if (this->cube->isCompleted()) {
                this->gameState = STATE_FINISHED;
            }

            this->cube->animate(this->frameTime);
            break;

        case STATE_QUIT:
            if (this->keyboardButtonStates[SDL_SCANCODE_Y]) {
                this->running = false;
            } else if (this->keyboardButtonStates[SDL_SCANCODE_N]) {
                this->gameState = STATE_RUNNING;
            }
            break;

        case STATE_FINISHED:
            if (this->keyboardButtonStates[SDL_SCANCODE_N]) {
                this->running = false;
            } else if (this->keyboardButtonStates[SDL_SCANCODE_Y]) {
                this->cube->shuffle(this->shuffles);
                this->gameTime = 0.0f;
                this->movesCounter = 0;
                this->gameState = STATE_RUNNING;
            }
            break;

        default:
            break;
    }
}

void Rubik::updateUI() {
    int totalTime = static_cast<int>(this->gameTime);
    int seconds = totalTime % 60;
    int minutes = totalTime / 60;
    int hours = totalTime / 3600;

    std::stringstream time;
    time << "Time: " << std::setw(2) << std::setfill('0') << hours << ":"
                     << std::setw(2) << std::setfill('0') << minutes << ":"
                     << std::setw(2) << std::setfill('0') << seconds;
    this->timeLabel->setText(time.str());

    std::stringstream moves;
    moves << "Moves: " << this->movesCounter;
    this->movesLabel->setText(moves.str());

    switch (this->gameState) {
        case STATE_FINISHED:
            this->promptLabel->setText("Done! New game? Y/N");
            break;

        case STATE_PAUSED:
            this->promptLabel->setText("Paused");
            break;

        case STATE_QUIT:
            this->promptLabel->setText("Quit? Y/N");
            break;

        default:
            this->promptLabel->setText("");
    }
}

void Rubik::render() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->camera.setProjectionType(Game::Camera::TYPE_PERSPECTIVE);
    this->camera.setNearPlane(0.1f);
    this->camera.setFarPlane(20.0f);

    this->defaultEffect->setUniform("mvp", this->camera.getProjection() *
                                           this->camera.getRotation() *
                                           this->camera.getTranslation());
    this->cube->setEffect(this->defaultEffect);
    this->cube->render();

    this->frameBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->pickupEffect->setUniform("mvp", this->camera.getProjection() *
                                          this->camera.getRotation() *
                                          this->camera.getTranslation());
    this->cube->setEffect(this->pickupEffect);
    this->cube->render();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->camera.setProjectionType(Game::Camera::TYPE_ORTHOGRAPHIC);
    this->camera.setNearPlane(-8.0f);
    this->camera.setFarPlane(8.0f);

    this->defaultEffect->setUniform("mvp", this->camera.getProjection());
    this->timeLabel->render();
    this->movesLabel->render();
    this->promptLabel->render();

    SDL_GL_SwapWindow(this->window);
}

}  // namespace Rubik
