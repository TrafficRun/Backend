# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ink19/TrafficPlatform/TrafficRun

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ink19/TrafficPlatform/TrafficRun/build

# Include any dependencies generated for this target.
include CMakeFiles/TrafficRun.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/TrafficRun.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/TrafficRun.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TrafficRun.dir/flags.make

CMakeFiles/TrafficRun.dir/main.cpp.o: CMakeFiles/TrafficRun.dir/flags.make
CMakeFiles/TrafficRun.dir/main.cpp.o: ../main.cpp
CMakeFiles/TrafficRun.dir/main.cpp.o: CMakeFiles/TrafficRun.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ink19/TrafficPlatform/TrafficRun/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TrafficRun.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TrafficRun.dir/main.cpp.o -MF CMakeFiles/TrafficRun.dir/main.cpp.o.d -o CMakeFiles/TrafficRun.dir/main.cpp.o -c /home/ink19/TrafficPlatform/TrafficRun/main.cpp

CMakeFiles/TrafficRun.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TrafficRun.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ink19/TrafficPlatform/TrafficRun/main.cpp > CMakeFiles/TrafficRun.dir/main.cpp.i

CMakeFiles/TrafficRun.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TrafficRun.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ink19/TrafficPlatform/TrafficRun/main.cpp -o CMakeFiles/TrafficRun.dir/main.cpp.s

CMakeFiles/TrafficRun.dir/core/config.cpp.o: CMakeFiles/TrafficRun.dir/flags.make
CMakeFiles/TrafficRun.dir/core/config.cpp.o: ../core/config.cpp
CMakeFiles/TrafficRun.dir/core/config.cpp.o: CMakeFiles/TrafficRun.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ink19/TrafficPlatform/TrafficRun/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TrafficRun.dir/core/config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TrafficRun.dir/core/config.cpp.o -MF CMakeFiles/TrafficRun.dir/core/config.cpp.o.d -o CMakeFiles/TrafficRun.dir/core/config.cpp.o -c /home/ink19/TrafficPlatform/TrafficRun/core/config.cpp

CMakeFiles/TrafficRun.dir/core/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TrafficRun.dir/core/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ink19/TrafficPlatform/TrafficRun/core/config.cpp > CMakeFiles/TrafficRun.dir/core/config.cpp.i

CMakeFiles/TrafficRun.dir/core/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TrafficRun.dir/core/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ink19/TrafficPlatform/TrafficRun/core/config.cpp -o CMakeFiles/TrafficRun.dir/core/config.cpp.s

CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o: CMakeFiles/TrafficRun.dir/flags.make
CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o: ../core/gameenv.cpp
CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o: CMakeFiles/TrafficRun.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ink19/TrafficPlatform/TrafficRun/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o -MF CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o.d -o CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o -c /home/ink19/TrafficPlatform/TrafficRun/core/gameenv.cpp

CMakeFiles/TrafficRun.dir/core/gameenv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TrafficRun.dir/core/gameenv.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ink19/TrafficPlatform/TrafficRun/core/gameenv.cpp > CMakeFiles/TrafficRun.dir/core/gameenv.cpp.i

CMakeFiles/TrafficRun.dir/core/gameenv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TrafficRun.dir/core/gameenv.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ink19/TrafficPlatform/TrafficRun/core/gameenv.cpp -o CMakeFiles/TrafficRun.dir/core/gameenv.cpp.s

# Object files for target TrafficRun
TrafficRun_OBJECTS = \
"CMakeFiles/TrafficRun.dir/main.cpp.o" \
"CMakeFiles/TrafficRun.dir/core/config.cpp.o" \
"CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o"

# External object files for target TrafficRun
TrafficRun_EXTERNAL_OBJECTS =

TrafficRun: CMakeFiles/TrafficRun.dir/main.cpp.o
TrafficRun: CMakeFiles/TrafficRun.dir/core/config.cpp.o
TrafficRun: CMakeFiles/TrafficRun.dir/core/gameenv.cpp.o
TrafficRun: CMakeFiles/TrafficRun.dir/build.make
TrafficRun: CMakeFiles/TrafficRun.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ink19/TrafficPlatform/TrafficRun/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable TrafficRun"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TrafficRun.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TrafficRun.dir/build: TrafficRun
.PHONY : CMakeFiles/TrafficRun.dir/build

CMakeFiles/TrafficRun.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TrafficRun.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TrafficRun.dir/clean

CMakeFiles/TrafficRun.dir/depend:
	cd /home/ink19/TrafficPlatform/TrafficRun/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ink19/TrafficPlatform/TrafficRun /home/ink19/TrafficPlatform/TrafficRun /home/ink19/TrafficPlatform/TrafficRun/build /home/ink19/TrafficPlatform/TrafficRun/build /home/ink19/TrafficPlatform/TrafficRun/build/CMakeFiles/TrafficRun.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TrafficRun.dir/depend

