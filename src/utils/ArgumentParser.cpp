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

#include "ArgumentParser.h"

#include <getopt.h>
#include <libgen.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace Rubik {

namespace Utils {

bool ArgumentParser::addArgument(const std::string& longName, const std::string& description, ArgumentType type) {
    if (regexec(this->stringRegex.get(), longName.c_str(), 0, nullptr, 0) == REG_NOMATCH) {
        return false;
    }

    if (this->arguments.find(longName) != this->arguments.end()) {
        return false;
    }

    auto argument = std::shared_ptr<Argument>(new Argument());
    argument->type = type;
    argument->description = description;
    argument->set = false;

    this->options[longName] = 0;
    this->arguments[longName] = argument;
    return true;
}

bool ArgumentParser::addArgument(char name, const std::string& longName,
        const std::string& description, ArgumentType type) {
    if (name < '0' || (name > '9' && name < 'A') || (name > 'Z' && name < 'a') || name > 'z') {
        return false;
    }

    if (this->aliases.find(name) != this->aliases.end()) {
        return false;
    }

    if (!this->addArgument(longName, description, type)) {
        return false;
    }

    this->options[longName] = name;
    this->aliases[name] = longName;
    return true;
}

bool ArgumentParser::parse(int argc, char** argv) {
    std::stringstream shortOptions;
    std::vector<struct option> longOptions;

    struct option longOption;
    for (auto& option: this->options) {
        longOption.name = option.first.c_str();
        longOption.flag = nullptr;
        longOption.val = 0;

        auto& argument = this->arguments.at(option.first);
        longOption.has_arg = (argument->type == ArgumentType::TYPE_BOOL) ? no_argument : required_argument;
        longOptions.push_back(longOption);  // vector::emplace_back() kills gcc, how unfortunate

        if (option.second) {
            shortOptions << option.second;
            if (argument->type != ArgumentType::TYPE_BOOL) {
                shortOptions << ":";
            }
        }
    }

    int i = 0;
    auto optionsArray = std::unique_ptr<struct option[]>(new struct option[longOptions.size() + 1]);
    for (auto& option: longOptions) {
        optionsArray[i++] = option;
    }

    optionsArray[i].name = nullptr;
    optionsArray[i].flag = nullptr;
    optionsArray[i].val = 0;
    optionsArray[i].has_arg = no_argument;

    int optionIndex = 0;
    bool parseFailed = false;
    std::shared_ptr<Argument> argument;

    int option;
    while ((option = getopt_long(argc, argv, shortOptions.str().c_str(), optionsArray.get(), &optionIndex)) != -1) {
        switch (option) {
            case 0:
                argument = this->arguments.at(optionsArray[optionIndex].name);
                if ((parseFailed = !this->validate(argument, optarg)) == false) {
                    if (optionsArray[optionIndex].has_arg != no_argument) {
                        argument->value = optarg;
                    }
                    argument->set = true;
                }
                break;

            default:
                auto alias = this->aliases.find(option);
                if (alias != this->aliases.end()) {
                    argument = this->arguments.at(alias->second);
                    if ((parseFailed = !this->validate(argument, optarg)) == false) {
                        if (argument->type != ArgumentType::TYPE_BOOL) {
                            argument->value = optarg;
                        }
                        argument->set = true;
                    }
                } else {
                    parseFailed = true;
                }
        }

        if (parseFailed) {
            break;
        }
    }

    char* application = basename(argv[0]);

    if (optind < argc) {
        std::cout << "Try '" << application << " --help' for more information." << std::endl;
        return false;
    }

    if (this->isSet("help") || parseFailed) {
        this->help(application);
    }

    return !parseFailed;
}

void ArgumentParser::help(char* application) const {
    std::cout << "Usage: " << application << " [option]..." << std::endl;
    if (!this->description.empty()) {
        std::cout << this->description << std::endl;
    }

    std::cout << std::endl << "Available options:" << std::endl;
    for (auto& option: this->options) {
        auto& argument = this->arguments.at(option.first);

        std::stringstream name;
        if (option.second) {
            name << std::setw(5) << "-" << option.second << ", --";
        } else {
            name << std::setw(10) << "--";
        }

        std::stringstream longName;
        longName << option.first;

        switch (argument->type) {
            case ArgumentType::TYPE_INT:
                longName << "=[int]";
                break;

            case ArgumentType::TYPE_FLOAT:
                longName << "=[float]";
                break;

            case ArgumentType::TYPE_STRING:
                longName << "=[string]";
                break;

            default:
                break;
        }

        std::cout << name.str()
                  << std::left << std::setw(25) << longName.str()
                  << argument->description << std::endl;
    }
}

bool ArgumentParser::validate(const std::shared_ptr<Argument>& argument, const char* value) const {
    regex_t* regex = nullptr;

    switch (argument->type) {
        case ArgumentType::TYPE_FLOAT:
            regex = this->floatRegex.get();
            break;

        case ArgumentType::TYPE_INT:
            regex = this->integerRegex.get();
            break;

        default:
            return true;
    }

    return (regexec(regex, value, 0, nullptr, 0) == REG_NOERROR);
}

}  // namespace Utils

}  // namespace Rubik
