#ifndef __AGTB_IO_JSON_HPP__
#define __AGTB_IO_JSON_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Concept.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <filesystem>
#include <generator>

// AGTB_FILE_NOT_IMPLEMENT();

AGTB_IO_BEGIN

namespace detail::JsonIO
{
    namespace fs = std::filesystem;
    namespace pt = boost::property_tree;

    namespace JsonManip
    {

    }

    template <typename __ptree>
        requires std::same_as<UnqualifiedType<__ptree>, pt::ptree>
    class PTreeWrapper
    {
    private:
        __ptree root;

    public:
        PTreeWrapper(const __ptree &ptree) : root(ptree)
        {
        }
        ~PTreeWrapper() = default;

        const __ptree &Unwrap() const
        {
            return root;
        }

        __ptree &Unwrap()
        {
            return root;
        }

        template <typename __self>
        auto Range(this __self &&self)
        {
            return std::ranges::subrange(self.root.begin(), self.root.end());
        }

        template <typename __self>
        auto Array(this __self &&self, const pt::path &path)
        {
            auto &range = self.root.get_child(path);
            return std::ranges::subrange(range.begin(), range.end()) |
                   std::views::transform([](auto &kv) -> decltype(auto)
                                         { return PTreeWrapper(kv.second); });
        }

        bool HasArray(const pt::path &path) const
        {
            return root.get_child_optional(path).has_value();
        }

        template <typename __value_type>
        __value_type Value(const pt::path &path) const
        {
            return root.template get<__value_type>(path);
        }

        template <typename __value_type>
        __value_type Value(const pt::path &path, const __value_type &default_value) const
        {
            return root.template get<__value_type>(path, default_value);
        }

        bool HasValue(const pt::path &path) const
        {
            return root.get_optional(path).has_value();
        }

        std::string ToString(bool pretty = true) const
        {
            std::ostringstream oss;
            pt::write_json(oss, root, pretty);
            return oss.str();
        }
    };

    using Json = PTreeWrapper<pt::ptree>;

    Json ReadJson(const fs::path &fpath)
    {
        if (!fs::exists(fpath))
        {
            AGTB_THROW(std::invalid_argument, std::format("file `{}` not exists", fpath.string()));
        }

        pt::ptree root{};
        pt::read_json(fpath, root);

        return Json(root);
    }

    void WriteJson(const Json &json, const std::string &filename, const std::locale &loc = std::locale(), bool pretty = true)
    {
        pt::write_json(filename, json.Unwrap(), loc, pretty);
    }
}

using detail::JsonIO::Json;
using detail::JsonIO::ReadJson;
using detail::JsonIO::WriteJson;

AGTB_IO_END

#endif