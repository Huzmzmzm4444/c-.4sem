#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

class ExpressionFactory;

class Expression
{
public:
    virtual ~Expression() = default;
    virtual double calculate(const std::map<std::string, double>& context) const = 0;
    virtual void print(std::ostream& out) const = 0;
    virtual void addRef() = 0;
    virtual void release() = 0;
};

class FlyweightExpression : public Expression
{
public:
    explicit FlyweightExpression(ExpressionFactory& factory) : factory_(factory) {}

    void addRef() override
    {
        ++ref_count_;
    }

    void release() override;

    int refCount() const
    {
        return ref_count_;
    }

protected:
    ExpressionFactory& factory_;
    int ref_count_{0};
};

class Constant final : public FlyweightExpression
{
public:
    Constant(ExpressionFactory& factory, double value, bool immortal)
        : FlyweightExpression(factory), value_(value), immortal_(immortal) {}

    double value() const
    {
        return value_;
    }

    bool immortal() const
    {
        return immortal_;
    }

    double calculate(const std::map<std::string, double>&) const override
    {
        return value_;
    }

    void print(std::ostream& out) const override
    {
        if (value_ == static_cast<long long>(value_)) {
            out << static_cast<long long>(value_);
        } else {
            out << value_;
        }
    }

private:
    double value_;
    bool immortal_;
};

class Variable final : public FlyweightExpression
{
public:
    Variable(ExpressionFactory& factory, std::string name)
        : FlyweightExpression(factory), name_(std::move(name)) {}

    const std::string& name() const
    {
        return name_;
    }

    double calculate(const std::map<std::string, double>& context) const override
    {
        const auto it = context.find(name_);
        if (it == context.end()) {
            throw std::runtime_error("Unknown variable: " + name_);
        }
        return it->second;
    }

    void print(std::ostream& out) const override
    {
        out << name_;
    }

private:
    std::string name_;
};

class BinaryOperation : public Expression
{
public:
    BinaryOperation(Expression* left, Expression* right) : left_(left), right_(right) {}

    ~BinaryOperation() override
    {
        left_->release();
        right_->release();
    }

    void addRef() override
    {
    }

    void release() override
    {
        delete this;
    }

    void print(std::ostream& out) const override
    {
        out << '(';
        left_->print(out);
        out << ' ' << symbol() << ' ';
        right_->print(out);
        out << ')';
    }

protected:
    virtual char symbol() const = 0;

    Expression* left_;
    Expression* right_;
};

class Addition final : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    double calculate(const std::map<std::string, double>& context) const override
    {
        return left_->calculate(context) + right_->calculate(context);
    }

private:
    char symbol() const override
    {
        return '+';
    }
};

class Subtraction final : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    double calculate(const std::map<std::string, double>& context) const override
    {
        return left_->calculate(context) - right_->calculate(context);
    }

private:
    char symbol() const override
    {
        return '-';
    }
};

class Multiplication final : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    double calculate(const std::map<std::string, double>& context) const override
    {
        return left_->calculate(context) * right_->calculate(context);
    }

private:
    char symbol() const override
    {
        return '*';
    }
};

class Division final : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    double calculate(const std::map<std::string, double>& context) const override
    {
        const double divisor = right_->calculate(context);
        if (divisor == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return left_->calculate(context) / divisor;
    }

private:
    char symbol() const override
    {
        return '/';
    }
};

class ExpressionFactory
{
public:
    ExpressionFactory()
    {
        for (int value = -5; value <= 256; ++value) {
            auto constant = std::make_unique<Constant>(*this, static_cast<double>(value), true);
            constants_.emplace(static_cast<double>(value), std::move(constant));
        }
    }

    Constant* createConstant(double value)
    {
        auto it = constants_.find(value);
        if (it == constants_.end()) {
            auto constant = std::make_unique<Constant>(*this, value, false);
            it = constants_.emplace(value, std::move(constant)).first;
        }
        it->second->addRef();
        return it->second.get();
    }

    Variable* createVariable(const std::string& name)
    {
        auto it = variables_.find(name);
        if (it == variables_.end()) {
            auto variable = std::make_unique<Variable>(*this, name);
            it = variables_.emplace(name, std::move(variable)).first;
        }
        it->second->addRef();
        return it->second.get();
    }

    void release(Constant* constant)
    {
        if (constant == nullptr) {
            return;
        }
        if (constant->immortal()) {
            return;
        }
        if (constant->refCount() == 0) {
            constants_.erase(constant->value());
        }
    }

    void release(Variable* variable)
    {
        if (variable == nullptr) {
            return;
        }
        if (variable->refCount() == 0) {
            variables_.erase(variable->name());
        }
    }

private:
    std::unordered_map<double, std::unique_ptr<Constant>> constants_;
    std::unordered_map<std::string, std::unique_ptr<Variable>> variables_;
};

void FlyweightExpression::release()
{
    --ref_count_;
    if (auto* constant = dynamic_cast<Constant*>(this)) {
        factory_.release(constant);
        return;
    }
    if (auto* variable = dynamic_cast<Variable*>(this)) {
        factory_.release(variable);
        return;
    }
}

std::ostream& operator<<(std::ostream& out, const Expression& expression)
{
    expression.print(out);
    return out;
}

int main()
{
    ExpressionFactory factory;

    Constant* c = factory.createConstant(2);
    Variable* x = factory.createVariable("x");
    Expression* expression = new Addition(c, x);

    std::map<std::string, double> context;
    context["x"] = 3.0;

    std::cout << *expression << '\n';
    std::cout << std::fixed << std::setprecision(2) << expression->calculate(context) << '\n';

    delete expression;

    Expression* complex_expression = new Multiplication(
        new Addition(factory.createVariable("x"), factory.createConstant(10)),
        new Subtraction(factory.createConstant(7), factory.createVariable("x"))
    );

    std::cout << *complex_expression << '\n';
    std::cout << std::fixed << std::setprecision(2) << complex_expression->calculate(context) << '\n';

    delete complex_expression;
    return 0;
}
