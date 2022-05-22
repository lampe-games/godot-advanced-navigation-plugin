#!python
import os, subprocess

env = DefaultEnvironment()

# fmt: off
opts = Variables([], ARGUMENTS)
opts.Add(EnumVariable('target', "Compilation target", 'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(PathVariable('target_path', 'The path where the lib is installed.', 'bin/'))
opts.Add(PathVariable('target_name', 'Advanced Navigation Plugin.', 'libadvancednavigation', PathVariable.PathAccept))
opts.Update(env)
# fmt: on

godot_headers_path = "thirdparty/godot-cpp/godot-headers/"
cpp_bindings_path = "thirdparty/godot-cpp/"
sources_path = "src/"
cpp_library = "libgodot-cpp"
bits = 64

if env["platform"] == "":
    print("No valid target platform selected.")
    quit()

if env["platform"] == "osx":
    env["target_path"] += "osx/"
    cpp_library += ".osx"
    env.Append(CCFLAGS=["-arch", "x86_64"])
    env.Append(CXXFLAGS=["-std=c++17"])
    env.Append(LINKFLAGS=["-arch", "x86_64"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-g", "-O2"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] in ("x11", "linux"):
    env["target_path"] += "x11/"
    cpp_library += ".linux"
    env.Append(CCFLAGS=["-fPIC"])
    env.Append(CXXFLAGS=["-std=c++17", "-Wall"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-g3", "-Og"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] == "windows":
    env["target_path"] += "win64/"
    cpp_library += ".windows"
    env["CXX"] = "x86_64-w64-mingw32-g++"
    env["AR"] = "x86_64-w64-mingw32-ar"
    env["RANLIB"] = "x86_64-w64-mingw32-ranlib"
    env["LINK"] = "x86_64-w64-mingw32-g++"
    env.Append(CCFLAGS=["-g", "-O3", "-std=c++17", "-Wwrite-strings"])
    env.Append(
        LINKFLAGS=[
            "--static",
            "-Wl,--no-undefined",
            "-static-libgcc",
            "-static-libstdc++",
        ]
    )

if env["target"] in ("debug", "d"):
    cpp_library += ".debug"
else:
    cpp_library += ".release"

cpp_library += "." + str(bits)

env.Append(
    CPPPATH=[
        ".",
        godot_headers_path,
        cpp_bindings_path + "include/",
        cpp_bindings_path + "include/core/",
        cpp_bindings_path + "include/gen/",
        "thirdparty/recastnavigation/Recast/Include/",
        "thirdparty/recastnavigation/Detour/Include/",
        "thirdparty/recastnavigation/DetourCrowd/Include/",
    ]
)
env.Append(LIBPATH=[cpp_bindings_path + "bin/"])
env.Append(LIBS=[cpp_library])
env.Append(CPPPATH=[sources_path])

sources = (
    Glob("{}/*.cpp".format(sources_path))
    + Glob("{}/*.cpp".format("thirdparty/recastnavigation/Recast/Source/"))
    + Glob("{}/*.cpp".format("thirdparty/recastnavigation/Detour/Source/"))
    + Glob("{}/*.cpp".format("thirdparty/recastnavigation/DetourCrowd/Source/"))
)
library = env.SharedLibrary(
    target=env["target_path"] + env["target_name"], source=sources
)

Default(library)

Help(opts.GenerateHelpText(env))
