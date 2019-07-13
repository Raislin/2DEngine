# 2DEngine
A 2D Game Engine using SFML.  Design largely from the first half of "SFML Game Design by Example" but with the code heavily revised to be more memory safe and utilize more C++11, 14, and 17 features.

A sample game is included in Sample, using the same resources and levels provided in the aforementioned book.

Supports Windows and Linux.  SFML 2.5.1 required.  To compile in Linux, use the typical CMake and Make commands.  CMake configuration is not currently setup to support Windows, so it will need to be compiled manually.

Additionally, you will need to set the appropriate preprocessor definition in Utilities.h.  I have plans to use C++ std::filesystem to replace Utilities.h in the future.


There are some problems with the current design and I will eventually rewrite most of this to improve on it.  

- Physics are tied to the rendering, resulting in speed ups and slow downs based on the frame rate.  
- I also take issue with the map format.  It's sufficed for the learning experience and getting a game up and running, but I have plans to build a more robust format and a custom editor for it.  I am also considering
using Tiled.  
- Game specific classes are intermixed with more generic engine specific classes.  It's also difficult or impossible to separate the two right now.
- There are other issues, but these are the ones that I plan to tackle first.