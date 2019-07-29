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
#include <Window.h>
#include <OpenGL.h>
#include <Font.h>
#include <Layout.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

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

    auto selectedCube = std::make_pair(-1, -1);
    Math::Vec3 motionDirection(static_cast<float>(x - mousePosition.first), static_cast<float>(y - mousePosition.second), 0.0f);
    mousePosition = this->getWindow()->getMousePosition();

    switch (this->state) {
        case GameState::RUNNING:
            if (mouseState[Graphene::MouseButton::BUTTON_LEFT] || mouseState[Graphene::MouseButton::BUTTON_RIGHT]) {
                // this->frameBuffer->bind();

                // float data[4];
                // glReadPixels(x, this->getWindow()->getHeight() - y, 1, 1, GL_RGBA, GL_FLOAT, data);

                // selectedCube = std::make_pair(roundf(data[0] * 100), roundf(data[1] * 100));
                // roundf(data[2] * 100);
            }

            if (mouseState[Graphene::MouseButton::BUTTON_LEFT] && !mouseState[Graphene::MouseButton::BUTTON_RIGHT]) {
                // if (selectedCube.get(Math::Vec3::Z) == 0.0f) {  // Front facet
                    this->rotateCube(selectedCube, motionDirection);
                // }
            } else if (mouseState[Graphene::MouseButton::BUTTON_RIGHT] && !mouseState[Graphene::MouseButton::BUTTON_LEFT]) {
                // if (Puzzle::isSelectionValid(selectedCube)) {
                    this->rotateCube(std::make_pair(-1, -1), motionDirection);
                // }
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

    auto scene = this->createScene();
    auto sceneRoot = scene->getRootNode();

    auto player = scene->createNode();
    auto cube = scene->createNode();
    this->puzzle = std::make_shared<Puzzle>();

    sceneRoot->attachNode(player);
    sceneRoot->attachNode(cube);

    /* Populate scene with objects */

    auto& objectManager = Graphene::GetObjectManager();

    auto background = objectManager.createEntity("assets/background.entity");
    background->translate(0.0f, 0.0f, 5.0f);
    background->scale(25.0f, 25.0f, 1.0f);
    sceneRoot->attachObject(background);

    auto camera = objectManager.createCamera(Graphene::ProjectionType::PERSPECTIVE);
    player->attachObject(camera);
    player->translate(0.0f, 0.0f, -5.0f);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                auto cubepart = scene->createNode();
                cube->attachNode(cubepart);

                auto entity = objectManager.createEntity("assets/cubepart.entity");
                entity->translate(static_cast<float>(i - 1), static_cast<float>(j - 1), static_cast<float>(k - 1));
                cubepart->attachObject(entity);

                int cubeId = i * 9 + j * 3 + k;
                this->puzzle->attachCube(cubepart, cubeId);
            }
        }
    }

    cube->rotate(Math::Vec3::UNIT_X, -25.0f);
    cube->rotate(Math::Vec3::UNIT_Y, -30.0f);
    this->puzzle->shuffle(this->shuffles);

    /* Update default viewport with camera */

    auto window = this->getWindow();
    auto viewport = window->createViewport(0, 0, window->getWidth(), window->getHeight());
    viewport->setCamera(camera);
}

void Rubik::setupUI() {
    /* Setup scene */

    auto uiScene = this->createScene();
    auto uiRootNode = uiScene->getRootNode();

    /* Populate scene with objects */

    auto& objectManager = Graphene::GetObjectManager();
    auto camera = objectManager.createCamera(Graphene::ProjectionType::ORTHOGRAPHIC);
    camera->setNearPlane(-1.0f);
    camera->setFarPlane(1.0f);

    auto font = std::make_shared<Graphene::Font>("fonts/dejavu-sans.ttf", 14);
    this->timeLabel = std::make_shared<Graphene::Label>(150, 20, font);
    this->movesLabel = std::make_shared<Graphene::Label>(150, 20, font);
    this->promptLabel = std::make_shared<Graphene::Label>(150, 20, font);

    uiRootNode->attachObject(camera);
    uiRootNode->attachObject(this->timeLabel);
    uiRootNode->attachObject(this->movesLabel);
    uiRootNode->attachObject(this->promptLabel);

    /* Arrange UI elements */

    auto window = this->getWindow();

    auto uiLayout = std::make_shared<Graphene::Layout>();
    uiLayout->addComponent(this->timeLabel, 10, window->getHeight() - 20);
    uiLayout->addComponent(this->movesLabel, 10, window->getHeight() - 40);
    uiLayout->addComponent(this->promptLabel, 10, 10);

    /* Update viewport with camera */

    auto overlay = window->createOverlay(0, 0, window->getWidth(), window->getHeight());
    overlay->setCamera(camera);
    overlay->setLayout(uiLayout);
}

void Rubik::updateScene() {
    const Graphene::KeyboardState& keyboardState = this->getWindow()->getKeyboardState();

    switch (this->state) {
        case GameState::RUNNING:
            if (this->puzzle->isCompleted()) {
                this->state = GameState::FINISHED;
            } else if (keyboardState[Graphene::KeyboardKey::KEY_ESCAPE]) {
                this->state = GameState::QUIT;
            } else if (keyboardState[Graphene::KeyboardKey::KEY_S]) {
                this->puzzle->selectCube(std::make_pair(std::rand() % 3, std::rand() % 3));
                this->puzzle->setAnimationState(static_cast<AnimationState>(std::rand() % 4 + 1));
            }

            this->puzzle->animate(this->getFrameTime());
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
    this->timeLabel->setText(time.str());

    std::wstringstream moves;
    moves << "Moves: " << this->moves;
    this->movesLabel->setText(moves.str());

    switch (this->state) {
        case GameState::FINISHED:
            this->promptLabel->setText(L"Done! New game? Y/N");
            break;

        case GameState::PAUSED:
            this->promptLabel->setText(L"Paused");
            break;

        case GameState::QUIT:
            this->promptLabel->setText(L"Quit? Y/N");
            break;

        default:
            this->promptLabel->setText(L"");
            break;
    }
}

void Rubik::rotateCube(const std::pair<int, int>& selectedCube, const Math::Vec3& direction) {
    if (this->state == GameState::PAUSED) {
        return;  // No action on pause
    }

    if (direction.length() < 1.5f) {
        return;  // Movement is too short
    }

    if (Puzzle::isSelectionValid(selectedCube) && this->puzzle->getAnimationState() == AnimationState::IDLE) {
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

    this->puzzle->selectCube(selectedCube);
    this->puzzle->setAnimationState(puzzleState);
}

}  // namespace Rubik
