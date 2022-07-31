# Dota2MT HappyFun 🤣

![](https://user-images.githubusercontent.com/109766416/182042714-d9ae78cb-efdf-46f2-961f-57ada1d790fa.png)
It has been built from the ground-up to be faster and better than previous cheats.

## Features List
<details>
<summary>Features (Drop Down)</summary>
  
## Feature Menu
  * Fully Dynamic Panorama UI that is created on runtime and modifies no files.
  * Integrity System that tries to detect when and where updates break the cheat.
  * All features are native C++ 
  * No ConVars are changed ever
  * Lua Execution ( implemented but not used, see ( ``vscriptSystem->RunScript`` )
  * Camera Zoom, pitch/yaw
  * ESP, spotted ESP ( when seen by enemy team ), info.
  * No Fog / No Fog of War ( not a maphack )
  * Full Protobuf packet interception/inspection/editing.
  * More...
  
  </details>
  
## System Requirements
  * CMake
  * a C++17 compiler ( like ``clang`` or ``gcc`` )  
  * Build essentials( make, ``gdb``, etc.. )
  * [google protobufs](https://github.com/protocolbuffers/protobuf/archive/v3.7.0.tar.gz) development library - Version 3.7.0 is recommended! The Newest versions are not compatible and will segfault even in proto2 mode!
  
  
## Build Instructions
(Pre-Requisite)Building Protobufs
```
First, uninstall any protobuf-devel package your distro may have. (Just the headers/libs don't remove Gnome)

git clone https://github.com/protocolbuffers/protobuf
git checkout v3.15.3
./autogen.sh && ./configure
make -j8 CFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0
cd src
sudo make install
sudo ldconfig #refresh .so cache

Check in the terminal the Version is correct.
[gamer@localhost Dota2MT-Master]$ protoc --version
libprotoc 3.15.3
```
First Build Protobufs ``./rebuildprotos.sh``
Now you can build the project ``cmake . && make -j``
For a Debug Build ``cmake . -DCMAKE_BUILD_TYPE="Debug" . && make -j``

## Screenshots
![dotah1](https://user-images.githubusercontent.com/109766416/182043043-c5923117-611b-417e-8c32-1f02a50922ea.png)

![dotah2](https://user-images.githubusercontent.com/109766416/182043048-2b675c73-d8c7-4765-93ef-c8055e33dbd5.png)



## Known Issues
  * There is a very rare tcmalloc bug I have encountered that will just crash your game. I can't seem to reproduce it when it happens.
  * Sometimes the UI will not open on the first time, I think I have fixed this, but if this happens to you, check the console and just try again.
  * Debug builds do not unload
  * SELinux might cause a problem with hardhooks
  
## Credits
[@orangmuda](https://github.com/labsbots) - His research into the SchemaSystem helped me especially when I was starting the project.

[@imhunterand](https://github.com/imhunterand) - I Use a modified version of this library to do the Hard Hooks used for GC msgs ( changing .code section )

