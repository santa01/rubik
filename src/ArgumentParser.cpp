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

#include <ArgumentParser.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Rubik {

ArgumentParser::ArgumentParser() {
    this->addArgument("help", "display this help", ValueType::BOOL);
    this->addArgument("version", "output version information", ValueType::BOOL);
}

void ArgumentParser::setDescription(const std::string& description) {
    this->description = description;
}

const std::string& ArgumentParser::getDescription() const {
    return this->description;
}

void ArgumentParser::setVersion(const std::string& version) {
    this->version = version;
}

const std::string& ArgumentParser::getVersion() const {
    return this->version;
}

bool ArgumentParser::addArgument(char name, const std::string& longName, const std::string& description, ValueType type) {
    if (!std::isalnum(name)) {
        return false;
    }

    if (this->aliases.find(name) != this->aliases.end()) {
        return false;
    }

    if (!this->addArgument(longName, description, type)) {
        return false;
    }

    auto argument = this->arguments[longName];
    argument->shortOption = name;

    this->aliases[name] = longName;
    return true;
}

bool ArgumentParser::addArgument(const std::string& longName, const std::string& description, ValueType type) {
    auto status = std::find_if_not(longName.begin(), longName.end(), [](char c) { return std::isalnum(c); });
    if (status != longName.end()) {
        return false;
    }

    if (this->arguments.find(longName) != this->arguments.end()) {
        return false;
    }

    auto argument = std::shared_ptr<Argument>(new Argument());
    argument->type = type;
    argument->description = description;
    argument->shortOption = '\0';
    argument->isSet = false;

    this->arguments[longName] = argument;
    return true;
}

bool ArgumentParser::parse(int argc, char** argv) {
    int optionIndex = 1;
    bool parseFailed = false;

    while (optionIndex < argc) {
        char name;
        std::string longName;

        std::string option(argv[optionIndex++]);
        ArgumentType argumentType = this->prepareArgument(option, name, longName);

        if (argumentType == ArgumentType::INVALID) {
            std::cerr << "Invalid argument '" << option << "'\n\n";
            parseFailed = true;
            break;
        }

        if (argumentType == ArgumentType::SHORT_NAME) {
            longName = this->aliases.at(name);
        }

        std::shared_ptr<Argument> argument = this->arguments.at(longName);
        if (argument->type == ValueType::BOOL) {
            argument->isSet = true;
            continue;
        }

        if (optionIndex == argc) {
            std::cerr << "Argument '" << option << "' requires value\n\n";
            parseFailed = true;
            break;
        }

        std::string argumentValue(argv[optionIndex++]);
        if (!this->validateArgument(argument->type, argumentValue)) {
            std::cerr << "Invalid '" << option << "' argument value '" << argumentValue << "'\n\n";
            parseFailed = true;
            break;
        }

        argument->value = argumentValue;
        argument->isSet = true;
    }

    if (this->isSet("help") || parseFailed) {
        this->printHelp(argv[0]);
    } else if (this->isSet("version")) {
        this->printVersion();
    }

    return !parseFailed;
}

bool ArgumentParser::isSet(char name) const {
    return this->isSet(this->aliases.at(name));
}

bool ArgumentParser::isSet(const std::string& longName) const {
    auto argument = this->arguments.at(longName);
    return argument->isSet;
}

std::string ArgumentParser::getOption(char name) const {
    return this->getOption(this->aliases.at(name));
}

std::string ArgumentParser::getOption(const std::string& longName) const {
    auto argument = this->arguments.at(longName);
    return argument->value;
}

ArgumentParser::ArgumentType ArgumentParser::prepareArgument(const std::string& option, char& name, std::string& longName) {
    size_t optionLength = option.length();
    ArgumentType argumentType = ArgumentType::INVALID;

    if (optionLength == 2 && option[0] == '-') {
        char optionName = option[1];
        if (this->aliases.find(optionName) != this->aliases.end()) {
            name = optionName;
            argumentType = ArgumentType::SHORT_NAME;
        }
    } else if (optionLength > 2 && option.substr(0, 2) == "--") {
        std::string optionName = option.substr(2);
        if (this->arguments.find(optionName) != this->arguments.end()) {
            longName = optionName;
            argumentType = ArgumentType::LONG_NAME;
        }
    }

    return argumentType;
}

bool ArgumentParser::validateArgument(ValueType type, const std::string& value) {
    std::stringstream testValue(value);

    switch (type) {
        case ValueType::FLOAT: {
            float floatValue;
            testValue >> std::noskipws >> floatValue;
            break;
        }

        case ValueType::INT: {
            int intValue;
            testValue >> std::noskipws >> intValue;
            break;
        }

        default:
            return true;
    }

    return (testValue.eof() && !testValue.fail());
}

void ArgumentParser::printHelp(char* application) const {
    if (!this->description.empty()) {
        std::cout << this->description << "\n";
    }

    std::cout << "Usage: " << application << " [option]..." << "\n\n"
              << "Available options:" << "\n";

    for (auto& argument: this->arguments) {
        std::stringstream option;
        if (argument.second->shortOption != '\0') {
            option << std::setw(5) << "-" << argument.second->shortOption << ", --";
        } else {
            option << std::setw(10) << "--";
        }

        std::stringstream longName;
        longName << argument.first << " ";

        switch (argument.second->type) {
            case ValueType::INT:
                longName << "[int]";
                break;

            case ValueType::FLOAT:
                longName << "[float]";
                break;

            case ValueType::STRING:
                longName << "[string]";
                break;

            default:
                break;
        }

        std::cout << option.str()
                  << std::left << std::setw(25) << longName.str()
                  << argument.second->description << "\n";
    }
}

void ArgumentParser::printVersion() const {
    if (!this->version.empty()) {
        std::cout << this->version << "\n";
    }
}

}  // namespace Rubik
