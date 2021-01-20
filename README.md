# GUnit
App for executing unit tests using GDB.

Documentation: https://github.com/estcube/GUnit/wiki/Documentation

## Example
For example if OpenOCD is used as GDB server and we are testing on an ARM chip.
Please note that you need to have the openOCD server running in the background.

Secondly, all the outputs will be put into ``gunit`` directory.
To specify where the directory will be generated, define ``build_dir`` argument for the constructor.
By default, it will be your current working directory.

```python
from gunit import GUnit

# Create a GUnit object for OpenOCD
gunit = GUnit.openOCD("localhost:9000", executable='arm-none-eabi-gdb')

# Execute the tests in the compiled file
gunit.test("test.elf")

# Construct a JUnit test report
gunit.junit()
```

Here you can see a simple example of the .c file that could be used with the python example above.
```c
#include "GUnit.h"
#include "foo.h"

void test1() {
    GASSERT(5, foo_getfive());
}

void before() {
    foo_init();
}

void after() {
    foo_deinit();
}

int main() {
    // This is for executing a test suite (a collection of tests) with before and after functions.
    GEXECUTE(before, after, test1);

    // This macro has to be called at the end of the test program.
    GEND();

    // Block forever so we don't go into undefined memory regions
    while(true);
}
```
