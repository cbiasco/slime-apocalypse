# slime-apocalypse
"Run away from crowds of 3D deformable slimes in real-time..."
This was our original intent! Currently, it's more of a (bland) demonstration of our implementation of 3D ChainMail.

# Authors
Dalton Hildreth

Caleb Biasco

Nathan Heller

# Build

To Init:
```
git clone --recursive https://github.com/cbiasco/slime-apocalypse.git
```

If you do not have Python installed on your computer or the Python executable is not a PATH variable, then you may have build issues. An alternative to those options is to go to http://glad.dav1d.de/ and generate a loader with the following specifications:

	Language = C/C++
	
	Specification = OpenGL
	
	API = GL 3.3
	
	Profile = Compatibility
	
	Extensions = ADD ALL (for simplicity's sake)
	
Once you have generated the loader, add the src and include directories under {root}/Libraries/glad and things should run from there.

To Build (inside of a Build folder under the root directory):
```
cd Build
# UNIX Makefile (NOTE: not guarenteed to work)
# Although, this might work anywhere.
cmake .. 

# Mac OSX (NOTE: not guarenteed to work)
cmake -G "Xcode" ..

# Microsoft Windows
cmake -G "Visual Studio 15" ..
# Then build the generated .sln file
```
