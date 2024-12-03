#pragma once

#include "nlohmann/json.hpp"

using json = nlohmann::json;

template<typename T>
static T read_config_file(const std::string& config_file = "config.json") {
    std::ifstream file(config_file, std::ios::in);
    if (!file) {
        throw std::runtime_error(fmt::format("config file could not be opened: {}", config_file));
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    auto json = json::parse(buffer.str());
    return T::from_json(json);
}

template<typename T>
void parse_enum_config(const json& j, const std::string& field_name, T& config_field) {
    if (j.contains(field_name)) {
        std::string config_str;
        j.at(field_name).get_to(config_str);
        std::optional<T> opt = magic_enum::enum_cast<T>(config_str);
        if (opt.has_value()) {
            config_field = opt.value();
        }
    }
}

template<typename T>
void set_config(const json& j, const std::string& field_name, T& config_field) {
    if (j.contains(field_name)) {
        j.at(field_name).get_to(config_field);
    }
}

template<typename T>
T set_config(const json& j, const std::string& field_name) {
    if (j.contains(field_name))
        return T::from_json(j.at(field_name));
    return T{};
}

template<typename T>
std::vector<T> set_configs(const json& j, const std::string& field_name) {
    std::vector<T> configs;
    if (j.contains(field_name))
        for (auto& r: j.at(field_name)) {
            configs.emplace_back(T::from_json(r));
        }
    else
        configs.emplace_back(T{});
    return configs;
}
