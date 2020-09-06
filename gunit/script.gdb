set pagination off
set $test_count = 0

set logging file {self.build_dir}{os.sep}gunit{os.sep}gdblog
set logging on

file {path}

b gunit_hook

commands 1
 set $test_count = $test_count + 1
 printf "TEST%d\\n", $test_count
 p result
 p expected
 p line_number
 bt
 c
end

run
set logging off
quit
