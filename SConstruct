#!/usr/bin/env python
from SCons.Script import Environment, SConscript, Glob, Default
import os
import sys

# Use external godot-cpp bindings
env = SConscript("godot-cpp/SConstruct")

# Add source files and include paths
env.Append(CPPPATH=[
    "src/",
    "godot-cpp/gdextension/",
    "godot-cpp/include/",
    "godot-cpp/gen/include/"
])
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