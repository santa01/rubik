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
    this->addArgument("help", "display this help", ValueType::TYPE_BOOL);
    this->addArgument("version", "output version information", ValueType::TYPE_BOOL);
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
    this->aliases[name] = longName;

    argument->shortOption = name;

    return true;
}

bool ArgumentParser::addArgument(const std::string& longName, const std::string& description, ValueType type) {
    if (!ArgumentParser::isAlphaNumeric(longName)) {
        return false;
    }

    if (this->arguments.find(longName) != this->arguments.end()) {
        return false;
    }

    auto argument = std::shared_ptr<Argument>(new Argument());
    this->arguments[longName] = argument;

    argument->type = type;
    argument->description = description;

    return true;
}

bool ArgumentParser::parse(int argc, char** argv) {
    int optionIndex = 1;
    bool parseFailed = false;

    while (optionIndex < argc) {
        char name;
        std::string longName;

        std::string option(argv[optionIndex++]);
        parseFailed = !ArgumentParser::prepare(option, name, longName);
        if (parseFailed) {
            break;
        }

        if (name != '\0') {
            longName = this->aliases[name];
        }

        std::shared_ptr<Argument> argument = this->arguments[longName];
        if (argument->type == ValueType::TYPE_BOOL) {
            argument->isSet = true;
            continue;
        }

        std::string argumentValue(argv[optionIndex++]);
        parseFailed = !ArgumentParser::validate(argument->type, argumentValue);
        if (parseFailed) {
            break;
        }

        argument->value = argumentValue;
        argument->isSet = true;
    }

    if (optionIndex < argc) {
        parseFailed = true;
    }

    if (this->isSet("version")) {
        this->printVersion();
    } else if (this->isSet("help") || parseFailed) {
        this->printHelp(argv[0]);
    }

    return !parseFailed;
}

bool ArgumentParser::isSet(char name) const {
    return this->isSet(this->aliases.at(name));
}

bool ArgumentParser::isSet(const std::string& longName) const {
    auto argument = this->arguments.find(longName);
    return (argument != this->arguments.end() && argument->second->isSet);
}

std::string ArgumentParser::getOption(char name) const {
    return this->getOption(this->aliases.at(name));
}

std::string ArgumentParser::getOption(const std::string& longName) const {
    if (!this->isSet(longName)) {
        return "";
    }

    auto argument = this->arguments.at(longName);
    return argument->value;
}

bool ArgumentParser::isAlphaNumeric(const std::string& value) {
    auto status = std::find_if_not(value.begin(), value.end(), [](char c) { return std::isalnum(c); });
    return (status == value.end());
}

bool ArgumentParser::prepare(const std::string& option, char& name, std::string& longName) {
    size_t optionLength = option.length();
    bool isOptionValid = false;

    if (optionLength == 2 && option[0] == '-') {
        char optionName = option[1];
        if (std::isalnum(optionName)) {
            name = optionName;
            longName = "";
            isOptionValid = true;
        }
    } else if (optionLength > 2 && option.substr(0, 2) == "--") {
        std::string optionName = option.substr(2);
        if (ArgumentParser::isAlphaNumeric(optionName)) {
            name = '\0';
            longName = optionName;
            isOptionValid = true;
        }
    }

    return isOptionValid;
}

bool ArgumentParser::validate(ValueType type, const std::string& value) {
    std::stringstream testValue(value);

    switch (type) {
        case ValueType::TYPE_FLOAT: {
            float floatValue;
            testValue >> std::noskipws >> floatValue;
            break;
        }

        case ValueType::TYPE_INT: {
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
    std::cout << "Usage: " << application << " [option]..." << std::endl;
    if (!this->description.empty()) {
        std::cout << this->description << std::endl;
    }

    std::cout << std::endl << "Available options:" << std::endl;
    for (auto& argument: this->arguments) {
        std::stringstream option;
        if (argument.second->shortOption != '\0') {
            option << std::setw(5) << "-" << argument.second->shortOption << ", --";
        } else {
            option << std::setw(10) << "--";
        }

        std::stringstream longName;
        longName << argument.first;

        switch (argument.second->type) {
            case ValueType::TYPE_INT:
                longName << "=[int]";
                break;

            case ValueType::TYPE_FLOAT:
                longName << "=[float]";
                break;

            case ValueType::TYPE_STRING:
                longName << "=[string]";
                break;

            default:
                break;
        }

        std::cout << option.str()
                  << std::left << std::setw(25) << longName.str()
                  << argument.second->description << std::endl;
    }
}

void ArgumentParser::printVersion() const {
    if (!this->version.empty()) {
        std::cout << this->version << std::endl;
    }
}

}  // namespace Rubik
