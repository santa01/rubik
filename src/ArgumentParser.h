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

#include <string>
#include <memory>
#include <unordered_map>

namespace Rubik {

enum class ValueType { BOOL, INT, FLOAT, STRING };

class ArgumentParser {
public:
    ArgumentParser();

    void setDescription(const std::string& description);
    const std::string& getDescription() const;

    void setVersion(const std::string& version);
    const std::string& getVersion() const;

    bool addArgument(char name, const std::string& longName, const std::string& description, ValueType type);
    bool addArgument(const std::string& longName, const std::string& description, ValueType type);
    bool parse(int argc, char** argv);

    bool isSet(char name) const;
    bool isSet(const std::string& longName) const;
    std::string getOption(char name) const;
    std::string getOption(const std::string& longName) const;

private:
    enum class ArgumentType { INVALID, SHORT_NAME, LONG_NAME };

    ArgumentType prepareArgument(const std::string& option, char& name, std::string& longName);
    bool validateArgument(ValueType type, const std::string& value);

    void printHelp(char* application) const;
    void printVersion() const;

    typedef struct {
        ValueType type;
        std::string value;
        std::string description;
        char shortOption = '\0';
        bool isSet = false;
    } Argument;

    std::unordered_map<std::string, std::shared_ptr<Argument>> arguments;
    std::unordered_map<char, std::string> aliases;

    std::string description;
    std::string version;
};

}  // namespace Rubik

#endif  // ARGUMENTPARSER_H
