<p align="center">
  <a href="https://github.com/eshnd/popcorn/wiki"><img src="https://github.com/eshnd/eshnd.github.io/blob/main/oie_Sgc9BCE3cEmF.png?raw=true" alt="Logo" height=170></a>
</p>
<p align="center">A programming language to write x86 os kernels<br>(by @eshnd)<p>
  
### [Read the docs →](https://github.com/eshnd/popcorn/wiki)
## About
I made this language mostly for fun, but I also think it could be useful! It comes with a bootloader, native dynamic memory management, and optimizations wherever possible.
## Install
This works on *NIX (and WSL): `wget https://raw.githubusercontent.com/eshnd/popcorn/refs/heads/main/src/c/popcorn.c && sudo gcc popcorn.c -o /bin/popcorn && rm popcorn.c`
## Run
`popcorn <INPUT-POP-PATH> <OUTPUT-OS-PATH> <OPTIONAL-OUTPUT-ASM-PATH>`
## Note
Currently, there's not much documentation for this project because it's still under development
