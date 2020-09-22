# GUnit
App for executing unit tests using GDB.

-fvar-tracking and -g has to be used when compiling the tests

## Example
For example if OpenOCD is used as GDB server and we are testing on an ARM chip
```python
gunit = GUnit.openOCD("localhost:9000", executable='arm-none-eabi-gdb')
gunit.test("test.elf")
gunit.junit()
```

The first line will create a GUnit object for OpenOCD. The second line will execute
the tests and the third line will construct a JUnit test report.

All the outputs will be put into ``gunit`` directory. To specify where the directory
will be generated, define ``build_dir`` argument for the constructor.
