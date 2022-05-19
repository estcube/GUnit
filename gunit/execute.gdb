b gunit::suite::run:end
b gunit::end

commands 1
 silent
 p suite
 p *test_name@n
 p *test_reason@n
 p *test_failed@n
 p *test_elapsed@n
 p *test_location@n
 c
end

commands 2
 silent
 set logging off
 quit
end

c
run
bt
