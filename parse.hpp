#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

class Arguments {
private:
    const int32_t default_threads_count_ = omp_get_max_threads();

    bool no_omp_ = false;
    bool omp_default_ = false;
    int32_t omp_custom_ = 0;
    std::string input_filename_;
    std::string output_filename_;

    int32_t threads_count_;
public:
    void SetNoOmp() {
        no_omp_ = true;
    }

    void SetDefaultOmp() {
        omp_default_ = true;
    }

    void SetCustomOmp(const int32_t& threads_number) {
        omp_custom_ = threads_number;
    }

    void SetInputFilename(const std::string& filename) {
        input_filename_ = filename;
    }

    void SetOutputFilename(const std::string& filename) {
        output_filename_ = filename;
    }

    void SetThreadsCount() {
        if (no_omp_) {
            threads_count_ = 1;
            return;
        }

        if (omp_default_) {
            threads_count_ = default_threads_count_;
            return;
        }

        threads_count_ = omp_custom_;
    }

    bool GetIfNoOmp() {
        return no_omp_;
    }

    bool GetIfDefaultOmp() {
        return omp_default_;
    }

    int32_t GetCustomOmp() {
        return omp_custom_;
    }

    std::string GetInputFilename() {
        return input_filename_;
    }

    std::string GetOutputFilename() {
        return output_filename_;
    }

    int32_t GetThreadsCount() {
        return threads_count_;
    }
};

class ErrorHandler {
public:
    static const int8_t kGoodParse = 0;
    static const int8_t kTooManyArguments = -1;
    static const int8_t kTooFewArguments = -2;
    static const int8_t kInvalidThreadsNumber = 1;
    static const int8_t kInvalidInputFilename = 2;
    static const int8_t kInvalidIterationsNumber = 3;

    static void ShowError(int8_t error_code) {
        switch (error_code) {
            case kGoodParse:
                return;
            case kTooManyArguments:
                std::cerr << "Too Many Arguments" << std::endl;
                exit(kTooManyArguments);
            case kTooFewArguments:
                std::cerr << "Too Few Arguments" << std::endl;
                exit(kTooFewArguments);
            case kInvalidThreadsNumber:
                std::cerr << "Invalid Threads Number" << std::endl;
                exit(kInvalidThreadsNumber);
            case kInvalidInputFilename:
                std::cerr << "Invalid Input Filename" << std::endl;
                exit(kInvalidInputFilename);
            case kInvalidIterationsNumber:
                std::cerr << "Invalid Iterations Number" << std::endl;
                exit(kInvalidIterationsNumber);
            default:
                std::cerr << "Unknown Error" << std::endl;
                exit(4);
        }
    }
};

class ArgParser {
private:
    Arguments& args_;
    int8_t error_code_ = ErrorHandler::kGoodParse;

    void CheckArguments() {
        if (args_.GetInputFilename().empty() || args_.GetOutputFilename().empty()) {
            error_code_ = ErrorHandler::kTooFewArguments;
            return;
        }

        std::ifstream file(args_.GetInputFilename());

        if (file.bad()) {
            error_code_ = ErrorHandler::kInvalidInputFilename;
            return;
        }

        file.close();

        if (!(args_.GetIfNoOmp() || args_.GetIfDefaultOmp()) && args_.GetCustomOmp() < 1) {
            error_code_ = ErrorHandler::kInvalidThreadsNumber;
            return;
        }

        args_.SetThreadsCount();
    }

public:
    explicit ArgParser(Arguments& args) : args_(args) {}

    void Parse (int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "--no-omp") == 0) {
                args_.SetNoOmp();
            } else if (std::strcmp(argv[i], "--omp-threads") == 0 && i < argc - 1) {
                ++i;
                if (std::strcmp(argv[i], "default") == 0) {
                    args_.SetDefaultOmp();
                } else {
                    args_.SetCustomOmp(std::stoi(argv[i]));
                }
            } else if (std::strcmp(argv[i], "--input") == 0 && i < argc - 1) {
                args_.SetInputFilename(argv[++i]);
            } else if (std::strcmp(argv[i], "--output") == 0 && i < argc - 1) {
                args_.SetOutputFilename(argv[++i]);
            } else {
                error_code_ = -1;
            }
        }

        CheckArguments();
        ErrorHandler::ShowError(error_code_);
    }
};
