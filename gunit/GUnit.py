import os
import re

from collections import defaultdict
from junit_xml import TestSuite, TestCase

class GUnit:
    def __init__(self, build_dir, executable='gdb'):
        self.build_dir = build_dir
        self.logging_file = f'{build_dir}{os.sep}gunit{os.sep}gdblog'
        self.log = f'-ex "set logging file {self.logging_file}"'

        self.remote = ''
        self.load = ''

        self.gdb = executable

    # Configure GUnit for OpenOCD server
    def openOCD(build_dir, gdb_uri, executable='gdb'):
        target = GUnit(build_dir, executable)
        target.remote = f'-ex "target remote {gdb_uri}"'
        target.load = '-ex load -ex "monitor reset init"'

        return target

    # Configure GUnit for regular GDB server
    def gdbserver(build_dir, gdb_uri):
        target = GUnit(build_dir)
        target.remote = f'-ex "target remote {gdb_uri}"'

        return target

    # Runs automatic testing on given binary/test suite
    def test(self, path=None):
        os.environ["GUNIT_GDBTIMES"] = f"{self.build_dir}{os.sep}gunit{os.sep}gdbtimes"

        # Activate GDB and connect to server
        command = f'{self.gdb} -batch-silent {self.log} -x setup.gdb {self.remote} '

        if path is not None:
            command += f'-ex "file {path}" {self.load} '

        command += f'-x script.gdb'

        os.system(command)

    def junit(self):
        cases = []

        with open(self.logging_file, "r") as gdblog:
            cases = gdblog.read().split("TESTx")

        test_cases = defaultdict(lambda: [])
        test_no = defaultdict(lambda: 1)

        last_function = None

        for testcase in cases[1:]:
            caseparts = testcase.split("BACKTRACE")

            suite = re.search(r"FILE (.*)\n", caseparts[0]).group(1)
            line = re.search(r"LINE (\d+)", caseparts[0]).group(1)
            success = re.search(r"PASS (\d)", caseparts[0]).group(1)
            expected = re.search(r"EXPECT (.+?)\n", caseparts[0]).group(1)
            result = re.search(r"RESULT (.+?)\n", caseparts[0]).group(1)

            function = re.search(r"in (?!gunit_hook|gunit_array)(.*) \(.*?\) at", caseparts[1]).group(1)
            suite = re.search(r"(.*/)*(.*?)\..*", suite).group(2)

            add = True
            tc = None

            if last_function != function:
                tc = TestCase(f"Test{test_no[suite]}", function, 0.0, "", "", allow_multiple_subelements=True)
                test_no[suite] += 1

            else:
                tc = test_cases[suite][-1]
                add = False

            last_function = function

            if success != "1":
                fail_message = ""
                if expected != result:
                    fail_message = f"Expected {expected}, received {result}"

                else:
                    fail_message = f"Did not expect {expected}"

                # Failure means assertion fail, error means unexpected fault during the execution of the test
                tc.add_failure_info(fail_message, caseparts[1])

            if add:
                test_cases[suite] += [tc]

        test_suites = []
        for key in test_cases.keys():
            test_suites += [TestSuite(key, test_cases[key])]

        with open(f"{self.build_dir}{os.sep}gunit{os.sep}report.xml", "w") as outxml:
            outxml.write(TestSuite.to_xml_string(test_suites))


gunit = GUnit.openOCD(".", ":9000", "arm-none-eabi-gdb")
gunit.test("/home/mathiasplans/ESTCube/OBCS/firmware/build/bin/main.elf")

#gunit = GUnit.gdbserver(".", ":9002")
#gunit.test()

gunit.junit()
