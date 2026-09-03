# witNES - Development Notes

## Overview

### Overview of NES structure

![image 55.png](image%2055.png)

1. CPU: 6502
2. Memory
3. APU: Audio Processing
4. PPU: very complex: Picture processing
5. VRAM: video
6. Graphics: Like graphics card
7. Palettes: color palettes (maybe?)
8. Mappers:

In older gaming consoles like the **Nintendo Entertainment System (NES)**, the hardware was limited in how much memory (ROM) it could access at once. To get around this limitation, **memory mappers** were used to make games larger and more complex.

Here's how it worked in simpler terms:

1. **Limited Memory**: The NES could only access a small portion of a game's memory at a time (32 KB for program data and 8 KB for graphics). But many games needed more memory than this.
2. **Bank Switching**: A **memory mapper** acted like a switchboard. It would swap different sections (or "banks") of the game's memory in and out, so the console could access more data without needing more physical memory.
    - Think of the game's memory like a book, but the NES could only read a few pages at a time.
    - The memory mapper allowed the NES to "switch" between different sets of pages when needed, even though it couldn't have all the pages open at once.
3. **Larger Games**: With this technique, games could be much larger than the NES itself could handle in one go, because the mapper allowed the system to load only what was needed at a specific time.

### Bit Fields

1. Bit fields are a feature in C++ for specifying the size of struct and class members so that they occupy a specific number of bits, not the default memory size.

Example:

```cpp
struct name{
    char x : 3 //use 3 bits for this member.
    int y : 6 //use 6 bits for this member.
}
```

2. The number of bits can be more than the member's type's default size.

3. Cannot have pointers to bit fields or references (&) **because they might not start at a byte boundary.**

4. Uses:
    - Memory optimization: uses less memory
    - Compress structs and data for easy transfer over network
    - In graphics applications, color components, pixel formats, and image data often have specific bit requirements which can be customized using bit fields.

### Unions

1. Same as structs, only difference is that all the members share the same memory location, instead of being assigned dedicated memory locations for each.

Example: Say we have the following struct:

```cpp
struct{
    char unu: 1;
    char sw2: 1;
    char sw1: 1;
}
```

And if we want to be able to set all these bits to be 0 at the same time, we can do:

```cpp
union{
    struct{
    char unu: 1;
    char sw2: 1;
    char sw1: 1;
    }
    
    char reg;
}

//we can just do:
union A;
A.reg = 0x0
```

---

## CPU

1. We have a 16 by 16 matrix of opcodes and instructions. The first byte's lower 4 bits represent the column in the matrix and the upper 4 bits represent the row.

2. The blank spots in the datasheets are all illegal opcodes. The CPU will still do something but not something it was designed to do officially.

3. ```cpp
   //first we did this
   
   struct INSTRUCTION{
       string name;
       uint8_t (wit6502::*operate)(void) = nullptr
   }
   
   
   //it expects a function pointer obviously.
   //When we define a member function IMM(), IMM is basically a pointer to the function
   //so we pass as &a::IMM
   /**
   Basically, & in the table is not reference. It is the address of operator. It basically gets the address of a::IMM and assigns it to the pointer 'operate'
   */
   
   //it is called like:
   
   (this->*lookup[opcode].addrmode)() 
   //inside the clock() function.
   
   ```

4. The clock function looks like this:
   (currently)
   ```cpp
   if(cycles == 0){
       opcode = read(pc)
       pc++
       
       cycles = lookup[opcode].cycles
       (this->*lookup[opcode].addrmode)() 
       (this->*lookup[opcode].operate)()
       
   }
   ```

---

I decided that I will implement all of this myself.

1. Why do we need clock? or cycles?
   What is their purpose?
   A: To synchronize all the components of the computer with the processor. If we did not have a clock or its cycles, signals would arrive at different components at different times.

### Addressing modes in the 6502

> **Reminder: Indirect addressing**
> Basically memory[address] contains the main address

| Abbr  | **Addressing mode** | **Formula**                                               | **What it does**                                                                                                                                                                                                                                            | **Cycles** |
| ----- | ------------------- | --------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| d,x   | Zero page indexed   | Peek((arg + X) % 256)                                     | all addresses are within the 00 - FF range and so form an index inside the 0 page (the first page) which is 256 bytes in 6502. This provides faster access since address is only 8 bits instead of 16                                                       | 4          |
| d,y   | Zero page indexed   | Peek((arg + Y) % 256)                                     | Same as above but with the Y register                                                                                                                                                                                                                       | 4          |
| a,x   | Absolute indexed    | Peek(arg + X)                                             | Simply add register X's value to the provided number to get the final address.                                                                                                                                                                              | 4+         |
| a,y   | Abs indexed         | Peek(arg + Y)                                             | Same as above but with Reg. Y                                                                                                                                                                                                                               | 4+         |
| (d,x) | Indexed indirect    | Peek(Peek((arg + X)%256) + Peek((arg+X + 1) % 256) * 256) | Both bytes of the address are stored in addresses in the zero page. We peek for the lower byte at (arg + X) % 256 and (arg + X + 1) % 256 for the higher byte. Multiplying by 256 shifts the higher byte 8 bits and both are added to form the full address | 6          |
| (d),y | Indirect indexed    | PEEK(PEEK(arg) + PEEK((arg+1) % 256) * 256 + Y)           | Basically the reverse of above, indirect address from zero page is calculated first and the index is added from the register later.                                                                                                                         | 5+         |

- `$deadbeef` is hexadecimal notation in 6502 lingo, `%` is used for binary

> **Note on `arg`**
> It is important to note that the value of `arg` is `$FF` at max in all indexed addressing modes except absolute addressing. That is basically how indirect indexed addressing guarantees that the first peek will be in the zero page as well.

#### Other simple addressing modes

| Abbr  | Name        | Description                                                                                                                                                     |
| ----- | ----------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|       | Implicit    | Destination of results are implied and addressing mode is not really relevant for operands here. EX: `RTS (return from subroutine) and CLC (set carry or something)` |
| A     | Accumulator | Instructions can operate on the accumulator, e.g. LSR A                                                                                                         |
| #v    | Immediate   | Use the 8 bit value provided itself                                                                                                                             |
| d     | Zero page   | Fetches value from an 8 bit address on the 0 page                                                                                                                |
| a     | Absolute    | Use the 16 bit address to fetch from anywhere in the memory                                                                                                     |
| label | Relative    | Branch instructions and jumps have a relative offset operand from the current PC (8 bit)                                                                        |
| (a)   | Indirect    | Find the actual jump address at the 16 bit pointer operand.                                                                                                     |

### What exactly is the FLAGSTAT Enum doing?

> On the 6502, all CPU status flags live in a single byte (the `P` register), and we represent each flag as a **bitmask enum** so we can manipulate individual bits efficiently, just like real hardware; for example:
>
> ```cpp
> enum FLAGSTAT { C = 1<<0, Z = 1<<1, I = 1<<2, B = 1<<4, U = 1<<5, V = 1<<6, N = 1<<7 };
> uint8_t status = 0;                    // actual CPU status register
> void setFlag(FLAGSTAT f, bool v) {     // set or clear a flag
>     if(v) status |= f; else status &= ~f;
> }
> uint8_t getFlag(FLAGSTAT f) {          // read a flag
>     return (status & f) ? 1 : 0;
> }
>
> // Usage:
> setFlag(C, true);   // turn Carry on
> setFlag(Z, false);  // turn Zero off
> bool negative = getFlag(N);
> ```
>
> **Key idea:** the enum values are just **bit masks**, `status` is the actual storage, and all flag operations are **bitwise**, exactly imitating how the CPU stores and manipulates flags in one byte.
>
> - We could have used variables instead of the enum to store these bit masks, but having them as enums provides type safety and localization? (FLAGSTAT)

### My mental flow of how instructions will be executed

The clock() function is called from outside and represents a tick. Every time it is called, the cycle variable is decreased by 1. Every time the cycle variable reaches 0:

**Step 1**: CPU grabs the opcode byte at PC.
**Step 2**: The corresponding `baseInstruction()` function is called immediately using the Instruction struct.
**Step 3**: This `baseInstruction()` in turn calls the `addrMode()` function, which handles the inner workings of operand retrieval, etc. depending upon the type of operand it is working on (decided by addrMode). For all 16 bit operands the addrMode() handles all the logic, while for all 8 bit operands, `fetch` is the intermediate function that is called and puts the final 8 bit value into the `fetched` variable.

> **addrMode function reference:**
> ```
> imm = #$00  
> zp  = $00  
> zpx = $00,X  
> zpy = $00,Y  
> izx = ($00,X)  
> izy = ($00),Y  
> abs = $0000  
> abx = $0000,X  
> aby = $0000,Y  
> ind = ($0000)  
> rel = $0000 (PC-relative)
> ```

- Addrmode functions should only return the respective address to read from/write to, not perform data read at the address. `addr_main` is the variable where the address will be stored by the addrmode functions to be used later by the respective operate functions. (they can still read/write if the memory contents are actually address bytes)

- The stack pointer register is 8 bits only because the stack is defined by page 1 of the memory ($01). So this remains the high byte the whole time.

- There are actually 2 buses in the NES. One for the CPU to the cartridge and one for the PPU to the cartridge.

- There are many types of mappers. I decided to go with 000 (NROM - 128/256) for starters. Should support most early games.

- For mirroring regions, look at masking to roll back to the original region.

- Unfortunately, because of a CPU bug, if this 2-byte variable has an address ending in $FF and thus crosses a page, then the CPU fails to increment the page when reading the second byte and thus reads the wrong address. For example, JMP ($03FF) reads $03FF and _$0300_ instead of $0400. This must be reflected in accurate emulation.

---

## PPU

- 8 registers memory mapped and exposed to the CPU ($2000 - $2007) (mirrored from $2008 - $3FFF)

- Sprites and backgrounds are created off of a fundamental graphic unit called a tile. A tile is 8x8 pixels. Each pixel is defined by 2 bits (1 from plane 0 and 1 from plane 1):
    - 00 - transparent
    - 01 - color 1 from palette
    - 10 - color 2 from palette
    - 11 - color 3 from palette

- And so any tile can only have 3 colors at max.

### Nametables

- Store tiles for the background of 1 screen (256 x 240 pixels).

The NES screen can scroll. To scroll smoothly, the PPU needs more than one screen's worth of background data: so it has 4 logical nametable slots arranged in a 2x2 grid (two screens wide, two screens tall). This gives us a larger "world" to scroll around in.

But the NES only has enough physical RAM (CIRAM) for 2 nametables, not 4. So two of the four logical slots have to be mirrors of the other two. Which ones mirror which depends on the game's scrolling direction:

- **Horizontal mirroring**: for games that scroll vertically (like Kid Icarus). Left and right nametables are the same.
- **Vertical mirroring**: for games that scroll horizontally (like Super Mario Bros). Top and bottom nametables are the same.

That's all it's saying. For NROM I just need to implement one of these two mirroring modes based on the flag in `header[6]` that I already read. I don't need to worry about four-screen or mapper-controlled mirroring for now.

> **So how does the PPU render one scanline (left to right) of the background?**
>
> - Fetch a nametable from 0x2000-0x2FFF (which tiles)
> - Fetch the corresponding attribute table entry from $23C0-$2FFF and increment the current VRAM address within the same row.
> - Fetch the low-order byte of an 8x1 pixel sliver of the pattern table from $0000-$0FF7 or $1000-$1FF7.
> - Fetch the high-order byte of this sliver from an address 8 bytes higher.
> - Turn the attribute data and the pattern table data into palette indices, and combine them with data from [sprite data](https://www.nesdev.org/wiki/PPU_sprite_evaluation "PPU sprite evaluation") using [priority](https://www.nesdev.org/wiki/PPU_sprite_priority "PPU sprite priority").

### High level flow of PPU

![PPU flow diagram](Pasted%20image%2020260722211932.png)

### Note on timing

In the actual hardware, the CPU and PPU tick almost at the same time, but in serialization of this in software it is not possible to do this accurately for me at the time of writing due to time constraints.

An educated guess is that most NROM games will not depend on these bleeding edge timing cases, and if they are encountered, I will implement edge case handling for those situations specifically (change the order of ticks accordingly).

In the future, I may create a masterclock class that controls the ticks for both components depending on the last tick, the upcoming instruction, and the current state.

For the first implementation, I will go with this tick order:


ppu.clock();
ppu.clock();
ppu.clock();
cpu.clock();


[A Visible Glitch](https://www.nesdev.org/wiki/PPU_registers#:~:text=backdrop%20override.-,Bit%200%20race%20condition,-Be%20careful%20when) that happens because of certain CPU-PPU alignments and timing edge cases causes some visual bugs in some games. I will not emulate this behavior (at least, not as of right now).

### Scrolling

There are some internal registers: `x`, `v`, `t`

Imagine one screen as a "camera". This camera can be moved anywhere in the 2D space within the 4 nametables.
Scanlines and dots only take care of the counts for the current frame. They do not track position through the different screens. To track position we have scrolling.

Mainly 5 components:
- nametable bits: which nametable the current tile is in
- coarse_x: how many tiles the camera has moved horizontally from the original position (original NN bits set by CPU in PPUCTRL)
- coarse_y: same as above but vertically
- fine_x: how many pixels the camera has moved within the coarse_x tile horizontally (pixel level detail)
- fine_y: how many pixels the camera has moved within the tile vertically (changes every scanline we render)

Conceptual flow:
- CPU sets PPUSCROLL before each frame. This ultimately sets the staging register `t` with the values of the above variables.
- Before rendering starts, this `t` value is copied into `v`.
- `v` contains the current nametable bits, coarse_x, coarse_y, fine_y.
- `x` contains fine_x.
- When we do nametable and pattern table fetches, we use coarse_x and coarse_y to get the correct tile number from the nametable. Then we use fine_y to get the right row (vertically) within the tile.
- We don't take into account fine_x while grabbing the row, so we grab the full 8 bits of the tile from the pattern table and load into the shift registers.
- Finally when we render, we use fine_x to offset into the shift registers (if the camera is 3 pixels into the current tile, use bits from the 12th bit of the register).

**Transitions and wraparound system:**
- At each 8 dot boundary, we increase coarse_x and check if increasing it makes it >= 32 (out of the current nametable horizontally). If yes, then we flip the horizontal nametable bit in `v` and reset coarse_x to 0. Otherwise, we just increase coarse_x by 1.
- At dot 256 we also increment fine_y and then check if it is overflowing past 7 (vertical tile boundary). If that happens, it is reset to 0 and coarse_y is incremented by 1, then if coarse_y is out of the 0-29 range (vertically over the current nametable) we flip the vertical nametable bit in `v` and reset coarse_y to 0 also.
- At dot 257 (after rendering), we have to set v = t (only coarse_x and horizontal nametable bits) again to bring the rendering back to where it was horizontally.

The documentation says:

> "The PPU uses the current VRAM address for both reading and writing PPU memory thru \$2007, and for fetching nametable data to draw the background. As it's drawing the background, it updates the address to point to the nametable data currently being drawn. Bits 10-11 hold the base address of the nametable minus $2000. Bits 12-14 are the Y offset of a scanline within a tile."

When it says current VRAM address for nametable fetches, it may sound confusing, but if we really think about it, what are VRAM addresses? They are a combination of nametable bits, coarse_x, coarse_y. They're addresses into the nametables. When the CPU uses it outside of rendering, however, `v` and `t` become general purpose addressing registers into the full PPU address space for the CPU.

The 15 bit registers _t_ and _v_ are composed this way during rendering:

```
yyy NN YYYYY XXXXX
||| || ||||| +++++-- coarse X scroll
||| || +++++-------- coarse Y scroll
||| ++-------------- nametable select
+++----------------- fine Y scroll
```


### Rendering current dot

#### Background pixel

5 bits total:
- 2 from pattern high and low
- 2 from attribute high and low
- 1 from mux (sprite 1, bg 0)

`S AA PP`

Then the flow is: palette RAM ($3F00-$3F1F) stores a 6-bit index (0-63) which looks up into my 64-color master palette array, which gives me the actual RGB to put on screen. Two levels of indirection between "what the game writes" and "what pixel color appears."

### Accessing PPUDATA during rendering

Technically possible, but it is an edge case that some tricky-to-emulate games use. Emulation of this is straightforward, but I will not implement it at this time (or ever).

### SDL2 Notes

*(to be filled in)*
