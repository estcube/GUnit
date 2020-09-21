b gunit_hook
b gunit_end

commands 1
 silent
 printf "\n"
 printf "TESTx\n"
 printf "LINE %d\n", line_number
 printf "PASS %d\n", (result == expected) ^ no
 printf "EXPECT %d\n", expected
 printf "RESULT %d\n", result
 printf "BACKTRACE\n"
 bt
 c
end

commands 2
 silent
 set logging off
 quit
end

c
