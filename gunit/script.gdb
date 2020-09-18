set pagination off
set $test_count = 0

set print frame-arguments none

set logging file gunit/gdblog
set logging overwrite on
set logging on

file a.out

b gunit_hook

commands 1
 silent
 set $test_count = $test_count + 1
 printf "TEST%d\n", $test_count
 printf "LINE %d\n", line_number
 printf "PASS %d\n", (result == expected) ^ no
 printf "EXPECT %d\n", expected
 printf "RESULT %d\n", result
 bt
 printf "\n"
 c
end

run
set logging off
quit
