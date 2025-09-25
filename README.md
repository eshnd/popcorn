# popcorn
programming language to write x86 os kernels
# install
this works on *NIX (and WSL): `wget https://raw.githubusercontent.com/eshnd/popcorn/refs/heads/main/src/c/popcorn.c && sudo gcc popcorn.c -o /bin/popcorn && rm popcorn.c`
# run
`popcorn <INPUT-POP-PATH> <OUTPUT-OS-PATH> <OPTIONAL-OUTPUT-ASM-PATH>`
# note
currently, there's not much documentation for this project because it's still under development
