#pragma once

#ifndef __AGTB_UTILS_ERROR_HPP__
#define __AGTB_UTILS_ERROR_HPP__

#include <exception>

#include "../details/Macros.hpp"

AGTB_BEGIN

namespace Errors
{
    class SimpleMsgError : public std::exception
    {
    private:
        std::string _what;

    public:
        explicit SimpleMsgError(const std::string &msg = "Construct failed")
            : _what(msg) {}
        explicit SimpleMsgError(const char *msg = "Construct failed")
            : _what(msg) {}
        virtual const char *what() const noexcept override
        {
            return _what.c_str();
        }
    };

#define AGTB_DEF_MSG_ERR(__err, __default)                 \
    class __err : public SimpleMsgError                    \
    {                                                      \
    public:                                                \
        explicit __err(const std::string &msg = __default) \
            : SimpleMsgError(msg) {}                       \
        explicit __err(const char *msg = __default)        \
            : SimpleMsgError(msg) {}                       \
    }

    // class ConstructorError : public SimpleMsgError
    // {
    // public:
    //     explicit ConstructorError(const std::string &msg = "Construct failed")
    //         : SimpleMsgError(msg) {}
    //     explicit ConstructorError(const char *msg = "Construct failed")
    //         : SimpleMsgError(msg) {}
    // };

    // class JsonKeyError : public SimpleMsgError
    // {
    // public:
    //     explicit JsonKeyError(const std::string &msg)
    //         : SimpleMsgError(msg) {}
    //     explicit JsonKeyError(const char *msg)
    //         : SimpleMsgError(msg) {}
    // };

    AGTB_DEF_MSG_ERR(ConstructorError, "Construct failed");
    AGTB_DEF_MSG_ERR(JsonKeyError, "Key not found");
    AGTB_DEF_MSG_ERR(ContainerSizeError, "Invalid size for use");
    AGTB_DEF_MSG_ERR(NotImplementError, "AGTB not implement this");
}

using Errors::ConstructorError;
using Errors::ContainerSizeError;
using Errors::JsonKeyError;
using Errors::NotImplementError;

AGTB_END

#endif