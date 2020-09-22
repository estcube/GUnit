from gunit import GUnit

gunit = GUnit.openOCD("localhost:9000", executable='arm-none-eabi-gdb')
gunit.test("/home/mathiasplans/ESTCube/OBCS/firmware/build/bin/main.elf")
gunit.junit()
