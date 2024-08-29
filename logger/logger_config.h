#pragma once

struct LoggerConfig {
    bool write_std_out = true;
    bool write_log_file = true;
    const std::string log_file_path = "./log.txt";
    uint64_t mem_block_size = MB * 20;
    uint64_t number_blocks = 5;
};
