from pathlib import Path
import subprocess
import sys

PROJECT_ROOT = Path(__file__).resolve().parent
SHADER_SOURCE_DIR = PROJECT_ROOT / "resources" / "shaders"
COMPILED_DIR = SHADER_SOURCE_DIR / "compiled"
SHADERCROSS_EXE = PROJECT_ROOT / "shadercross" / "shadercross.exe"

ENTRY_POINT = "main"
TARGETS = {"DXIL": "dxil", "SPIRV": "spv"}


def make_c_header_from_binary(binary_path, header_path, symbol):
    data = binary_path.read_bytes()
    header_path.parent.mkdir(parents=True, exist_ok=True)
    with header_path.open("w", encoding="utf-8", newline="\n") as header:
        header.write("#pragma once\n\n")
        header.write(f"static const unsigned char {symbol}[] = {{\n")
        for offset in range(0, len(data), 12):
            chunk = data[offset:offset + 12]
            header.write("    " + ", ".join(f"0x{byte:02x}" for byte in chunk) + ",\n")
        header.write("};\n")
        header.write(f"static const unsigned int {symbol}_len = {len(data)};\n")


def get_var_name_for_header(binary_path, format_suffix):
    relative = binary_path.relative_to(COMPILED_DIR)
    base = relative.with_suffix("").as_posix()
    safe = "".join(ch if ch.isalnum() else "_" for ch in base)
    return f"{safe}_{format_suffix}"


def main():
    if not SHADERCROSS_EXE.is_file():
        raise FileNotFoundError(f"shadercross not found at: {SHADERCROSS_EXE}")
    if not SHADER_SOURCE_DIR.is_dir():
        raise FileNotFoundError(f"Shader source directory not found: {SHADER_SOURCE_DIR}")

    COMPILED_DIR.mkdir(parents=True, exist_ok=True)

    # Looks for *.frag (and *.frag.hlsl if you use double extensions)
    shaders = sorted(
        path for path in SHADER_SOURCE_DIR.rglob("*")
        if COMPILED_DIR not in path.parents
        and path.is_file()
        and (path.name.endswith(".frag") or path.name.endswith(".frag.hlsl"))
    )

    if not shaders:
        print(f"No .frag files found in {SHADER_SOURCE_DIR}")
        return 0

    print(f"Found {len(shaders)} fragment shader(s).\n")
    failures = []

    for source in shaders:
        relative = source.relative_to(SHADER_SOURCE_DIR)
        
        # Strip .frag or .frag.hlsl cleanly for the output name
        clean_stem = source.name.removesuffix(".hlsl").removesuffix(".frag")
        output_dir = COMPILED_DIR / relative.parent
        output_dir.mkdir(parents=True, exist_ok=True)
        
        print(f"[SHADER] {relative}")

        for target, extension in TARGETS.items():
            binary = output_dir / f"{clean_stem}.frag.{extension}"
            header = binary.with_suffix(binary.suffix + ".h")
            
            command = [
                str(SHADERCROSS_EXE), str(source),
                "-s", "HLSL", "-d", target, "-t", "fragment",
                "-e", ENTRY_POINT, "-o", str(binary),
            ]
            print(f"  {target}: {binary.relative_to(PROJECT_ROOT)}")
            result = subprocess.run(command, cwd=PROJECT_ROOT)
            
            if result.returncode != 0 or not binary.is_file():
                failures.append(f"Compilation failed: {relative} ({target})")
                continue

            symbol = get_var_name_for_header(binary, extension)
            make_c_header_from_binary(binary, header, symbol)
            print(f"  Header: {header.relative_to(PROJECT_ROOT)}")
        print()

    if failures:
        print(f"Completed with {len(failures)} error(s):")
        for failure in failures:
            print(f"  - {failure}")
        return 1

    print("All shaders compiled and headers generated.")
    return 0


if __name__ == "__main__":
    try:
        code = main()
    except Exception as error:
        print(f"[FATAL] {error}")
        code = 1

    if sys.platform == "win32":
        input("\nPress Enter to close...")
    raise SystemExit(code)
