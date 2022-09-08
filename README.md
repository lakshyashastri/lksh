# Compile
Simply run `make` to compile the shell, then run `./lksh` to execute
# Points to note

## Discover
- The command does not search dot (hidden) folders
## Background
- In cases where background and foreground processes are being run together, the time taken by the foreground processes is printed on the terminal
- The number of background processes is limited to MAX_LENGTH. See [Other](#Other)

## Foreground
- The total time printed is for all processes

## Other
- Most arrays are not dynamic (yet) and have a max size of MAX_LENGTH. This means some things like path lengths and hostname will be limited to MAX_LENGTH units (characters / number of units). However, most important things have dynamic memory and so this should not affect the running of the shell too much, especially for testing purposes
- Folder and file names should ideally not have spaces