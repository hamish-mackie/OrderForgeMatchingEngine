#pragma once
#include <atomic>

constexpr uint64_t MB = 1048576;

class RingBuffer {
public:
    explicit RingBuffer(uint64_t mem_block_size = MB * 5, uint32_t no_blocks = 2) :
        mem_block_size_(mem_block_size), no_blocks_(no_blocks) {
        for (int i = 0; i < no_blocks_; ++i) {
            mem_blocks_.push_back(static_cast<char *>(std::malloc(mem_block_size_)));
            mem_blocks_end_.push_back(0);
        }
    }

    char *get_write_pointer(uint64_t wanted_space) {
        auto [current_write_offset, current_write_block] = get_current_write();
        auto *write_block = mem_blocks_[current_write_block];

        if (current_write_offset + wanted_space > mem_block_size_) {
            mem_blocks_end_[current_write_block] = current_write_offset;
            current_write_offset = 0;
            current_write_block = (current_write_block + 1) % mem_blocks_.size();
            write_block_index_.store(current_write_block, std::memory_order_relaxed);
            write_ptr_offset_.store(current_write_offset, std::memory_order_relaxed);
        }

        return &mem_blocks_[current_write_block][current_write_offset];
    }

    void forward_write_pointer(uint64_t new_write_offset) {
        auto offset = write_ptr_offset_.load(std::memory_order_relaxed);
        write_ptr_offset_.store(new_write_offset + offset, std::memory_order_relaxed);
    }

    bool can_read() {
        check_wrap_read_pointer();
        auto current_write_offset = write_ptr_offset_.load(std::memory_order_relaxed);
        auto current_write_block = write_block_index_.load(std::memory_order_relaxed);
        auto current_read_offset = read_ptr_offset_;
        auto current_read_block = read_block_index_;

        if (current_read_block == current_write_block) {
            return current_read_offset < current_write_offset;
        } else {
            return true;
        }
    }
    char *get_read_pointer() {
        check_wrap_read_pointer();
        return &mem_blocks_[read_block_index_][read_ptr_offset_];
    }

    void forward_read_pointer(uint64_t new_read_offset) { read_ptr_offset_ += new_read_offset; }

    ~RingBuffer() {
        for (const auto block: mem_blocks_) {
            free(block);
        }
    }

private:
    std::atomic<uint64_t> write_ptr_offset_{0};
    std::atomic<size_t> write_block_index_{0};
    uint64_t read_ptr_offset_{0};
    size_t read_block_index_{0};
    uint64_t mem_block_size_;
    std::vector<char *> mem_blocks_;
    std::vector<uint64_t> mem_blocks_end_;
    uint64_t no_blocks_;

    std::pair<std::atomic<uint64_t>, std::atomic<size_t>> get_current_write() {
        return {write_ptr_offset_.load(std::memory_order_relaxed), write_block_index_.load(std::memory_order_relaxed)};
    }

    void check_wrap_read_pointer() {
        auto end = mem_blocks_end_[read_block_index_];
        if (end != 0 && read_ptr_offset_ == end) {
            read_block_index_ = (read_block_index_ + 1) % mem_blocks_.size();
            read_ptr_offset_ = 0;
            mem_blocks_end_[read_block_index_] = 0;
        }
    }
};
