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

#include <Rubik.h>
#include <ObjectManager.h>
#include <RenderManager.h>
#include <RenderState.h>
#include <Window.h>
#include <OpenGL.h>
#include <Shader.h>
#include <ObjectGroup.h>
#include <TextComponent.h>
#include <Layout.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

namespace Rubik {

Rubik::Rubik() {
    std::srand(static_cast<int>(std::time(nullptr)));
}

int Rubik::getShuffles() const {
    return this->shuffles;
}

void Rubik::setShuffles(int shuffles) {
    this->shuffles = shuffles;
}

void Rubik::onMouseMotion(int x, int y) {
    static Graphene::MousePosition mousePosition(this->getWindow()->getMousePosition());
    const Graphene::MouseState& mouseState = this->getWindow()->getMouseState();

    Math::Vec3 motionDirection(static_cast<float>(x - mousePosition.first), static_cast<float>(y - mousePosition.second), 0.0f);
    mousePosition = this->getWindow()->getMousePosition();

    int objectId = -1;
    bool isCubeSelected = false;
    bool isFrontCubeSelected = false;

    switch (this->state) {
        case GameState::RUNNING:
            if (mouseState[Graphene::MouseButton::BUTTON_LEFT] || mouseState[Graphene::MouseButton::BUTTON_RIGHT]) {
                int pickupX = x * this->pickupBuffer->getWidth() / this->getWindow()->getWidth();
                int pickupY = (this->getWindow()->getHeight() - y) * this->pickupBuffer->getHeight() / this->getWindow()->getHeight();
                this->pickupBuffer->getPixel(pickupX, pickupY, GL_RED_INTEGER, GL_INT, &objectId);

                isCubeSelected = (std::find(this->puzzleObjects.begin(), this->puzzleObjects.end(), objectId) != this->puzzleObjects.end());
                isFrontCubeSelected = (isCubeSelected && std::get<2>(this->puzzle->getCubePosition(objectId)) == 0);
            }

            if (mouseState[Graphene::MouseButton::BUTTON_LEFT] && !mouseState[Graphene::MouseButton::BUTTON_RIGHT]) {
                if (isFrontCubeSelected) {
                    this->rotateCube(objectId, motionDirection);
                }
            } else if (mouseState[Graphene::MouseButton::BUTTON_RIGHT] && !mouseState[Graphene::MouseButton::BUTTON_LEFT]) {
                if (isCubeSelected) {
                    this->rotateCube(-1, motionDirection);
                }
            }
            break;

        default:
            break;
    }
}

void Rubik::onKeyboardKey(Graphene::KeyboardKey key, bool state) {
    static bool pausePressed = false;
    static float rotationSpeed = 0.0f;

    switch (this->state) {
        case GameState::RUNNING:
            if (key == Graphene::KeyboardKey::KEY_S) {
                if (state) {
                    if (rotationSpeed == 0.0f) {
                        rotationSpeed = this->puzzle->getRotationSpeed();
                        this->puzzle->setRotationSpeed(rotationSpeed * 2.0f);
                    }
                } else {
                    this->puzzle->setRotationSpeed(rotationSpeed);
                    rotationSpeed = 0.0f;
                }
            }
            // Fall through

        case GameState::PAUSED:
            if (key == Graphene::KeyboardKey::KEY_P) {
                if (state) {
                    if (!pausePressed) {
                        this->state = (this->state == GameState::RUNNING) ? GameState::PAUSED : GameState::RUNNING;
                        pausePressed = true;
                    }
                } else {
                    pausePressed = false;
                }
            }
            break;

        default:
            break;
    }
}

void Rubik::onSetup() {
    Graphene::RenderStateCallback callback([](Graphene::RenderState* renderState, const std::shared_ptr<Graphene::Object>& object) {
        renderState->getShader()->setUniform("objectId", object->getId());
    });

    auto& renderState = Graphene::GetRenderManager().getRenderState(Graphene::RenderBuffer::ID);
    renderState->setShader(Graphene::GetObjectManager().createShader("shaders/object_pickup.shader"));
    renderState->setCallback(callback);

    this->setupScene();
    this->setupUI();
}

void Rubik::onIdle() {
    this->updateScene();
    this->updateUI();

    const Graphene::KeyboardState& keyboardState = this->getWindow()->getKeyboardState();
    if (this->state == GameState::RUNNING && !keyboardState[Graphene::KeyboardKey::KEY_S]) {
        this->gameTime += this->getFrameTime();
    }
}

void Rubik::setupScene() {
    /* Setup scene */

    auto& scene = this->createScene();
    scene->setAmbientEnergy(0.2f);

    auto& sceneRoot = scene->getRoot();
    auto& player = scene->getPlayer();
    this->puzzle = std::make_shared<Puzzle>();

    auto cube = std::make_shared<Graphene::ObjectGroup>();
    sceneRoot->addObject(cube);

    /* Populate scene with objects */

    auto& objectManager = Graphene::GetObjectManager();
    auto& renderManager = Graphene::GetRenderManager();
    renderManager.setLightPass(true);

    auto background = objectManager.createEntity("assets/background.entity");
    background->translate(0.0f, 0.0f, 5.0f);
    background->scale(25.0f, 25.0f, 1.0f);
    sceneRoot->addObject(background);

    auto camera = objectManager.createCamera(Graphene::ProjectionType::PERSPECTIVE);
    auto light = objectManager.createLight(Graphene::LightType::DIRECTED);

    player->addObject(camera);
    player->addObject(light);
    player->translate(0.25f, -0.25f, -4.5f);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                auto cubepart = std::make_shared<Graphene::ObjectGroup>();
                cube->addObject(cubepart);

                auto entity = objectManager.createEntity("assets/cubepart.entity");
                entity->translate(static_cast<float>(i), static_cast<float>(j), static_cast<float>(k));
                cubepart->addObject(entity);

                this->puzzle->addCube(entity);
                this->puzzleObjects.push_back(entity->getId());
            }
        }
    }

    cube->roll(-30.0f);
    cube->yaw(-30.0f);
    this->puzzle->shuffle(this->shuffles);

    /* Update default viewport with camera */

    auto& window = this->getWindow();
    auto& viewport = window->createViewport(0, 0, window->getWidth(), window->getHeight());
    viewport->setCamera(camera);

    /* Create framebuffer for object ID rendering and picking */

    this->pickupBuffer = this->createFrameBuffer(window->getWidth() / 2, window->getHeight() / 2, GL_R32I);
    auto& pickupViewport = pickupBuffer->createViewport(0, 0, this->pickupBuffer->getWidth(), this->pickupBuffer->getHeight());
    pickupViewport->setCamera(camera);
}

void Rubik::setupUI() {
    /* Setup scene */

    auto& uiScene = this->createScene();
    auto& uiRoot = uiScene->getRoot();

    /* Populate scene with objects */

    auto& objectManager = Graphene::GetObjectManager();
    auto camera = objectManager.createCamera(Graphene::ProjectionType::ORTHOGRAPHIC);
    camera->setNearPlane(-1.0f);
    camera->setFarPlane(1.0f);

    this->timeLabel = objectManager.createLabel(200, 20, "fonts/dejavu-sans.ttf", 14);
    this->movesLabel = objectManager.createLabel(200, 20, "fonts/dejavu-sans.ttf", 14);
    this->promptLabel = objectManager.createLabel(200, 20, "fonts/dejavu-sans.ttf", 14);

    uiRoot->addObject(camera);
    uiRoot->addObject(this->timeLabel);
    uiRoot->addObject(this->movesLabel);
    uiRoot->addObject(this->promptLabel);

    /* Arrange UI elements */

    auto& window = this->getWindow();

    auto uiLayout = std::make_shared<Graphene::Layout>();
    uiLayout->addComponent(this->timeLabel, 10, window->getHeight() - 20);
    uiLayout->addComponent(this->movesLabel, 10, window->getHeight() - 40);
    uiLayout->addComponent(this->promptLabel, 10, 10);

    /* Update viewport with camera */

    auto& overlay = window->createOverlay(0, 0, window->getWidth(), window->getHeight());
    overlay->setCamera(camera);
    overlay->setLayout(uiLayout);
}

void Rubik::updateScene() {
    const Graphene::KeyboardState& keyboardState = this->getWindow()->getKeyboardState();

    switch (this->state) {
        case GameState::RUNNING:
            if (this->puzzle->isSolved()) {
                this->state = GameState::FINISHED;
            } else if (keyboardState[Graphene::KeyboardKey::KEY_ESCAPE]) {
                this->state = GameState::QUIT;
            } else if (keyboardState[Graphene::KeyboardKey::KEY_S]) {
                this->puzzle->selectCube(this->puzzleObjects[std::rand() % this->puzzleObjects.size()]);
                this->puzzle->setAnimationState(static_cast<AnimationState>(std::rand() % 4 + 1));
            }

            this->puzzle->update(this->getFrameTime());
            break;

        case GameState::QUIT:
            if (keyboardState[Graphene::KeyboardKey::KEY_Y]) {
                this->exit(0);
            } else if (keyboardState[Graphene::KeyboardKey::KEY_N]) {
                this->state = GameState::RUNNING;
            }
            break;

        case GameState::FINISHED:
            if (keyboardState[Graphene::KeyboardKey::KEY_N]) {
                this->exit(0);
            } else if (keyboardState[Graphene::KeyboardKey::KEY_Y]) {
                this->moves = 0;
                this->gameTime = 0.0f;
                this->state = GameState::RUNNING;
                this->puzzle->shuffle(this->shuffles);
            }
            break;

        default:
            break;
    }
}

void Rubik::updateUI() {
    int gameTime = static_cast<int>(this->gameTime);
    int seconds = gameTime % 60;
    int minutes = gameTime / 60;
    int hours = gameTime / 3600;

    std::wstringstream time;
    time << "Time: " << std::setw(2) << std::setfill(L'0') << hours << ":"
                     << std::setw(2) << std::setfill(L'0') << minutes << ":"
                     << std::setw(2) << std::setfill(L'0') << seconds;
    this->timeLabel->getComponent<Graphene::TextComponent>()->setText(time.str());

    std::wstringstream moves;
    moves << "Moves: " << this->moves;
    this->movesLabel->getComponent<Graphene::TextComponent>()->setText(moves.str());

    switch (this->state) {
        case GameState::FINISHED:
            this->promptLabel->getComponent<Graphene::TextComponent>()->setText(L"Done! New game? Y/N");
            this->promptLabel->setVisible(true);
            break;

        case GameState::PAUSED:
            this->promptLabel->getComponent<Graphene::TextComponent>()->setText(L"Paused");
            this->promptLabel->setVisible(true);
            break;

        case GameState::QUIT:
            this->promptLabel->getComponent<Graphene::TextComponent>()->setText(L"Quit? Y/N");
            this->promptLabel->setVisible(true);
            break;

        default:
            this->promptLabel->setVisible(false);
            break;
    }
}

void Rubik::rotateCube(int objectId, const Math::Vec3& direction) {
    if (this->state == GameState::PAUSED) {
        return;  // No action on pause
    }

    if (direction.length() < 1.5f) {
        return;  // Movement is too short
    }

    if (objectId != -1 && this->puzzle->getAnimationState() == AnimationState::IDLE) {
        this->moves++;
    }

    float xDirection = direction.get(Math::Vec3::X);
    float yDirection = direction.get(Math::Vec3::Y);
    AnimationState puzzleState = AnimationState::IDLE;

    if (fabsf(xDirection) > fabsf(yDirection)) {
        puzzleState = (xDirection > 0) ? AnimationState::RIGHT_ROTATION : AnimationState::LEFT_ROTATION;
    } else {
        puzzleState = (yDirection > 0) ? AnimationState::DOWN_ROTATION : AnimationState::UP_ROTATION;
    }

    this->puzzle->selectCube(objectId);
    this->puzzle->setAnimationState(puzzleState);
}

}  // namespace Rubik
