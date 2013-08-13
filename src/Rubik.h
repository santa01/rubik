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

#ifndef RUBIK_H
#define RUBIK_H

#include "Camera.h"
#include "Vec3.h"
#include "Cube.h"
#include "TextLabel.h"
#include "NonCopyable.h"
#include "FrameBuffer.h"
#include "ArgumentParser.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_scancode.h>
#include <memory>
#include <vector>

namespace Rubik {

class Rubik: public Common::NonCopyable {
public:
    Rubik(int argc, char** argv);
    int exec();

private:
    enum {
        ERROR_OK,
        ERROR_SETUP
    };

    enum GameState {
        STATE_RUNNING,
        STATE_PAUSED,
        STATE_QUIT,
        STATE_FINISHED
    };

    bool initialize();
    void shutdown();

    bool parseCLI();
    bool initSDL();
    bool initOpenGL();

    void onMouseMotionEvent(SDL_MouseMotionEvent& event);
    void onMouseButtonEvent(SDL_MouseButtonEvent& event);
    void onKeyboardEvent(SDL_KeyboardEvent& event);

    void rotateCube(const Math::Vec3& direction, const Math::Vec3& position);

    void updateScene();
    void updateUI();
    void render();

    SDL_Window* window;
    SDL_GLContext context;

    std::unique_ptr<Game::Cube> cube;
    std::unique_ptr<Game::TextLabel> timeLabel;
    std::unique_ptr<Game::TextLabel> movesLabel;
    std::unique_ptr<Game::TextLabel> promptLabel;

    std::unique_ptr<Opengl::FrameBuffer> frameBuffer;
    std::shared_ptr<Opengl::RenderEffect> defaultEffect;
    std::shared_ptr<Opengl::RenderEffect> pickupEffect;

    Utils::ArgumentParser arguments;
    Game::Camera camera;

    int argc;
    char** argv;

    int width;
    int height;
    float fov;
    float maxFps;
    int shuffles;
    bool vsync;

    int movesCounter;
    float frameTime;
    float gameTime;

    bool running;
    GameState gameState;

    Uint8 mouseButtonStates[SDL_BUTTON_X2 + 1];
    Uint8 keyboardButtonStates[SDL_NUM_SCANCODES];
};

}  // namespace Rubik

#endif  // RUBIK_H
