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

#ifndef FONT_H
#define FONT_H

#include "NonCopyable.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_IMAGE_H
#include <string>
#include <memory>
#include <tuple>

namespace Rubik {

namespace Game {

void FT_Delete_Library(FT_Library* library);
void FT_Delete_Face(FT_Face* face);
void FT_Render_Callback(int y, int count, const FT_Span* spans, void* param);

typedef struct {
    void* pixels;
    int width;
    int height;
    int offset;
} FT_RenderContext;

class Font: public Common::NonCopyable {
public:
    Font() {
        if (Font::library == nullptr) {
            Font::library = std::shared_ptr<FT_Library>(new FT_Library, FT_Delete_Library);
            FT_Init_FreeType(Font::library.get());
        }
    }

    bool load(const std::string& name, int size);
    std::tuple<std::shared_ptr<void>, int, int> render(const std::string& text);

private:
    static std::shared_ptr<FT_Library> library;
    std::shared_ptr<FT_Face> face;
};

}  // namespace Game

}  // namespace Rubik

#endif  // FONT_H
