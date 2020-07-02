/*
  ==============================================================================

    DumbOptional.h
    Created: 2 Jul 2020 2:36:51pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#ifdef __APPLE__

    #include <memory>

struct DumbNullopt_t {
};

template<typename T>
class DumbOptional
{
private:
    std::unique_ptr<T> mValue{};

public:
    DumbOptional() = default;
    ~DumbOptional() = default;

    DumbOptional(DumbOptional const & other) : mValue(other.has_value() ? std::make_unique<T>(*other.mValue) : nullptr)
    {
    }
    DumbOptional(DumbOptional && other) = default;
    DumbOptional(T const & value) : mValue(std::make_unique<T>(value)) {}
    DumbOptional([[maybe_unused]] DumbNullopt_t const &) : mValue(nullptr) {}

    DumbOptional & operator=(DumbOptional const & other)
    {
        if (other.has_value()) {
            mValue = std::make_unique<T>(*other.mValue);
        } else {
            mValue.reset();
        }
        return *this;
    }
    DumbOptional & operator=(DumbOptional && other) = default;

    T & operator*() { return *mValue; }
    T const & operator*() const { return *mValue; }

    T & value() { return *mValue; }
    T const & value() const { return *mValue; }

    bool has_value() const { return mValue.get() != nullptr; }

    bool operator==(T const & other) const
    {
        if (mValue.get() == nullptr) {
            return false;
        }
        return *mValue == other;
    }

    DumbOptional & operator=(T const & value)
    {
        mValue = std::make_unique<T>(value);
        return *this;
    }

    DumbOptional & operator=([[maybe_unused]] DumbNullopt_t const & nullopt)
    {
        mValue.reset();
        return *this;
    }

    void reset() { mValue.reset(); }

    T * operator->() { return mValue.get(); }

    T const * operator->() const { return mValue.get(); }
};

template<typename T>
using optional = DumbOptional<T>;
DumbNullopt_t const nullopt{};

#else

    #include <optional>

template<typename T>
using optional = std::optional<T>;
auto const nullopt = std::nullopt;

#endif