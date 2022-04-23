#include "System.h"

System::System(sdword ramMin, sdword ramMax, sdword regMin, sdword regMax, sdword romMin, sdword romMax, double Mhz) :
        bus{ram,ramMin,ramMax,registers, regMin, regMax, eeprom,romMin,romMax}{
    cpu.connectBus(&bus);
    cpu.setCycleDuration(Mhz);
}

void System::executeProgram(const std::string& programObjFile, uint64_t instructionsToExecute, bool logging, const std::string& outFile) {
    eeprom.loadProgram(programObjFile);
    cpu.reset(eeprom[0xFFFC - 0x8000] | eeprom[0xFFFD - 0x8000] << 8);
    bus.log = logging;
    if(!bus.openProgramOutFile(outFile)) { bus.log = false; }
    cpu.execute(instructionsToExecute);
}

