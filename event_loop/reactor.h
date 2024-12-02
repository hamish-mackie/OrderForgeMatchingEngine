#pragma once

#include <vector>

class ReactorItem {
public:
    virtual void run() = 0;
    virtual ~ReactorItem() = default;
};

class Reactor {
public:
    void run() {
        run_event_loop_ = true;
        while (run_event_loop_) {
            for (auto* item: reactor_items_) {
                item->run();
            }
        }
    }

    void stop() { run_event_loop_ = false; }

    template<typename T>
    void add_item(T* item) {
        reactor_items_.push_back(reinterpret_cast<ReactorItem*>(item));
    }

private:
    // Probably should be made thread safe
    bool run_event_loop_{false};
    std::vector<ReactorItem*> reactor_items_;
};
