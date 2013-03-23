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

#ifndef LOGGER_H
#define LOGGER_H

#include "NonCopyable.h"

namespace Rubik {

namespace Utils {

class Logger: public Common::NonCopyable {
public:
    enum {
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(int level, const char* message, ...);

    void setThreshold(int threshold) {
        switch (threshold) {
            case Logger::LOG_INFO:
            case Logger::LOG_WARNING:
            case Logger::LOG_ERROR:
                this->threshold = threshold;
                break;
        }
    }

private:
    Logger() {
        this->threshold = Logger::LOG_INFO;
    }

    int threshold;
};

}  // namespace Utils

}  // namespace Rubik

#endif  // LOGGER_H
