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
CMAKE_COMMAND = C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/tool/bin/cmake.exe

# The command to remove a file.
RM = C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/tool/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec

# Utility rule file for or32_crt0.

# Include the progress variables for this target.
include project/codec/CMakeFiles/or32_crt0.dir/progress.make

project/codec/CMakeFiles/or32_crt0: lib/sm32/or32_crt0.o


lib/sm32/or32_crt0.o: ../../../project/codec/or32_crt0.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ../../lib/sm32/or32_crt0.o"
	cd C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/project/codec && C:/ITEGCC/bin/sm32-elf-gcc.exe -DCFG_CHIP_REV_A1 -DCFG_CHIP_FAMILY=970 -DCFG_RAM_SIZE=0x4000000 -DCFG_MEMDBG_ENABLE -DCFG_WT_SIZE=0 -DCFG_MMAP_SIZE=0 -DCFG_LCD_WIDTH=0 -DCFG_LCD_HEIGHT=0 -DCFG_LCD_PITCH=0 -DCFG_LCD_BPP=0 -DCFG_CMDQ_SIZE=0 -DCFG_FONT_FILENAME="WenQuanYiMicroHeiMono.ttf" -DCFG_AUDIO_ENABLE -DAUDIO_PLUGIN_MESSAGE_QUEUE -DENABLE_CODECS_PLUGIN -DCONFIG_ITADRIVER -DCFG_RISC_ENABLE -DCFG_AUDIO_SAMPLING_RATE=8000 -DCFG_AEC_DELAY_MS=60 -DCFG_SPEAKER_GAIN=0 -DCFG_MIC_GAIN=0 -DCFG_NOISE_GATE_THRESHOLD=0.3 -DCFG_EQUALIZER_SPKGAIN_SET=NULL -DCFG_EQUALIZER_MICGAIN_SET=NULL -DCFG_AUDIO_CODEC_AACDEC -DCFG_AUDIO_CODEC_AMR -DCFG_AUDIO_CODEC_EAC3DEC -DCFG_AUDIO_CODEC_MP3DEC -DCFG_AUDIO_CODEC_WAV -DCFG_AUDIO_CODEC_WMADEC -DCFG_AUDIO_CODEC_SBC -DCFG_AUDIO_CODEC_MP2ENC -DCFG_AUDIO_CODEC_AACENC -DCFG_PRIVATE_DRIVE="A" -DCFG_PUBLIC_DRIVE="B" -DCFG_TEMP_DRIVE="C" -DCFG_BACKUP_DRIVE="" -DCFG_RTC_DEFAULT_TIMESTAMP=1325376000 -DCFG_INTERNAL_RTC_TIMER= -DCFG_GPIO_ETHERNET_LINK=-1 -DCFG_I2S_USE_GPIO_MODE_2=y -DCFG_NET_ETHERNET_PHY_ADDR=-1 -DCFG_NET_ETHERNET_COUNT=1 -DCFG_DBG_PRINTBUF_SIZE=0x10000 -DCFG_DBG_PRINTBUF -DCFG_UPGRADE_FILENAME="ITEPKG03.PKG" -DCFG_UPGRADE_FILENAME_LIST="" -DCFG_UPGRADE_ENC_KEY=0 -DCFG_NORMAL_BUFFER_MODE -DCFG_BUILD_RELEASE -DCFG_VERSION_MAJOR=0 -DCFG_VERSION_MINOR=1 -DCFG_VERSION_PATCH=0 -DCFG_VERSION_CUSTOM=0 -DCFG_VERSION_TWEAK= -DCFG_VERSION_MAJOR_STR="0" -DCFG_VERSION_MINOR_STR="1" -DCFG_VERSION_PATCH_STR="0" -DCFG_VERSION_CUSTOM_STR="0" -DCFG_VERSION_TWEAK_STR="" -DCFG_SYSTEM_NAME="ITE Castor3" -DCFG_MANUFACTURER="www.ite.com.tw" -DCFG_PROJECT_NAME="codec" -D__DYNAMIC_REENT__ -D__OPENRTOS__=1 -D_DEFAULT_SOURCE -D_GNU_SOURCE -D_POSIX_BARRIERS -D_POSIX_C_SOURCE=200809L -D_POSIX_MONOTONIC_CLOCK -D_POSIX_PRIORITY_SCHEDULING=1 -D_POSIX_READER_WRITER_LOCKS -D_POSIX_THREADS -D_POSIX_TIMERS -D_UNIX98_THREAD_MUTEX_ATTRIBUTES -DOSIP_MT -DORTP_BIGENDIAN -DCFG_OPENRTOS_HEAP_SIZE=0 -DENABLE_CODECS_PLUGIN=1 -DENABLE_WIEGAND_PLUGIN=1 -IC:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk//include/or32 -c -o C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/lib/sm32/or32_crt0.o C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/project/codec/or32_crt0.S

or32_crt0: project/codec/CMakeFiles/or32_crt0
or32_crt0: lib/sm32/or32_crt0.o
or32_crt0: project/codec/CMakeFiles/or32_crt0.dir/build.make

.PHONY : or32_crt0

# Rule to build all files generated by this target.
project/codec/CMakeFiles/or32_crt0.dir/build: or32_crt0

.PHONY : project/codec/CMakeFiles/or32_crt0.dir/build

project/codec/CMakeFiles/or32_crt0.dir/clean:
	cd C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/project/codec && $(CMAKE_COMMAND) -P CMakeFiles/or32_crt0.dir/cmake_clean.cmake
.PHONY : project/codec/CMakeFiles/or32_crt0.dir/clean

project/codec/CMakeFiles/or32_crt0.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/project/codec C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/project/codec C:/Maksim_user/DB_C10/20190215_Ctrlboard_SDK_v2.3.2.1_d22119/ite_sdk/build/openrtos/codec/project/codec/CMakeFiles/or32_crt0.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : project/codec/CMakeFiles/or32_crt0.dir/depend
