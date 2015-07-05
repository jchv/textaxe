# TextAxe
TextAxe is a simple text editor. It is designed to act as a general purpose text editor, without any specific use case in mind.

## Features

  * Basic network transparency
  * Automatic charset detection
  * Basic ability to render entire documents to images
  * Bundles a monospace font for code editing
  * Bundles a MS PGothic-compatible font (Mona) for viewing 2ch Shift-JIS art :)

## State
TextAxe is incomplete. It should work for the basic usages it's intended to be used for, but you might find issues. In particular, the network transparency may not be very stable or usable, as it isn't very well tested.

TextAxe lacks any sort of statefulness. It does not remember recently opened files, nor does it remember the font you last selected.

This code does not contain any unit testing (yet.) There is also not any continous integration yet, but I do plan on at least setting up builds with Travis.

## Compiling
TextAxe was written using Qt Framework and should compile on Qt 5.4. Earlier versions of Qt may also work, but due to bugs in older versions of Qt, they are not recommended. In particular, there are two now-solved bugs that can impede your ability to save documents, which is obviously a pretty big problem. Rather than build in work-arounds, this code simply assumes you have read the README to be aware that older versions of Qt won't work right.

TextAxe uses C++11, but will compile under Microsoft Visual Studio 2013 or higher. It should also compile under GCC, Clang and for most OSes supported by Qt (If it does not, feel free to open a ticket.)
