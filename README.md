# hxdp — Terminal-based Hexadecimal File Viewer

![Demo](images/demo.gif)

**hxdp** is a terminal-based hexadecimal file viewer for Unix-like systems, written in C. It allows users to view any file's raw bytes in hexadecimal format alongside their ASCII representation, with convenient navigation and colour highlighting.


## Features

- Scroll through file pages using:
  - `w` — Page Up  
  - `s` — Page Down  
- Navigate within a page using arrow keys
- Colour-coded display:
  - Cyan for byte offsets  
  - Green for printable ASCII bytes  
  - Grey for non-printable bytes  
- Displays bytes and their corresponding ASCII characters side by side


## Usage

Usage: ./hxdp [-c|--cache-size <cache_size>] [-b|--bytes-per-line <bytes>] [--safe] [--light] <filename>
## Command-line arguments

**`<filename>`**  
The file to open and view

**`-c, --cache-size <value>`**  
Set the number of pages to cache (minimum 5, default 25)

**`-b, --bytes-per-line <value>`**  
Set the number of bytes displayed per line (maximum 24, default 16)

**`--safe`**  
Enable safe mode with enforced limits:  
- Max cache size: 10,000 pages  
- Max bytes per line: 24  

**`--light`**  
Enable light mode for low resource usage:  
- Max cache size: 8 pages  
- Max bytes per line: 16  

**`-h, --help`**  
Show help message and exit

## Examples
**`./hxdp myfile.bin`**

**`./hxdp -c 10 -b 16 myfile.bin`**

**`./hxdp --safe myfile.bin`**

## Limitations
- Only works on Unix-like systems due to use of system-specific calls (`termios`, `ioctl`, `unistd.h`).
- Does not support editing of files (read-only viewer).
- Terminal size must be sufficient to display full output. Resizing is not handled dynamically.

## Future Plans
- Add dynamic resizing support for different terminal dimensions.
- Support for customisable colour themes via command-line arguments.

