#include "gtest/gtest.h"
#include "System.h"

class Im_Abs_ZP : public testing::TestWithParam<word> {
    System system{0x0000, 0xFFFF, -1, -1, -1, -1, .1};
    _65C02& cpu = system.cpu;
    RAM& ram = system.ram;

    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}
public:
    void TestLoadRegisterImmediate(byte, byte, byte _65C02::*);
    void TestLoadRegisterAbsolute(byte, byte, byte _65C02::*);
    void TestLoadRegisterZeroPage(byte, byte, byte _65C02::*);
};

class IndY_XInd : public testing::TestWithParam<std::tuple<byte, byte, word, byte>> {
    System system{0x0000, 0xFFFF, -1, -1, -1, -1, .1};
    _65C02& cpu = system.cpu;
    RAM& ram = system.ram;

    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}

public:
    void TestLoadRegisterIndirectIndexed(byte, byte, byte, word, byte, byte _65C02::*);
    void TestLoadRegisterIndexedIndirect(byte opcode, byte xVal, byte zpAddr, word zpValue, byte valToLoad, byte _65C02::*Register);
};

class ZPX_ZPY : public testing::TestWithParam<std::tuple<byte, byte, byte>> {
    System system{0x0000, 0xFFFF, -1, -1, -1, -1, .1};
    _65C02& cpu = system.cpu;
    RAM& ram = system.ram;

    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}

public:
    void TestLoadRegisterZeroPageX(byte, byte, word, byte, byte _65C02::*);
    void TestLoadRegisterZeroPageY(byte, byte, word, byte, byte _65C02::*);
};

class ABSX_ABSY : public testing::TestWithParam<std::tuple<byte, byte, byte>> {
    System system{0x0000, 0xFFFF, -1, -1, -1, -1, .1};
    _65C02& cpu = system.cpu;
    RAM& ram = system.ram;

    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}

public:
    void TestLoadRegisterAbsoluteX(byte, byte, word, byte, byte _65C02::*);
    void TestLoadRegisterAbsoluteY(byte, byte, word, byte, byte _65C02::*);
};

class ZPInd : public testing::TestWithParam<std::tuple<byte, byte, word>> {
    System system{0x0000, 0xFFFF, -1, -1, -1, -1, .1};
    _65C02& cpu = system.cpu;
    RAM& ram = system.ram;

    virtual void SetUp() {
        cpu.reset();
        cpu.PC = 0xFFFC;
    }
    virtual void TearDown() {}

public:
    void TestLoadRegisterZeroPageIndirect(byte, byte, byte, word, byte _65C02::*);
};


static void VerifyUnmodifiedCPUFlagsFromLoadRegister(const std::bitset<_65C02::StatusFlags::numFlags>& ps, const std::bitset<_65C02::StatusFlags::numFlags>& psCopy) {
    EXPECT_EQ(ps.test(_65C02::StatusFlags::C), psCopy.test(_65C02::StatusFlags::C));
    EXPECT_EQ(ps.test(_65C02::StatusFlags::I), psCopy.test(_65C02::StatusFlags::I));
    EXPECT_EQ(ps.test(_65C02::StatusFlags::D), psCopy.test(_65C02::StatusFlags::D));
    EXPECT_EQ(ps.test(_65C02::StatusFlags::B), psCopy.test(_65C02::StatusFlags::B));
    EXPECT_EQ(ps.test(_65C02::StatusFlags::V), psCopy.test(_65C02::StatusFlags::V));
}


void Im_Abs_ZP::TestLoadRegisterImmediate(byte opcode, byte value, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, value);
    cpu.PS.set(_65C02::StatusFlags::N, (!value) & 0x80);
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = value;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 2;
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (value & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void Im_Abs_ZP::TestLoadRegisterAbsolute(byte opcode, byte value, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, value);
    cpu.PS.set(_65C02::StatusFlags::N, (!value) & 0x80);
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = 0x00;
    ram[0xFFFE] = 0x90;
    ram[0x9000] = value;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 4;
    constexpr dword EXPECTED_BYTES = 3;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (value & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void Im_Abs_ZP::TestLoadRegisterZeroPage(byte opcode, byte value, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, value);
    cpu.PS.set(_65C02::StatusFlags::N, (!value) & 0x80);
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = 0x42;
    ram[0x0042] = value;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 3;
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !value);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (value & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void IndY_XInd::TestLoadRegisterIndirectIndexed(byte opcode, byte yVAl, byte zpAddr, word zpVal, byte valToLoad, byte _65C02::* Register) {
    word valAddress = zpVal + yVAl;
    if(valAddress == 0xFFFC or valAddress == 0xFFFD or valAddress == zpAddr or valAddress == (byte)(zpAddr + 1)) {
        GTEST_SKIP();
    }
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.Y = yVAl;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = zpAddr;
    ram[zpAddr] = (zpVal & 0xFF);         //low byte
    ram[(byte)(zpAddr + 1)] = zpVal >> 8;   //high byte
    ram[zpVal + yVAl] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    dword EXPECTED_CYCLES = 5 + (((zpVal & 0xFF) + yVAl) > 0xFF);
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void IndY_XInd::TestLoadRegisterIndexedIndirect(byte opcode, byte xVal, byte zpAddr, word zpVal, byte valToLoad, byte _65C02::* Register) {
    if(zpVal == 0xFFFC or zpVal == 0xFFFD or zpVal == (byte)(zpAddr + xVal) or zpVal == (byte)(zpAddr + xVal + 1)) {
        GTEST_SKIP();
    }
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.X = xVal;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = zpAddr;
    ram[(byte)(zpAddr + xVal)] = (zpVal & 0xFF);
    ram[(byte)(zpAddr + xVal + 1)] = zpVal >> 8;
    ram[zpVal] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 6;
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void ZPX_ZPY::TestLoadRegisterZeroPageX(byte opcode, byte xVal, word zpAddr, byte valToLoad, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.X = xVal;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = zpAddr;
    ram[(byte)(zpAddr + xVal)] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 4;
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void ZPX_ZPY::TestLoadRegisterZeroPageY(byte opcode, byte yVal, word zpAddr, byte valToLoad, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.Y = yVal;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = zpAddr;
    ram[(byte)(zpAddr + yVal)] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    dword EXPECTED_CYCLES = 4 + ((word)(zpAddr + yVal) > 0xFF);
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void ABSX_ABSY::TestLoadRegisterAbsoluteX(byte opcode, byte xVal, word addr, byte valToLoad, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.X = xVal;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = addr & 0xFF;
    ram[0xFFFE] = addr >> 8;
    ram[(addr & 0xFF00) | (byte)(addr + xVal)] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    dword EXPECTED_CYCLES = 4 + ((addr & 0xFF) + xVal > 0xFF);
    constexpr dword EXPECTED_BYTES = 3;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void ABSX_ABSY::TestLoadRegisterAbsoluteY(byte opcode, byte yVal, word addr, byte valToLoad, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    cpu.Y = yVal;
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = addr & 0xFF;
    ram[0xFFFE] = addr >> 8;
    ram[(addr & 0xFF00) | (byte)(addr + yVal)] = valToLoad;
    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    dword EXPECTED_CYCLES = 4 + ((addr & 0xFF) + yVal > 0xFF);
    constexpr dword EXPECTED_BYTES = 3;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

void ZPInd::TestLoadRegisterZeroPageIndirect(byte opcode, byte zpAddr, byte valToLoad, word zpVal, byte _65C02::* Register) {
    cpu.PS.set(_65C02::StatusFlags::Z, valToLoad);
    cpu.PS.set(_65C02::StatusFlags::N, (!valToLoad) & 0x80);
    ram[0xFFFC] = opcode;
    ram[0xFFFD] = zpAddr;
    ram[zpAddr] = zpVal & 0xFF;
    ram[(byte)(zpAddr + 1)] = zpVal >> 8;
    ram[zpVal] = valToLoad;

    auto psCopy = cpu.PS;
    auto PCCopy = cpu.PC;
    constexpr dword EXPECTED_CYCLES = 5;
    constexpr dword EXPECTED_BYTES = 2;
    cpu.execute();

    EXPECT_EQ(cpu.cycles.getCycles(), EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC - PCCopy, EXPECTED_BYTES);
    EXPECT_EQ(cpu.*Register, valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::Z), !valToLoad);
    EXPECT_EQ(cpu.PS.test(_65C02::StatusFlags::N), (valToLoad & 0x80) >> 7);
    VerifyUnmodifiedCPUFlagsFromLoadRegister(cpu.PS, psCopy);
}

TEST_P(Im_Abs_ZP, LDAImmediate) {
    TestLoadRegisterImmediate(_65C02::INS_LDA_IM, GetParam(), &_65C02::A);
}

TEST_P(Im_Abs_ZP, LDXImmediate) {
    TestLoadRegisterImmediate(_65C02::INS_LDX_IM, GetParam(), &_65C02::X);
}

TEST_P(Im_Abs_ZP, LDYImmediate) {
    TestLoadRegisterImmediate(_65C02::INS_LDY_IM, GetParam(), &_65C02::Y);
}

TEST_P(Im_Abs_ZP, LDAAbsolute) {
    TestLoadRegisterAbsolute(_65C02::INS_LDA_ABS, GetParam(), &_65C02::A);
}

TEST_P(Im_Abs_ZP, LDXAbsolute) {
    TestLoadRegisterAbsolute(_65C02::INS_LDX_ABS, GetParam(), &_65C02::X);
}

TEST_P(Im_Abs_ZP, LDYAbsolute) {
    TestLoadRegisterAbsolute(_65C02::INS_LDY_ABS, GetParam(), &_65C02::Y);
}

TEST_P(Im_Abs_ZP, LDAZeroPage) {
    TestLoadRegisterZeroPage(_65C02::INS_LDA_ZP, GetParam(), &_65C02::A);
}

TEST_P(Im_Abs_ZP, LDXZeroPage) {
    TestLoadRegisterZeroPage(_65C02::INS_LDX_ZP, GetParam(), &_65C02::X);
}

TEST_P(Im_Abs_ZP, LDYZeroPage) {
    TestLoadRegisterZeroPage(_65C02::INS_LDY_ZP, GetParam(), &_65C02::Y);
}

TEST_P(IndY_XInd, LDAIndirectY) {
    TestLoadRegisterIndirectIndexed(_65C02::INS_LDA_INDY, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), std::get<3>(GetParam()), &_65C02::A);
}

TEST_P(IndY_XInd, LDAXIndirect) {
    TestLoadRegisterIndexedIndirect(_65C02::INS_LDA_XIND, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), std::get<3>(GetParam()), &_65C02::A);
}

TEST_P(ZPX_ZPY, LDAZeroPageX) {
    TestLoadRegisterZeroPageX(_65C02::INS_LDA_ZPX, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::A);
}

TEST_P(ZPX_ZPY, LDYZeroPageX) {
    TestLoadRegisterZeroPageX(_65C02::INS_LDY_ZPX, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::Y);
}

TEST_P(ZPX_ZPY, LDXZeroPageY) {
    TestLoadRegisterZeroPageY(_65C02::INS_LDX_ZPY, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::X);
}

TEST_P(ABSX_ABSY, LDAAbsoluteX) {
    TestLoadRegisterAbsoluteX(_65C02::INS_LDA_ABSX, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::A);
}

TEST_P(ABSX_ABSY, LDYAbsoluteX) {
    TestLoadRegisterAbsoluteX(_65C02::INS_LDY_ABSX, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::Y);
}

TEST_P(ABSX_ABSY, LDAAbsoluteY) {
    TestLoadRegisterAbsoluteY(_65C02::INS_LDA_ABSY, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::A);
}

TEST_P(ABSX_ABSY, LDXAbsoluteY) {
    TestLoadRegisterAbsoluteY(_65C02::INS_LDX_ABSY, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::X);
}

TEST_P(ZPInd, LDAZPIndirect) {
    TestLoadRegisterZeroPageIndirect(_65C02::INS_LDA_ZPIND, std::get<0>(GetParam()), std::get<1>(GetParam()), std::get<2>(GetParam()), &_65C02::A);
}

INSTANTIATE_TEST_SUITE_P(LoadValueParam, Im_Abs_ZP,
                         testing::Range(static_cast<word>(std::numeric_limits<byte>::min()),
                                        static_cast<word>(std::numeric_limits<byte>::max() + 1), 0x7F));

INSTANTIATE_TEST_SUITE_P(RegVal_ZPAddr_ZPVal_LoadVal_Params, IndY_XInd,
                         testing::Values(
                                 std::make_tuple(0x04, 0x00, 0x9000, 0x00),
                                 std::make_tuple(0xff, 0x00, 0x9001, 0x7F),
                                 std::make_tuple(0xff, 0xff, 0x4200, 0xFF),
                                 std::make_tuple(0xff, 0x00, 0x01FF, 0xFF),
                                 std::make_tuple(0xff, 0xff, 0xFFFF, 0x7F)
                         ));

INSTANTIATE_TEST_SUITE_P(RegVal_ZPAddr_LoadVal_Params, ZPX_ZPY,
                         testing::Values(
                                 std::make_tuple(0x00, 0x00, 0x00),
                                 std::make_tuple(0x00, 0x00, 0x7F),
                                 std::make_tuple(0x5, 0x42, 0xFF),
                                 std::make_tuple(0xFF, 0xFF, 0xFF)
                                 ));

INSTANTIATE_TEST_SUITE_P(ZPAddr_LoadVal_ZPVal_Params, ZPInd,
                         testing::Values(
                                 std::make_tuple(0x00, 0x00, 0x9000),
                                 std::make_tuple(0x00, 0x7F, 0x9000),
                                 std::make_tuple(0x00, 0xFF, 0x9000),
                                 std::make_tuple(0xFF, 0x00, 0x9000)
                                 ));

INSTANTIATE_TEST_SUITE_P(RegVal_Addr_LoadVal_Params, ABSX_ABSY,
                         testing::Values(
                                 std::make_tuple(0x00, 0x4480, 0x00),
                                 std::make_tuple(0x00, 0x4480, 0x7F),
                                 std::make_tuple(0x00, 0x4480, 0xFF),
                                 std::make_tuple(0xFF, 0x4480, 0x00)
                                 ));

TEST(ProgramLoggingOutput, LoadRegister) {
    System system{0x0000, 0x3FFF, 0x6000, 0x7FFF, 0x8000, 0xFFFF, .001};
    system.executeProgram("a.out", 88, true, "emulation_load_register.txt");
}


