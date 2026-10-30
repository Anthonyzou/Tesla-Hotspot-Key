#!/usr/bin/env python3
"""Generate .vscode/c_cpp_properties.json from arduino-cli's compile_commands.json.

cpptools cannot read the sketch's entry directly: it is keyed on the generated
.ino.cpp (not the .ino), and the ESP32 core passes most include paths through
@response files using -iprefix/-iwithprefixbefore, which cpptools ignores.
"""
import json
import shlex
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DB = ROOT / "build" / "compile_commands.json"
OUT = ROOT / ".vscode" / "c_cpp_properties.json"

CPP_STD = {"gnu++2b": "gnu++23", "c++2b": "c++23", "gnu++2a": "gnu++20", "c++2a": "c++20"}


def expand(args):
    for a in args:
        if a.startswith("@"):
            yield from expand(shlex.split(Path(a[1:]).read_text()))
        else:
            yield a


def main():
    entries = json.loads(DB.read_text())
    entry = next((e for e in entries if e["file"].endswith(".ino.cpp")), None)
    if entry is None:
        sys.exit(f"no .ino.cpp entry in {DB}")
    raw = entry.get("arguments") or shlex.split(entry["command"])
    args = list(expand(raw))

    includes, defines, cpp_std, prefix = [], [], None, ""
    it = iter(args[1:])
    for a in it:
        if a == "-iprefix":
            prefix = next(it)
        elif a in ("-iwithprefixbefore", "-iwithprefix"):
            includes.append(prefix + next(it))
        elif a in ("-I", "-isystem"):
            includes.append(next(it))
        elif a.startswith("-I"):
            includes.append(a[2:])
        elif a == "-D":
            defines.append(next(it))
        elif a.startswith("-D"):
            defines.append(a[2:])
        elif a.startswith("-std="):
            std = a[5:]
            cpp_std = CPP_STD.get(std, std)

    core = next((p for p in includes if p.rstrip("/").endswith("cores/esp32")), None)
    config = {
        "name": "arduino-cli",
        "compilerPath": args[0],
        "includePath": list(dict.fromkeys(includes)),
        "defines": list(dict.fromkeys(defines)),
        # .ino files get Arduino.h injected by the Arduino preprocessor.
        "forcedInclude": [f"{core}/Arduino.h"] if core else [],
    }
    if cpp_std:
        config["cppStandard"] = cpp_std

    OUT.parent.mkdir(exist_ok=True)
    OUT.write_text(json.dumps({"version": 4, "configurations": [config]}, indent=2) + "\n")
    print(f"wrote {OUT.relative_to(ROOT)} ({len(config['includePath'])} include paths)")


if __name__ == "__main__":
    main()
