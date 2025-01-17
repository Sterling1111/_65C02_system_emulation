#include <gtest/gtest.h>
#include "System.h"

class LoggingTests : public testing::Test {
public:
    System system{0x0000, 0x3FFF, 0x6000, 0x7FFF, 0x8000, 0xFFFF, 1};
    W65C02& cpu = system.cpu;
    RAM& ram = system.ram;
    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}

    static bool compareFiles(const std::string& p1, const std::string& p2);
    bool executeProgramAndCompareFiles(const std::string& emulationObjFile, uint64_t instructionsToExecute,
                                       const std::string& emulationOutFile, const std::string& cpuOutFile);

    static void convertFileToNativeLineEndings(const std::string &inputFileName, const std::string &outputFileName);
};

void LoggingTests::convertFileToNativeLineEndings(const std::string& inputFileName, const std::string& outputFileName) {
    std::ifstream inputFile(inputFileName, std::ios::in | std::ios::binary);
    if (!inputFile) {
        return;
    }

    std::ofstream outputFile(outputFileName, std::ios::out | std::ios::binary);
    if (!outputFile) {
        return;
    }

    char prevChar = '\0';
    char currentChar;
    while (inputFile.get(currentChar)) {
        if (currentChar == '\r') {
            // Handle CR character (possibly part of CRLF)
            outputFile.put(currentChar);
            if (inputFile.peek() == '\n') {
                // If next character is LF, skip it
                inputFile.get(currentChar);
                outputFile.put(currentChar);
            }
        } else if (currentChar == '\n') {
            // Handle LF character
            outputFile.put('\n'); // Convert LF to native line ending
        } else {
            // Other characters
            outputFile.put(currentChar);
        }
        prevChar = currentChar;
    }

    inputFile.close();
    outputFile.close();
}

bool LoggingTests::compareFiles(const std::string &p1, const std::string &p2) {
    std::string p1_native = p1 + "_native";
    std::string p2_native = p2 + "_native";

    convertFileToNativeLineEndings(p1, p1_native);
    convertFileToNativeLineEndings(p2, p2_native);

    std::ifstream f1(p1_native, std::ifstream::binary | std::ifstream::ate);
    std::ifstream f2(p2_native, std::ifstream::binary | std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        return false; // File problem
    }

    if (f1.tellg() != f2.tellg()) {
        return false; // Size mismatch
    }

    // Seek back to the beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);

    bool filesEqual = std::equal(
            std::istreambuf_iterator<char>(f1.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(f2.rdbuf())
    );

    // Clean up temporary files
    std::remove(p1_native.c_str());
    std::remove(p2_native.c_str());

    return filesEqual;
}

bool LoggingTests::executeProgramAndCompareFiles(const std::string &emulationObjFile, uint64_t instructionsToExecute,
                                                 const std::string &emulationOutFile, const std::string& cpuOutFile) {
    system.executeProgram(emulationObjFile, instructionsToExecute, true, emulationOutFile);
    return compareFiles(emulationOutFile, cpuOutFile);
}

TEST_F(LoggingTests, StoreRegister) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_store_register.out",
                                              119,
                                              "EmulationLogFiles//emulation_store_register.txt",
                                              "65C02LogFiles//65C02_store_register.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, JumpsAndCalls) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_jumps_and_calls.out",
                                              22,
                                              "EmulationLogFiles//emulation_jumps_and_calls.txt",
                                              "65C02LogFiles//65C02_jumps_and_calls.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, LoadRegister) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_load_register.out",
                                              155,
                                              "EmulationLogFiles//emulation_load_register.txt",
                                              "65C02LogFiles//65C02_load_register.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, LogicalOperations) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_logical_operations.out",
                                              643,
                                              "EmulationLogFiles//emulation_logical_operations.txt",
                                              "65C02LogFiles//65C02_logical_operations.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, RegisterTransfer) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_transfer_register.out",
                                              62,
                                              "EmulationLogFiles//emulation_transfer_register.txt",
                                              "65C02LogFiles//65C02_transfer_register.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, StackOperations) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_stack_operations.out",
                                              43,
                                              "EmulationLogFiles//emulation_stack_operations.txt",
                                              "65C02LogFiles//65C02_stack_operations.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, Subtraction) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_subtraction.out",
                                              33291,
                                              "EmulationLogFiles//emulation_subtraction.txt",
                                              "65C02LogFiles//65C02_subtraction.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, Addition) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_addition.out",
                                              33291,
                                              "EmulationLogFiles//emulation_addition.txt",
                                              "65C02LogFiles//65C02_addition.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, Compare) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_compare.out",
                                              13325,
                                              "EmulationLogFiles//emulation_compare.txt",
                                              "65C02LogFiles//65C02_compare.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, Arithmetic) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_arithmetic_operations.out",
                                              226,
                                              "EmulationLogFiles//emulation_arithmetic_operations.txt",
                                              "65C02LogFiles//65C02_arithmetic_operations.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, IncrementsDecrements) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_increments_decrements.out",
                                              6237,
                                              "EmulationLogFiles//emulation_increments_decrements.txt",
                                              "65C02LogFiles//65C02_increments_decrements.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, Shifts) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_shifts.out",
                                              7178,
                                              "EmulationLogFiles//emulation_shifts.txt",
                                              "65C02LogFiles//65C02_shifts.txt")};
    EXPECT_TRUE(result);
}

TEST_F(LoggingTests, ShiftOperations) {
    bool result{executeProgramAndCompareFiles("EmulationOutFiles//emulation_shift_operations.out",
                                              207,
                                              "EmulationLogFiles//emulation_shift_operations.txt",
                                              "65C02LogFiles//65C02_shift_operations.txt")};
    EXPECT_TRUE(result);
}