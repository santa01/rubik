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

#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <regex.h>  // TODO: Use C++11 regex
#include <string>
#include <memory>
#include <unordered_map>

namespace Rubik {

namespace Utils {

class ArgumentParser {
public:
    enum ArgumentType {
        TYPE_BOOL,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING
    };

    ArgumentParser():
            floatRegex(new regex_t, regfree),
            integerRegex(new regex_t, regfree),
            stringRegex(new regex_t, regfree) {
        regcomp(this->floatRegex.get(), "^(0|[1-9][[:digit:]]*)\\.[[:digit:]]+$", REG_EXTENDED | REG_NOSUB);
        regcomp(this->integerRegex.get(), "^(0|[1-9][[:digit:]]*)$", REG_EXTENDED | REG_NOSUB);
        regcomp(this->stringRegex.get(), "^[[:alnum:]](-|[[:alnum:]])*$", REG_EXTENDED | REG_NOSUB);
        this->addArgument("help", "display this help", ArgumentType::TYPE_BOOL);
    }

    void setDescription(const std::string& description) {
        this->description = description;
    }

    bool addArgument(const std::string& longName,const std::string& description, ArgumentType type);
    bool addArgument(char name, const std::string& longName, const std::string& description, ArgumentType type);

    bool parse(int argc, char** argv);

    bool isSet(char name) const {
        return this->isSet(this->aliases.at(name));
    }

    bool isSet(const std::string& longName) const {
        auto argument = this->arguments.find(longName);
        return (argument != this->arguments.end() && argument->second->set);
    }

    std::string getOption(char name) const {
        return this->getOption(this->aliases.at(name));
    }

    std::string getOption(const std::string& longName) const {
        if (!this->isSet(longName)) {
            return "";
        }

        auto argument = this->arguments.at(longName);
        return argument->value;
    }

private:
    typedef struct {
        ArgumentType type;
        std::string value;
        std::string description;
        char shortOption;
        bool set;
    } Argument;

    void help(char* application) const;
    bool validate(const std::shared_ptr<Argument>& argument, const char* value) const;

    void pushArgument(char name, const std::string& longName, const std::string& description, ArgumentType type) {
        auto argument = std::shared_ptr<Argument>(new Argument());
        argument->type = type;
        argument->description = description;
        argument->shortOption = name;
        argument->set = false;

        this->arguments[longName] = argument;
    }

    std::unordered_map<std::string, std::shared_ptr<Argument>> arguments;
    std::unordered_map<char, std::string> aliases;

    std::string description;
    std::unique_ptr<regex_t, void (*)(regex_t*)> floatRegex;
    std::unique_ptr<regex_t, void (*)(regex_t*)> integerRegex;
    std::unique_ptr<regex_t, void (*)(regex_t*)> stringRegex;
};

}  // namespace Utils

}  // namespace Rubik

#endif  // ARGUMENTPARSER_H
