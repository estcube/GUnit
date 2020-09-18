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

        # Activate GDB and connect to server
        os.system(f'gdb -batch-silent -x script.gdb')

        return


gunit = GUnit(".", 10)
gunit.test("a.out")
