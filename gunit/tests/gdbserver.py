from gunit import GUnit

gunit = GUnit.gdbserver("localhost:9000")
gunit.test()
gunit.junit()
