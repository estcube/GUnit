import magic
import os
import re

from junit_xml import TestSuite, TestCase

class GUnit:
    def __init__(self, build_dir, executable='gdb'):
        self.build_dir = build_dir
        self.logging_file = f'{build_dir}{os.sep}gunit{os.sep}gdblog'
        self.log = f'-ex "set logging file {self.logging_file}"'

        self.remote = ''
        self.load = ''
        self.run = 'run'

        self.gdb = executable

    def openOCD(build_dir, gdb_port, executable='gdb'):
        target = GUnit(build_dir, executable)
        target.remote = f'-ex "target remote :{gdb_port}"'
        target.load = '-ex load -ex "monitor reset init"'
        target.run = '-ex c'

        return target

    # Runs automatic testing on given binary/test suite
    def test(self, path):
        # Activate GDB and connect to server
        os.system(f'{self.gdb} -batch-silent {self.log} -x setup.gdb {self.remote} -ex "file {path}" {self.load} -x script.gdb {self.run}')

    def junit(self):
        cases = []

        with open(self.logging_file, "r") as gdblog:
            cases = gdblog.read().split("TESTx")

        test_cases = []

        for i, testcase in enumerate(cases[1:]):
            line = re.search(r"LINE (\d+)", testcase).group(1)
            success = re.search(r"PASS (\d)", testcase).group(1)
            expected = re.search(r"EXPECT (.+?)\n", testcase).group(1)
            result = re.search(r"RESULT (.+?)\n", testcase).group(1)

            caseparts = testcase.split("BACKTRACE")

            tc = TestCase(f"Test{i + 1}", "x", 0.0, caseparts[0], "")

            if success != "1":
                fail_message = f"Expected {expected}, received {result}"

                # Failure means assertion fail, error means unexpected fault during the execution of the test
                tc.add_failure_info(fail_message)

                tc.stderr = caseparts[1]

            test_cases += [tc]

        test_suite = TestSuite("Test test suite", test_cases)
        print(TestSuite.to_xml_string([test_suite]))


gunit = GUnit.openOCD(".", 9000, "arm-none-eabi-gdb")
# gunit.test("/home/mathiasplans/ESTCube/OBCS/firmware/build/bin/main.elf")
gunit.junit()
