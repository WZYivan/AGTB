#pragma once

#ifndef __AGTB_UTILS_FILEREADER_HPP__
#define __AGTB_UTILS_FILEREADER_HPP__

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

namespace fs = std::filesystem;

class FileReader
{
private:
    fs::path path;
    std::ifstream ifs;
    bool path_ok, stream_ok, next_used = true;
    std::string next_line;

public:
    bool Ok()
    {
        if (std::getline(ifs, next_line))
        {
            next_used = false;
        }
        else
        {
            stream_ok = false;
        }
        return path_ok && stream_ok;
    }
    FileReader(const fs::path fpath) : path(fpath)
    {
        ifs = std::ifstream(path);
        path_ok = fs::exists(path);
        stream_ok = !ifs.eof();
    }
    std::string NextLine()
    {
        if (!next_used)
        {
            next_used = true;
            return next_line;
        }

        if (Ok())
        {
            next_used = true;
            return next_line;
        }
    }
};

class StreamReader
{
private:
    std::istream &is;
    bool stream_ok, next_used = true;
    std::string next_line;

public:
    bool Ok()
    {
        if (std::getline(is, next_line))
        {
            next_used = false;
        }
        else
        {
            stream_ok = false;
        }
        return stream_ok;
    }
    StreamReader(std::istream &_is) : is(_is)
    {
        stream_ok = !is.eof();
    }
    std::string NextLine()
    {
        if (!next_used)
        {
            next_used = true;
            return next_line;
        }

        if (Ok())
        {
            next_used = true;
            return next_line;
        }
    }
};

AGTB_UTILS_END

#endif