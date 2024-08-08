# InterpolationStorage
Allows to store data automatically interpolate it.

# Why using this?
1. If you have a lot of data to draw you can "thin out" your data using interpolation.
   You just store your data in the InterpolationStorage and iterate with your custom step and always get data.
2. If you have a small amount of data but you need a more data.
   You can generate needed data using interpolation.
   For example you have images with 30 frames per second but you need 60.
3. If you measure a device orientation and you need to smooth it.
4. If you have to interpolate an entire coordinate frames
5. A lot more

# Designed with this in mind
Pure C++ 20 without any dependencies<br>
Absolutely crossplatform<br>
Build with a single command<br>
Absolutely customizable interpolator<br>

# Build from cloned repo directory
clang++ -std=c++20 main.cpp -o executable
