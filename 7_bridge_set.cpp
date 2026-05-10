#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <vector>

template <class T>
class SetStorage
{
public:
    virtual ~SetStorage() = default;
    virtual bool add(const T& value) = 0;
    virtual bool remove(const T& value) = 0;
    virtual bool contains(const T& value) const = 0;
    virtual std::size_t size() const = 0;
    virtual std::vector<T> elements() const = 0;
    virtual std::unique_ptr<SetStorage<T>> clone() const = 0;
};

template <class T>
class VectorSetStorage : public SetStorage<T>
{
public:
    VectorSetStorage() = default;

    explicit VectorSetStorage(const std::vector<T>& values)
    {
        for (const auto& value : values) {
            add(value);
        }
    }

    bool add(const T& value) override
    {
        if (contains(value)) {
            return false;
        }
        values_.push_back(value);
        return true;
    }

    bool remove(const T& value) override
    {
        const auto it = std::find(values_.begin(), values_.end(), value);
        if (it == values_.end()) {
            return false;
        }
        values_.erase(it);
        return true;
    }

    bool contains(const T& value) const override
    {
        return std::find(values_.begin(), values_.end(), value) != values_.end();
    }

    std::size_t size() const override
    {
        return values_.size();
    }

    std::vector<T> elements() const override
    {
        return values_;
    }

    std::unique_ptr<SetStorage<T>> clone() const override
    {
        return std::make_unique<VectorSetStorage<T>>(*this);
    }

private:
    std::vector<T> values_;
};

template <class T>
class HashSetStorage : public SetStorage<T>
{
public:
    HashSetStorage() = default;

    explicit HashSetStorage(const std::vector<T>& values)
    {
        for (const auto& value : values) {
            values_.insert(value);
        }
    }

    bool add(const T& value) override
    {
        return values_.insert(value).second;
    }

    bool remove(const T& value) override
    {
        return values_.erase(value) > 0;
    }

    bool contains(const T& value) const override
    {
        return values_.find(value) != values_.end();
    }

    std::size_t size() const override
    {
        return values_.size();
    }

    std::vector<T> elements() const override
    {
        return {values_.begin(), values_.end()};
    }

    std::unique_ptr<SetStorage<T>> clone() const override
    {
        return std::make_unique<HashSetStorage<T>>(*this);
    }

private:
    std::unordered_set<T> values_;
};

template <class T, std::size_t Threshold = 8>
class Set
{
public:
    Set() : storage_(std::make_unique<VectorSetStorage<T>>()) {}

    Set(const Set& other) : storage_(other.storage_->clone()) {}

    Set& operator=(const Set& other)
    {
        if (this != &other) {
            storage_ = other.storage_->clone();
        }
        return *this;
    }

    bool add(const T& value)
    {
        const bool inserted = storage_->add(value);
        rebalance();
        return inserted;
    }

    bool remove(const T& value)
    {
        const bool erased = storage_->remove(value);
        rebalance();
        return erased;
    }

    bool contains(const T& value) const
    {
        return storage_->contains(value);
    }

    std::size_t size() const
    {
        return storage_->size();
    }

    Set unite(const Set& other) const
    {
        Set result(*this);
        for (const auto& value : other.storage_->elements()) {
            result.add(value);
        }
        return result;
    }

    Set intersect(const Set& other) const
    {
        Set result;
        for (const auto& value : storage_->elements()) {
            if (other.contains(value)) {
                result.add(value);
            }
        }
        return result;
    }

    std::vector<T> elements() const
    {
        auto values = storage_->elements();
        std::sort(values.begin(), values.end());
        return values;
    }

private:
    void rebalance()
    {
        const auto values = storage_->elements();
        if (values.size() > Threshold) {
            if (dynamic_cast<HashSetStorage<T>*>(storage_.get()) == nullptr) {
                storage_ = std::make_unique<HashSetStorage<T>>(values);
            }
        } else {
            if (dynamic_cast<VectorSetStorage<T>*>(storage_.get()) == nullptr) {
                storage_ = std::make_unique<VectorSetStorage<T>>(values);
            }
        }
    }

    std::unique_ptr<SetStorage<T>> storage_;
};

template <class T, std::size_t Threshold>
std::ostream& operator<<(std::ostream& out, const Set<T, Threshold>& set)
{
    out << "{";
    const auto values = set.elements();
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << values[i];
    }
    out << "}";
    return out;
}

int main()
{
    Set<int> a;
    a.add(1);
    a.add(2);
    a.add(3);
    a.add(4);

    Set<int> b;
    b.add(3);
    b.add(4);
    b.add(5);
    b.add(6);

    const auto union_set = a.unite(b);
    const auto intersection_set = a.intersect(b);

    std::cout << "A = " << a << '\n';
    std::cout << "B = " << b << '\n';
    std::cout << "A union B = " << union_set << '\n';
    std::cout << "A intersection B = " << intersection_set << '\n';
    std::cout << "Contains 5 in A? " << (a.contains(5) ? "Yes" : "No") << '\n';

    return 0;
}
