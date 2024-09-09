#pragma once

namespace of {

template<typename T>
class SingleTonWrapper {
    using Type = T;

public:
    template<typename... Args>
    static Type& get_instance(Args&&... args) {
        static Type instance(std::forward<Args>(args)...);
        return instance;
    }

    SingleTonWrapper(const SingleTonWrapper&) = delete;

    SingleTonWrapper& operator=(const SingleTonWrapper&) = delete;

private:
    SingleTonWrapper() = default;
};

} // namespace of
