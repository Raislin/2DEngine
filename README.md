# 2DEngine
A 2D Game Engine using SFML now migrated to a public github.  Design largely from the first half of "SFML Game Design by Example" but with the code almost entirely rewritten to be more memory safe and utilize more C++11, 14, and 17 features.

A sample game is included in sample, using the same resources and levels provided in the aforementioned book.  Custom maps are simple to create, but as I plan to use a completely different format in the future
(described below), I have no plans on how to document it.  You may use one of the existing levels and the map loading method in src/Map.cpp to get the gist of it.

Supports Windows and Linux.  SFML 2.5.1 required.  To compile in Linux, use the typical CMake and Make commands.  CMake configuration is not currently setup to support Windows, so it will need to be compiled manually.

Additionally, you will need to set the appropriate preprocessor definition in Utilities.h.  I have plans to use C++ std::filesystem to replace Utilities.h in the future.


There are some problems with the current design and I will eventually rewrite most of this to improve on it.  

- Physics are tied to the rendering, resulting in speed ups and slow downs based on the frame rate.  
- I also take issue with the map format.  It's sufficed for the learning experience and getting a game up and running, but I have plans to build a more robust format and a custom editor for it.  I am also considering
using Tiled.  
- Game specific classes are intermixed with more generic engine specific classes.  It's also difficult or impossible to separate the two right now.
- There are other issues, but these are the ones that I plan to tackle first.