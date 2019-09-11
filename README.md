# ncTemplate
A template project for applications and games made with the nCine.

Most of the projects will work with the amount of customization offered by this script system.
They will be able to target all engine supported platforms: Linux, macOS, Windows, MinGW, Android and Emscripten.

## Customization

### Package information

The main CMake script to modify for your project is `cmake/package_info.cmake`.

It exposes a number of different variables that will be used by the rest of the script system:

- **PACKAGE_NAME**: The name of your project
- **PACKAGE_EXE_NAME**: The name of the executable file for your project (without any extension).  
  If you don't set it the default will be a lower case version of `${PACKAGE_NAME}`.
- **PACKAGE_VENDOR**: You can set it to your company name or your personal name (used by installers).
- **PACKAGE_COPYRIGHT**: The copyright dates and notes for the project.
- **PACKAGE_DESCRIPTION**: A short description of your project (used by installers).
- **PACKAGE_HOMEPAGE**: The URL of your project homepage (used by installers).
- **PACKAGE_REVERSE_DNS**: The name of your project expressed in reverse domain name notation.  
  It is used for the XDG Desktop file on Linux, the Bundle Identifier on macOS and for the java package namespace on Android.
- **PACKAGE_INCLUDE_DIRS**: If your project uses one or more separate include directories for headers you can specify them with this variable.
  The paths to the include directories can be absolute or relative to `${CMAKE_SOURCE_DIR}.`
- **PACKAGE_SOURCES**: The list of source files that will be compiled.  
  The paths to the source files can be absolute or relative to `${CMAKE_SOURCE_DIR}.`
- **PACKAGE_ANDROID_ASSETS**: A list of files that will become Android assets and part of your project APK archive.  
  The paths to the asset files can be absolute or relative to `${PACKAGE_DATA_DIR}`.

This file also supports the declaration of some callback functions that will be called in the main `CMakeLists.txt` script:
- **callback_start()**: This function will be called at the very beginning, when the CMake `project()` command has not yet been called.
- **callback_before_target()**: This function will be called just before declaring the executable target for your project.  
  It will happen after having set and checked all package information, found the engine and retrieved the package version.
- **callback_after_target()**: This function will be called later on, when your project executable target `${PACKAGE_EXE_NAME}` is already declared.  
  At this time most of the target properties are already set, like sources, include directories, compile definitions and compile options.
- **callback_end()**: This function will be called at the very end, when all the variables are set and all the external scripts have been included.

### Package options

The `cmake/package_options.cmake` holds a list of options that you can setup when invoking CMake:

- **PACKAGE_BUILD_ANDROID**: When enabled the scripts will try to build an Android version of the project.  
  When this option is enabled you have access to an additional set of variables:
  - **NDK_DIR**: The path to the Android NDK directory.  
    You need to set this variable if the script system can detect any NDK related environment variable in your system.
  - **PACKAGE_NDK_ARCHITECTURES**: A list of target CPU architectures for the Android version of your project.  
    The default is `arm64-v8a`.
  - **PACKAGE_ASSEMBLE_APK**: If the option is on then the CMake script will try to assemble an APK by invoking Gradle.
- **PACKAGE_STRIP_BINARIES**: When turned on binary files will be stripped of their symbols to reduce their size.  
  It does not work on Windows.
- **PACKAGE_DATA_DIR**: The path to the package root data directory.  
  The default is to search for a directory at the same level of your project one that has a `-data` suffix.  
  You don't usually need to change the value of this variable.
- **NCINE_EXTERNAL_DIR**: The path to the nCine external dependencies directory.  
  The default is to search for a directory at the same level of your project one that is called `nCine-external`.  
  You don't usually need to change the value of this variable.

### Additional files

- Rewrite the `LICENSE` and `README.md` files with the information related to your project.
- If your project targets macOS remember to modify the `NSHumanReadableCopyright` in the `Info.plist.in` file.
- Don't forget to read the `README.md` file in the `ncTemplate-data` repository for additional customization information.

## Notes

The scripts will add a compiler definitions when the project is compiled in `Debug` mode.
By default it is `${PACKAGE_UPPER_NAME}_DEBUG` which, for this specific project, will become `NCTEMPLATE_DEBUG`.
