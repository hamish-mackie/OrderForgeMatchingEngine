#pragma once

#include "config_helpers.h"

struct LoggerConfig {
    bool write_std_out = true;
    bool write_log_file = true;
    std::string log_file_path = "./log.txt";
    uint64_t mem_block_size_mb = 20;
    uint64_t number_blocks = 5;

    static LoggerConfig from_json(const json& j) {
        LoggerConfig config{};

        set_config(j, "write_std_out", config.write_std_out);
        set_config(j, "log_file_path", config.log_file_path);
        set_config(j, "mem_block_size", config.mem_block_size_mb);
        set_config(j, "number_blocks", config.number_blocks);

        return config;
    }
};

inline LoggerConfig get_default_logger_config() { return {}; }
