# Ledge Detection & Blocking System for Unreal Engine 5.3+

This is a lightweight C++ system to prevent your character from walking off high ledges.  
It's simple, optimized, and works out of the box — no plugins or extra setup required.

## What It Does
- Casts line traces in front of the character
- Checks if the player is about to walk off a ledge
- Blocks forward movement if a fall is detected
- All values are tweakable in the header file

## How to Use

Everything is inside the `LedgeCharacter.cpp` file.
If you want to copy the code to your own character class, just copy the part between the following comments:

///////// COPY FROM HERE START /////////
... code ...
///////// COPY FROM HERE END /////////

Make sure to **replace the actor/class name** where necessary to match your character.

If you have any questions or run into issues, feel free to reach out to me on Discord:  
**@lootera89** I'll be happy to help.


#License: MIT
This project is licensed under the **MIT License** – short, simple, and permissive.
You’re free to **use**, **modify**, **share**, and **build** whatever you want with it — for personal, educational, or commercial projects. No strings attached.
