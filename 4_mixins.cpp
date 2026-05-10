#include <atomic>
#include <cassert>
#include <iostream>

template <class Derived>
class less_than_comparable
{
public:
    friend bool operator>(const Derived& lhs, const Derived& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const Derived& lhs, const Derived& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Derived& lhs, const Derived& rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const Derived& lhs, const Derived& rhs)
    {
        return !(lhs < rhs) && !(rhs < lhs);
    }

    friend bool operator!=(const Derived& lhs, const Derived& rhs)
    {
        return !(lhs == rhs);
    }
};

template <class Derived>
class counter
{
public:
    counter()
    {
        ++created_count_;
    }

    counter(const counter&)
    {
        ++created_count_;
    }

    counter(counter&&) noexcept
    {
        ++created_count_;
    }

    static std::size_t count()
    {
        return created_count_.load();
    }

private:
    inline static std::atomic<std::size_t> created_count_{0};
};

class Number : public less_than_comparable<Number>, public counter<Number>
{
public:
    explicit Number(int value) : value_(value) {}

    int value() const
    {
        return value_;
    }

    bool operator<(const Number& other) const
    {
        return value_ < other.value_;
    }

private:
    int value_;
};

int main()
{
    Number one{1};
    Number two{2};
    Number three{3};
    Number four{4};

    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);
    assert(one != two);

    std::cout << "Count: " << counter<Number>::count() << '\n';
    return 0;
}
