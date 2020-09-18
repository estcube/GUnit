set pagination off
set $test_count = 0

set logging file gunit/gdblog
set logging overwrite on
set logging on

file a.out

b gunit_hook

commands 1
 set $test_count = $test_count + 1
 printf "TEST%d\n", $test_count
 printf "PASS %d\n", result == expected
 printf "RESULT %d\n", result
 printf "EXPECT %d\n", expected
 printf "ONLINE %d\n", line_number
 bt
 c
end

run
set logging off
quit
