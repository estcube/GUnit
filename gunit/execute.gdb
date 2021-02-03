b gunit_hook
b gunit_end
b gunit_fail_hook
b gunit_range_hook

commands 1
 silent
 !date +%s.%N >> $GUNIT_GDBTIMES
 printf "TESTx\n"
 printf "FILE %s\n", file
 printf "LINE %d\n", line_number
 printf "PASS %d\n", (result == expected) ^ no
 printf "TYPE 0\n"
 printf "EXPECT %d\n", expected
 printf "RESULT %d\n", result
 printf "BACKTRACE\n"
 bt
 !date +%s.%N >> $GUNIT_GDBTIMES
 c
end

commands 2
 silent
 set logging off
 quit
end

commands 3
 silent
 !date +%s.%N >> $GUNIT_GDBTIMES
 printf "TESTx\n"
 printf "FILE %s\n", file
 printf "LINE %d\n", line_number
 printf "PASS %d\n", 0
 printf "TYPE 1\n"
 printf "BACKTRACE\n"
 p *test
 !date +%s.%N >> $GUNIT_GDBTIMES
 c
end

commands 4
 silent
 !date +%s.%N >> $GUNIT_GDBTIMES
 printf "TESTx\n"
 printf "FILE %s\n", file
 printf "LINE %d\n", line_number
 printf "PASS %d\n", !greater && (expected > result) || greater && (expected < result)
 printf "TYPE 2\n"
 printf "EXPECT %d\n", expected
 printf "RESULT %d\n", result
 printf "BACKTRACE\n"
 bt
 !date +%s.%N >> $GUNIT_GDBTIMES
 c
end

!date +%s.%N > $GUNIT_GDBTIMES

c
run
