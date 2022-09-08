# TODO
- make command input storage dynamic to save space
- include all .c libraries so dont have to compile all manually: use makefile
- colour shit
- // check commands part: make that one liner: run the function lksh_<command_name>
- cd ignoring all but first arg when more than 1 arg is not how zsh cd behaves
- change get_pwd to take result argument
- transfer subtract_strings to helpers, and getcwd func too: fix all import problems
- cd `../~` compatibility: just replace ~ with ROOT
- `cd ` handle
- Fix the CMDS thing, make it dynamic
- **DOES NOT HANDLE SPACES AND TABS AT THE END**
- Colour code with red for no such file or dir and other error messages
  - block calculate + format also when outside dir
- st blocks thing
- flag handling should be more modular for future flags
- cd .. from ~: behaviour for path display in shell

- time
  - fix sleep 0.5: kapil has decimal time

- took n seconds sep colour

- discover
  - discovr nicndnx prints the thing

- dont include .c files
- make mem allocs dynamic everywhere
- `cmd ` doesn't work: **MAJOR PROBLEM: NOT HANDLING TABS AND SPACES**
# ls
  - child dir total block size not working; also colouring
  - **ls file**
  - ls invalid option remove the - from the flag
  - ls ~/includes handle: handle ~ everywhere by replacing it with ROOT in string using [this](https://stackoverflow.com/questions/32496497/standard-function-to-replace-character-or-substring-in-a-char-array)
  - **ls colour coding not working when ls'ing outside current dir (down)**
  - `ls -al ..` vs `ls -a -l ..`
  - `ls -l` wiggly: because hard links: right align that as well
  - colour `ls -l` output

- handle guidelines
- PERFECT SHIT: HANDLE ALL COMMAND ERRORS: **PERROR**
- FOREGROUND PRINTS WHEN BG

==========

- ~~ls file, colour when multiple directories~~~
- ~~ls colour coding not working when ls'ing outside current dir (down)~~
- ~~ls -l right align hard links + hard links not working properly~~

- vim& somehow takes over + cant type shit after

~~- bg process when you run foreground while bg running, the wrong name is printed when bg ends~~
- `\t   \t cd   ..` not working
~~- path displayed on shell prompt~~
- ls file
