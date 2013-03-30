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

#ifndef POLANDBALL_H
#define POLANDBALL_H

#include "Camera.h"
#include "Vec3.h"
#include "Cube.h"
#include "NonCopyable.h"
#include "FrameBuffer.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_scancode.h>
#include <memory>
#include <vector>

namespace Rubik {

class Rubik: public Common::NonCopyable {
public:
    Rubik();
    int exec();

private:
    enum {
        ERROR_OK,
        ERROR_SETUP
    };

    bool initialize();
    void shutdown();

    bool initSDL();
    bool initOpenGL();

    Math::Vec3 pickSubCube(const Math::Vec3& position) {
        this->frameBuffer->bind();

        float data[4];
        glReadPixels(position.get(Math::Vec3::X), this->height - position.get(Math::Vec3::Y),
                1, 1, GL_RGBA, GL_FLOAT, data);

        return Math::Vec3(roundf(data[0] * 100.0f), roundf(data[1] * 100.0f), roundf(data[2] * 100.0f));
    }

    void rotateCube(const Math::Vec3& direction);

    void update() {
        if (this->keyboardButtonStates[SDL_SCANCODE_ESCAPE]) {
            this->running = false;
        }

        this->cube->animate(this->frameTime);
    }

    void render();

    SDL_Window* window;
    SDL_GLContext context;

    std::unique_ptr<Game::Cube> cube;
    std::unique_ptr<Opengl::FrameBuffer> frameBuffer;
    std::shared_ptr<Opengl::RenderEffect> defaultEffect;
    std::shared_ptr<Opengl::RenderEffect> pickupEffect;

    Game::Camera camera;

    int width;
    int height;
    bool running;
    float frameTime;

    bool mouseButtonStates[SDL_BUTTON_X2 + 1];
    bool keyboardButtonStates[SDL_NUM_SCANCODES];
};

}  // namespace Rubik

#endif  // POLANDBALL_H
