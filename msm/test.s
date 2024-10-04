.start
prep main
call 0
halt
.add
resn 0
.loop_l1_0
.l3_0
get 0
push 10
cmple
jumpf if_l1_1
get 0
push 1
add
dup
set 0
drop 1
jump if_l2_1
.if_l1_1
jump l2_0
.if_l2_1
jump loop_l1_0
.l2_0
get 0
ret
push 0
ret
.main
resn 2
push 10
dup
set 0
drop 1
prep start
swap
drop 1
push 1
sub
dup
set 1
drop 1
prep add
get 0
call 1
get 1
write
drop 1
get 0
dbg
push 0
ret