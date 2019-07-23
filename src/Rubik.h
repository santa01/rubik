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

#include <Engine.h>
#include <Puzzle.h>
#include <Label.h>
#include <Vec3.h>
#include <memory>

#if defined(_WIN32)
#define KEY_ESC   1
#define KEY_Y     21
#define KEY_P     25
#define KEY_S     31
#define KEY_N     49
#elif defined(__linux__)
#define KEY_ESC   9
#define KEY_Y     29
#define KEY_P     33
#define KEY_S     39
#define KEY_N     57
#endif

namespace Rubik {

class Rubik: public Graphene::Engine {
public:
    Rubik();

    int getShuffles() const;
    void setShuffles(int shuffles);

private:
    void onMouseMotion(int x, int y);
    void onKeyboardButton(int button, bool state);
    void onSetup();
    void onIdle();

    void setupScene();
    void setupUI();
    void updateScene();
    void updateUI();

    void rotateCube(const std::pair<int, int>& selectedCube, const Math::Vec3& direction);

    std::shared_ptr<Puzzle> puzzle;
    std::shared_ptr<Graphene::Label> timeLabel;
    std::shared_ptr<Graphene::Label> movesLabel;
    std::shared_ptr<Graphene::Label> promptLabel;

    int shuffles = 20;
    int moves = 0;
    float gameTime = 0.0f;

    enum class GameState { RUNNING, PAUSED, QUIT, FINISHED };
    GameState state = GameState::RUNNING;
};

}  // namespace Rubik

#endif  // RUBIK_H
