# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/tool/bin/cmake.exe

# The command to remove a file.
RM = C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/tool/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu

# Include any dependencies generated for this target.
include sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/depend.make

# Include the progress variables for this target.
include sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/progress.make

# Include the compile flags for this target's objects.
include sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/flags.make

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/flags.make
sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj: ../../../sdk/alt_cpu/oledCtrl/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj"
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/oledCtrl.plugin.dir/main.c.obj   -c C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/sdk/alt_cpu/oledCtrl/main.c

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/oledCtrl.plugin.dir/main.c.i"
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/sdk/alt_cpu/oledCtrl/main.c > CMakeFiles/oledCtrl.plugin.dir/main.c.i

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/oledCtrl.plugin.dir/main.c.s"
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/sdk/alt_cpu/oledCtrl/main.c -o CMakeFiles/oledCtrl.plugin.dir/main.c.s

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.requires:

.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.requires

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.provides: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.requires
	$(MAKE) -f sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/build.make sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.provides.build
.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.provides

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.provides.build: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj


# Object files for target oledCtrl.plugin
oledCtrl_plugin_OBJECTS = \
"CMakeFiles/oledCtrl.plugin.dir/main.c.obj"

# External object files for target oledCtrl.plugin
oledCtrl_plugin_EXTERNAL_OBJECTS =

sdk/alt_cpu/oledCtrl/oledCtrl.plugin: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj
sdk/alt_cpu/oledCtrl/oledCtrl.plugin: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/build.make
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable oledCtrl.plugin"
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-gcc.exe  -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -Wno-unused-result -gstabs -O3 -g3 -mno-delay -msoft-div -msoft-float -msoft-mul -mno-ror -mno-cmov -mno-sext -fomit-frame-pointer -fno-short-enums -DNDEBUG   -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -Wno-unused-result -gstabs -nostartfiles -T"C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/default.ld" "C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/alt_cpu_crt0.o" "C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/alt_cpu_ticktimer.o" "C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/alt_cpu_gpio.o" $(oledCtrl_plugin_OBJECTS) $(oledCtrl_plugin_EXTERNAL_OBJECTS)  -o oledCtrl.plugin  -LC:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/lib/sm32  -LC:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32 
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-objcopy.exe -O binary C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl/oledCtrl.plugin C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/oledCtrl.raw
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && C:/ITEGCC/bin/sm32-elf-objdump.exe -D C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl/oledCtrl.plugin > C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/oledCtrl.dbg
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && sm32-elf-readelf -a C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl/oledCtrl.plugin > C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl/oledCtrl.symbol
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && dataconv -x C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/lib/sm32/oledCtrl.raw

# Rule to build all files generated by this target.
sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/build: sdk/alt_cpu/oledCtrl/oledCtrl.plugin

.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/build

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/requires: sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/main.c.obj.requires

.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/requires

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/clean:
	cd C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl && $(CMAKE_COMMAND) -P CMakeFiles/oledCtrl.plugin.dir/cmake_clean.cmake
.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/clean

sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/sdk/alt_cpu/oledCtrl C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl C:/workspace/IT970_Series/Software_Design_Kit/SDK_R/set20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/alt_cpu/sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : sdk/alt_cpu/oledCtrl/CMakeFiles/oledCtrl.plugin.dir/depend

