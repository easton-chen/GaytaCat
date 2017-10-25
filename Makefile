RV64GCC = riscv64-unknown-elf-gcc
FLAG = -Wa,-march=rv64im
TESTPRO = fact matrix qsort Ackermann

all: Simulation $(TESTPRO)

Simulation: Simulation.cpp Read_Elf.cpp
	g++ -o Simulation Simulation.cpp Read_Elf.cpp

fact: ./testcode/fact.c
	$(RV64GCC) $(FLAG) -o fact ./testcode/fact.c

matrix: ./testcode/matrix.c
	$(RV64GCC) $(FLAG) -o matrix ./testcode/matrix.c

qsort: ./testcode/qsort.c
	$(RV64GCC) $(FLAG) -o qsort ./testcode/qsort.c

Ackermann: ./testcode/Ackermann.c
	$(RV64GCC) $(FLAG) -o Ackermann ./testcode/Ackermann.c

clean:
	rm -f Simulation $(TESTPRO)
