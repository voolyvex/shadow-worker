#!/usr/bin/env python
from SCons.Script import Environment, SConscript, Glob, Default
import os
import sys

# Use external godot-cpp bindings
env = SConscript("C:/Godot/godot-cpp/SConstruct")

# Add source files
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# Ensure bin directory exists
if not os.path.exists("bin"):
    os.makedirs("bin")

# Build the library with platform-specific naming
if env["platform"] == "windows":
    library = env.SharedLibrary(
        "bin/libshadow_worker",  # Base name
        source=sources
    )
else:
    library = env.SharedLibrary(
        "bin/libshadow_worker{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources
    )

Default(library)