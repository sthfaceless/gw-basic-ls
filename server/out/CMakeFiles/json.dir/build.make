# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/danil/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/203.5981.166/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/danil/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/203.5981.166/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/danil/CLionProjects/gw_basic_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/danil/CLionProjects/gw_basic_server/server/out

# Include any dependencies generated for this target.
include CMakeFiles/json.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/json.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/json.dir/flags.make

CMakeFiles/json.dir/server/src/json.c.o: CMakeFiles/json.dir/flags.make
CMakeFiles/json.dir/server/src/json.c.o: ../src/json.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/danil/CLionProjects/gw_basic_server/server/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/json.dir/server/src/json.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/json.dir/server/src/json.c.o   -c /home/danil/CLionProjects/gw_basic_server/server/src/json.c

CMakeFiles/json.dir/server/src/json.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/json.dir/server/src/json.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/danil/CLionProjects/gw_basic_server/server/src/json.c > CMakeFiles/json.dir/server/src/json.c.i

CMakeFiles/json.dir/server/src/json.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/json.dir/server/src/json.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/danil/CLionProjects/gw_basic_server/server/src/json.c -o CMakeFiles/json.dir/server/src/json.c.s

# Object files for target json
json_OBJECTS = \
"CMakeFiles/json.dir/server/src/json.c.o"

# External object files for target json
json_EXTERNAL_OBJECTS =

libjson.a: CMakeFiles/json.dir/server/src/json.c.o
libjson.a: CMakeFiles/json.dir/build.make
libjson.a: CMakeFiles/json.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/danil/CLionProjects/gw_basic_server/server/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libjson.a"
	$(CMAKE_COMMAND) -P CMakeFiles/json.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/json.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/json.dir/build: libjson.a

.PHONY : CMakeFiles/json.dir/build

CMakeFiles/json.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/json.dir/cmake_clean.cmake
.PHONY : CMakeFiles/json.dir/clean

CMakeFiles/json.dir/depend:
	cd /home/danil/CLionProjects/gw_basic_server/server/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/danil/CLionProjects/gw_basic_server /home/danil/CLionProjects/gw_basic_server /home/danil/CLionProjects/gw_basic_server/server/out /home/danil/CLionProjects/gw_basic_server/server/out /home/danil/CLionProjects/gw_basic_server/server/out/CMakeFiles/json.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/json.dir/depend

