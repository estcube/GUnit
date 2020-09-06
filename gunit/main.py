import magic
import os
import re

def iself(f):
    ftype = magic.from_file(f)

    if "ELF" in ftype:
        return True

    return False

class GUnit:
    def __init__(self, build_dir, gdb_port, build_method=None):
        self.build_dir = build_dir
        self.gdb_port = gdb_port
        self.build_method = build_method

    # Composes a test suite C file from raw test suite
    def testsuite(self, path):

        return

    # Runs automatic testing on given binary/test suite
    def test(self, path):
        # Converts given C file to executable
        #if !iself(path):
        #    if self.build_method is None:
        #        return # ERROR

        #    path = testsuite(path)
        #    path = self.build_method(path)

        commands = f'''
set pagination off
set confirm off
set $test_count = 0

set logging file {self.build_dir}{os.sep}gunit{os.sep}gdblog
set logging on

file {path}

b gunit_hook

commands\\nset $test_count = $test_count + 1\\nprintf "TEST%d\\n", $test_count\\np result\\np expected\\np line_number\\nbt\\nc\\nend

run
set logging off
quit
        '''

        commands = commands.split('\n')

        # Remove empty lines
        for command in commands:
            if re.match("^[ \t]+$", command) or command == "":
                commands.remove(command)

        commands = map(lambda cmd: f" -ex='{cmd}'", commands)
        commands = ''.join(commands)

        # Activate GDB and connect to server
        os.system(f'gdb -batch-silent {commands}')


        return


gunit = GUnit(".", 10)
gunit.test("a.out")
