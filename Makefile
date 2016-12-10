all:
	cd gem5
	scons CPU_MODELS="TimingSimpleCPU" build/X86/gem5.fast -j3
