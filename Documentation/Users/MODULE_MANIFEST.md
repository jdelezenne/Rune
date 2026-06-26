# Module manifests (`.runemanifest`)

A **module manifest** is a JSON file that describes how to build a Rune **library** or **executable module** into a `.runemodule` semantic interface and, optionally, a backend artifact. Manifests are the authoritative build description for multi-file modules, stdlib packages, and anything that declares dependencies or platform-specific sources.

Manifest files use the extension `.runemanifest` and live next to (or above) the `.rune` sources they list. Paths inside the manifest are resolved relative to the manifest directory unless they are already absolute.

## What a manifest produces

Building from a manifest runs the full module pipeline: parse every listed source, semantic analysis, dependency loading, package writing, and (when a backend is selected) backend artifact emission.

| Artifact | Description |
|---|---|
| `<output>/Name.runemodule` | Semantic module interface (always produced) |
| `<output>/vm/Name.runebc` | VM bytecode (when `--backend vm` is selected) |
| `<output>/c/Name.c`, `Name.h` | C translation unit and header (when `--backend c`) |
| `<output>/llvm/Name.o` | Native object file (when `--backend llvm`) |

The `.runemodule` semantic interface is what other modules consume at `import` time. Backend artifacts are consumed at link or execution time and are not part of the Sema dependency contract.

If `"output"` is omitted, the artifact directory defaults to:

```text
<manifest-directory>/
```

(or the directory named by `--output` on the command line).

## Minimal example

```json
{
  "version": "1.0.0",
  "name": "MyLib",
  "kind": "library",
  "configuration": "release",
  "sources": [
    "Utils.rune",
    "Types.rune"
  ]
}
```

Build it:

```bash
rune build MyLib.runemanifest
# shorthand:
rune MyLib.runemanifest
```

## Top-level fields

| Field | Required | Type | Description |
|---|---|---|---|
| `version` | yes | string | Manifest schema version. Only `"1.0.0"` is supported during development. |
| `name` | yes | string | Module name used in `import` / `use` and in the default output filename. |
| `kind` | no | string | `"library"` (default) or `"executable"`. |
| `configuration` | no | string | `"release"` (default) or `"debug"`. Debug configuration writes debug metadata into the `.runemodule`. |
| `sources` | yes | array | One or more `.rune` files, or platform-filtered source objects (see below). Must be non-empty after platform filtering. |
| `dependencies` | no | array | Other modules this build needs (see [Dependencies](#dependencies)). |
| `linkLibraries` | no | string array | Extra native libraries to link when the module is used at runtime. |
| `linkDirectories` | no | string array | Directories to search for native libraries. |
| `output` | no | string | Output directory path, relative to the manifest directory unless absolute. |
| `format` | no | string | `"binary"` (default). Applies to the `.runemodule` interface file. |

The loader rejects any `version` other than `"1.0.0"`, empty `name`, empty `sources`, and invalid enum strings. There is no backward compatibility path for development formats.

## Sources

Each entry in `sources` is either:

1. **A string** — path to a `.rune` file, relative to the manifest directory.
2. **An object** — platform-filtered source:

```json
{
  "path": "platform/posix/socket.rune",
  "platforms": ["darwin", "linux"]
}
```

Supported platform names: `windows`, `darwin`, `linux`.

When you build or validate a manifest, Rune selects sources for the **target platform**:

- String entries are always included.
- Object entries are included only when the target platform appears in `platforms`.
- If no sources match the target platform, loading fails.

This is how the `LibC` module ships different platform bindings from one manifest:

```json
{
  "path": "platform/darwin/stat.rune",
  "platforms": ["darwin"]
},
{
  "path": "platform/windows/stat.rune",
  "platforms": ["windows"]
}
```

Override the target platform when validating or building:

```bash
rune manifest validate LibC.runemanifest --platform linux
rune build LibC.runemanifest --platform darwin
```

If `--platform` is omitted, the host platform is used.

## Dependencies

Each dependency entry is a string path to a `.runemodule` interface or another `.runemanifest`.
Relative paths are resolved from the manifest directory.

| Form | Meaning |
|---|---|
| `"../modules/Rune.runemodule"` | Use a prebuilt module interface. |
| `"../Other/Other.runemanifest"` | Build against another manifest. |

Example from the Standard library manifest:

```json
{
  "version": "1.0.0",
  "name": "Standard",
  "kind": "library",
  "configuration": "release",
  "sources": [
    "Console.rune",
    "Environment.rune",
    "Math.rune",
    "Mutex.rune"
  ],
  "dependencies": [
    "../Rune/Rune.runemanifest"
  ],
  "format": "binary"
}
```

At build time, manifest dependencies are loaded from their exact paths. Imports not covered by exact dependencies are resolved from `-I` / `--module-path` directories and the compiler's default module artifact directory.

## Module Paths And Linking

Use `-I` / `--module-path` when dependency interfaces are not in the compiler's default module artifact directory.

`linkLibraries` records native dynamic-library names. `linkDirectories` records directories used to locate those dynamic libraries. `rune manifest write` writes `linkDirectories` from `-L` / `--link-path`.

## Executable modules

Set `"kind": "executable"` for a module that defines a program entry point instead of a reusable library. Executable modules still emit a `.runemodule` semantic interface through the same pipeline, plus a backend artifact when a backend is selected.

## CLI integration

### Build from a manifest

```bash
rune build path/to/Module.runemanifest
rune path/to/Module.runemanifest          # same

# Build with VM backend artifact
rune build path/to/Module.runemanifest --backend vm
```

**Manifest mode is strict:** only these options are allowed on the command line:

| Option | Purpose |
|---|---|
| `-v`, `--verbose` | Verbose build output |
| `--stats` | Print build performance statistics |
| `--backend <name>` | Backend artifact to produce (`vm`, `c`, `llvm`) |
| `--debug` | Generate debug information |
| `--optimize` | Enable optimizations |
| `--platform <name>` | Target platform (`windows`, `darwin`, `linux`) |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable) |
| `-o`, `--output <dir>` | Override output directory |

Any other flag is rejected. You cannot pass `.rune` source files alongside a manifest — sources come entirely from the manifest.

### Validate a manifest

Checks JSON schema, resolves sources for the target platform, and prints a summary:

```bash
rune manifest validate Standard.runemanifest
rune manifest validate LibC.runemanifest --platform windows
```

Example output:

```text
OK   Standard
     platform: darwin
     sources: 4
     dependencies: 1
     output: /path/to/Standard/
```

### Write a new manifest

Scaffold a manifest from source files on the command line:

```bash
rune manifest write MyLib.runemanifest \
  --name MyLib \
  --library \
  --format binary \
  Sources/A.rune Sources/B.rune
```

`rune manifest write` options:

| Option | Purpose |
|---|---|
| `--name <Name>` | **Required.** Module name. |
| `--library` / `--executable` | Module kind (default: library). |
| `--format binary` | Artifact format written into the manifest. |
| `-o`, `--output <path>` | Artifact output directory stored in the manifest. |
| `-L`, `--link-path <dir>` | Native dynamic-library directory (repeatable). |
| `--debug` | Set `"configuration": "debug"` instead of the default `"release"`. |

Source files must be existing `.rune` paths. The writer stores paths relative to the manifest directory when possible.

## Stdlib manifests in this repository

| Manifest | Module | Notes |
|---|---|---|
| `Code/Modules/Rune/Rune.runemanifest` | `Rune` | Core types and collections; no dependencies. |
| `Code/Modules/Standard/Standard.runemanifest` | `Standard` | Console, math, environment; depends on `Rune`. |
| `Code/Modules/LibC/LibC.runemanifest` | `LibC` | C library bindings; platform-specific sources; depends on `Rune`. |

After building the compiler, CMake target `BuildStdlibModules` rebuilds all three into `Binaries/<Platform>/<Config>/Rune/modules/`.

## Practical notes

- **One manifest per module.** Do not list the same `.rune` file twice in `sources`.
- **Keep paths stable.** Prefer manifest-relative paths so the project can be moved or checked out elsewhere.
- **Use exact dependency paths.** Each dependency entry names a `.runemodule` or `.runemanifest` path.
- **Validate before CI.** `rune manifest validate` catches missing files and platform mismatches without running a full build.
- **`.runemodule` is a semantic interface.** It contains exported declarations and type metadata. It does not contain bytecode. VM bytecode lives in a separate `.runebc` artifact under the `vm/` output subdirectory.

See also [Command line](COMMAND_LINE.md) for all `rune` subcommands and flags.
