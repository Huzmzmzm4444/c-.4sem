#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Checkpoint
{
public:
    enum class Type
    {
        Mandatory,
        Optional
    };

    Checkpoint(std::string name, double latitude, double longitude, Type type, double penalty_hours = 0.0)
        : name_(std::move(name)), latitude_(latitude), longitude_(longitude), type_(type), penalty_hours_(penalty_hours)
    {
        if (latitude_ < -90.0 || latitude_ > 90.0) {
            throw std::out_of_range("Latitude is out of range");
        }
        if (longitude_ < -180.0 || longitude_ > 180.0) {
            throw std::out_of_range("Longitude is out of range");
        }
        if (type_ == Type::Mandatory) {
            penalty_hours_ = 0.0;
        }
    }

    const std::string& name() const
    {
        return name_;
    }

    double latitude() const
    {
        return latitude_;
    }

    double longitude() const
    {
        return longitude_;
    }

    Type type() const
    {
        return type_;
    }

    bool isMandatory() const
    {
        return type_ == Type::Mandatory;
    }

    double penaltyHours() const
    {
        return penalty_hours_;
    }

private:
    std::string name_;
    double latitude_;
    double longitude_;
    Type type_;
    double penalty_hours_;
};

class CheckpointBuilder
{
public:
    virtual ~CheckpointBuilder() = default;
    virtual void reset() = 0;
    virtual void addCheckpoint(std::size_t index, const Checkpoint& checkpoint) = 0;
};

class CheckpointDirector
{
public:
    void build(const std::vector<Checkpoint>& checkpoints, CheckpointBuilder& builder) const
    {
        builder.reset();
        for (std::size_t i = 0; i < checkpoints.size(); ++i) {
            builder.addCheckpoint(i + 1, checkpoints[i]);
        }
    }
};

class TextCheckpointBuilder : public CheckpointBuilder
{
public:
    void reset() override
    {
        out_.str({});
        out_.clear();
    }

    void addCheckpoint(std::size_t index, const Checkpoint& checkpoint) override
    {
        out_ << index << ". " << checkpoint.name() << " | "
             << std::fixed << std::setprecision(4)
             << checkpoint.latitude() << ", " << checkpoint.longitude() << " | ";
        if (checkpoint.isMandatory()) {
            out_ << "незачёт СУ";
        } else {
            out_ << checkpoint.penaltyHours() << " h";
        }
        out_ << '\n';
    }

    std::string result() const
    {
        return out_.str();
    }

private:
    std::ostringstream out_;
};

class PenaltySumBuilder : public CheckpointBuilder
{
public:
    void reset() override
    {
        total_penalty_ = 0.0;
    }

    void addCheckpoint(std::size_t, const Checkpoint& checkpoint) override
    {
        if (!checkpoint.isMandatory()) {
            total_penalty_ += checkpoint.penaltyHours();
        }
    }

    double result() const
    {
        return total_penalty_;
    }

private:
    double total_penalty_{0.0};
};

class TableCheckpointBuilder : public CheckpointBuilder
{
public:
    void reset() override
    {
        rows_.clear();
    }

    void addCheckpoint(std::size_t index, const Checkpoint& checkpoint) override
    {
        std::ostringstream lat;
        std::ostringstream lon;
        std::ostringstream status;

        lat << std::fixed << std::setprecision(4) << checkpoint.latitude();
        lon << std::fixed << std::setprecision(4) << checkpoint.longitude();

        if (checkpoint.isMandatory()) {
            status << "незачёт СУ";
        } else {
            status << std::fixed << std::setprecision(2) << checkpoint.penaltyHours() << " h";
        }

        rows_.push_back({std::to_string(index), checkpoint.name(), lat.str(), lon.str(), status.str()});
    }

    std::string result() const
    {
        std::vector<std::string> headers{"#", "Name", "Latitude", "Longitude", "Penalty"};
        std::vector<std::size_t> widths(headers.size());
        for (std::size_t i = 0; i < headers.size(); ++i) {
            widths[i] = headers[i].size();
        }

        for (const auto& row : rows_) {
            for (std::size_t i = 0; i < row.size(); ++i) {
                widths[i] = std::max(widths[i], row[i].size());
            }
        }

        std::ostringstream out;
        printRow(out, headers, widths);
        printSeparator(out, widths);
        for (const auto& row : rows_) {
            printRow(out, row, widths);
        }
        return out.str();
    }

private:
    using Row = std::vector<std::string>;

    static void printSeparator(std::ostringstream& out, const std::vector<std::size_t>& widths)
    {
        for (std::size_t width : widths) {
            out << '+' << std::string(width + 2, '-');
        }
        out << "+\n";
    }

    static void printRow(std::ostringstream& out, const Row& row, const std::vector<std::size_t>& widths)
    {
        for (std::size_t i = 0; i < row.size(); ++i) {
            out << "| " << std::left << std::setw(static_cast<int>(widths[i])) << row[i] << ' ';
        }
        out << "|\n";
    }

    std::vector<Row> rows_;
};

int main()
{
    std::vector<Checkpoint> checkpoints{
        {"Start", 55.7558, 37.6176, Checkpoint::Type::Mandatory},
        {"River", 55.7000, 37.5000, Checkpoint::Type::Optional, 1.50},
        {"Forest", 55.6000, 37.3000, Checkpoint::Type::Optional, 2.25},
        {"Finish", 55.5000, 37.1000, Checkpoint::Type::Mandatory}
    };

    CheckpointDirector director;
    TextCheckpointBuilder text_builder;
    PenaltySumBuilder sum_builder;
    TableCheckpointBuilder table_builder;

    director.build(checkpoints, text_builder);
    director.build(checkpoints, sum_builder);
    director.build(checkpoints, table_builder);

    std::cout << text_builder.result() << '\n';
    std::cout << "Total optional penalty: " << std::fixed << std::setprecision(2) << sum_builder.result() << " h\n\n";
    std::cout << table_builder.result();

    return 0;
}
