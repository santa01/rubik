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
#include <ArgumentParser.h>
#include <Config.h>
#include <EngineConfig.h>

int main(int argc, char** argv) {
    Rubik::ArgumentParser arguments;
    arguments.setDescription(RUBIK_DESCRIPTION);
    arguments.setVersion(RUBIK_VERSION);

    arguments.addArgument('f', "fov", "camera field of view", Rubik::ValueType::FLOAT);
    arguments.addArgument('h', "height", "viewport height", Rubik::ValueType::INT);
    arguments.addArgument('w', "width", "viewport width", Rubik::ValueType::INT);
    arguments.addArgument('s', "samples", "MSAA samples", Rubik::ValueType::INT);
    arguments.addArgument('F', "fps", "maximum fps limit", Rubik::ValueType::FLOAT);
    arguments.addArgument('v', "vsync", "vertical sync", Rubik::ValueType::BOOL);
    arguments.addArgument('d', "debug", "debug logging", Rubik::ValueType::BOOL);
    arguments.addArgument('D', "data", "game data directory", Rubik::ValueType::STRING);
    arguments.addArgument('S', "shuffles", "initial cube shuffles", Rubik::ValueType::INT);

    if (!arguments.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (arguments.isSet("help") || arguments.isSet("version")) {
        return EXIT_SUCCESS;
    }

    auto& config = Graphene::GetEngineConfig();
    config.setFov(arguments.isSet("fov") ? stof(arguments.getOption("fov")) : 75.0f);
    config.setHeight(arguments.isSet("height") ? stoi(arguments.getOption("height")) : 480);
    config.setWidth(arguments.isSet("width") ? stoi(arguments.getOption("width")) : 640);
    config.setSamples(arguments.isSet("samples") ? stoi(arguments.getOption("samples")) : 0);
    config.setMaxFps(arguments.isSet("fps") ? stof(arguments.getOption("fps")) : 0.0f);
    config.setVsync(arguments.isSet("vsync"));
    config.setDebug(arguments.isSet("debug"));
    config.setDataDirectory(arguments.isSet("data") ? arguments.getOption("data") : RUBIK_DATADIR);

    Rubik::Rubik rubik;
    rubik.setShuffles(arguments.isSet("shuffles") ? stoi(arguments.getOption("shuffles")) : 20);

    return rubik.exec();
}
