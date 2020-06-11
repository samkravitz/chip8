# chip8
Chip8 emulator written in C++

## Usage

### Building
As long as you have SDL2 installed on your machine, the emulator will compile with simply  
```make```  

### Controls
The Chip8 has a pretty unusal 16 controller keypad which makes it difficult to map to a modern keyboard.  
This is the mapping that's used, where the original Chip8 keypad is on the left and the keyboard mapping is on the right.  
```
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+
```
  
  
### Emulator
The emulator, chip8, requires 1 argument: the relative path to the desired ROM  
`./chip8 [path/to/ROM]`

## Dependencies
* SDL2

## Credits
I followed [this](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/) excellent article to get started on how to write an emulator.  
I also followed [this](https://web.archive.org/web/20120313055436/http://www.dgames.org/beep-sound-with-sdl/) guide on how to make beep sounds with SDL because I had no idea how to do that.

