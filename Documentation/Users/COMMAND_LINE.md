# Rune command line

The `rune` executable is the Rune compiler driver. It exposes user-facing commands for building and running programs (`build`, `run`), inspecting compiled artifacts (`inspect`), managing module manifests (`manifest`), starting the language server (`lsp`), and running editor debug sessions (`debug`), plus developer commands under `dev` for inspecting compiler stages.

Run `rune` with no arguments to print a short usage summary.

## Command overview

| Command | Purpose |
|---|---|
| `rune run ...` | Execute from `.rune` sources, a `.runemanifest`, or a compiled executable `.runemodule`. |
| `rune build ...` | Build a `.runemodule` and optional backend artifact from `.rune` sources or a `.runemanifest`. |
| `rune inspect module ...` | Inspect a binary `.runemodule` semantic interface as text or JSON. |
| `rune inspect bytecode ...` | Inspect a `.runebc` VM bytecode artifact as text or JSON. |
| `rune manifest validate ...` | Check a `.runemanifest` file. |
| `rune manifest write ...` | Create a new `.runemanifest` from sources. |
| `rune dev tokenize <files...>` | Tokenize `.rune` files and emit the token stream (`text` or `json`). |
| `rune dev parse <files...>` | Parse `.rune` files and emit the AST (`text` or `json`). |
| `rune dev ril <files...>` | Lower `.rune` files to RIL and print to stdout (`text` or `json`). |
| `rune dev bytecode <file.runebc>` | Decode a `.runebc` artifact and emit bytecode contents (`text` or `json`). |
| `rune lsp --stdio` | Start the Rune language server over stdio. |
| `rune debug ...` | Execute a Rune program under the VS Code Debug Adapter Protocol. |
| `rune <file.rune ...>` | Shorthand for `rune run <file.rune ...>`. |
| `rune <file.runemanifest>` | Shorthand for `rune run <file.runemanifest>`. |

The default command is `run` whenever the first argument is not a named command. In practice this is the shorthand for `.rune`, `.runemanifest`, and executable `.runemodule` inputs.
Mixing `.rune`, `.runemanifest`, and `.runemodule` inputs in one invocation is an error.
Unknown positional file extensions are rejected.

Global flags:

| Flag | Purpose |
|---|---|
| `-h`, `--help` | Print command help. Use after a command or subcommand for scoped help. |
| `-v`, `--verbose <categories>` | Comma-separated verbose categories: `all`, `none`, `parse`, `package`, `emit`, `run`, `sema`, `sema.pipeline`, `sema.decl`, `sema.import`, `sema.lookup`, `sema.type`. |

For `run`, arguments after `--` are passed to the program:

```bash
rune run App.rune -- --input data.txt --count 3
```

---

## `build`

Build a **module semantic interface** (`.runemodule`) and an optional **backend artifact** from `.rune` source files or a `.runemanifest`. This runs parsing, semantic analysis, dependency binding, package writing, and backend artifact emission.

```bash
# From a manifest:
rune build Code/Modules/Standard/Standard.runemanifest
rune build Standard.runemanifest --backend vm --platform linux

# From source files:
rune build Examples/Modules/MyLib/*.rune --name MyLib
rune build App.rune --executable --name Main --backend vm -o Main/
rune build Foo.rune Bar.rune --name Foo --format binary
```

When given a `.runemanifest`, the manifest is authoritative for module name, sources, dependencies, and platform-filtered sources. See [Module manifests](MODULE_MANIFEST.md) for the JSON schema.

**Options:**

| Option | Purpose |
|---|---|
| `-v`, `--verbose <categories>` | Verbose logging categories. |
| `--stats` | Print build performance statistics. |
| `--stats-detail` | Print build performance statistics with nested phase categories. |
| `--backend <name>` | Backend to emit: `vm`, `c`, or `llvm`. When omitted, only the `.runemodule` interface is produced. |
| `--debug` | Generate debug information. |
| `--optimize` | Enable optimizations. |
| `--platform <name>` | Target platform: `darwin`, `linux`, `windows`, or `wasm` (default: host) |
| `--name <Name>` | Module name (source builds only; inferred from single-file source builds when omitted) |
| `--library` / `--executable` | Module kind (source builds only; default: executable) |
| `--format binary` | Interface format (source builds only; default: binary) |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable) |
| `-d`, `--dependency <path>` | Explicit `.runemodule` or `.runemanifest` dependency path (source builds only; repeatable) |
| `-l`, `--link-library <name>` | Native library to link (source builds only; repeatable) |
| `--include-header <name>` | C header the C backend must `#include` for `@symbol` FFI bindings (source builds only; repeatable) |
| `-o`, `--output <dir>` | Output directory |

`--platform` controls platform-filtered sources, `#platform(...)` conditions, and
the target ABI used by semantic analysis. It does not select an LLVM target
triple: the native LLVM backend still emits code for the host. Use the
Emscripten build pipeline to produce a WebAssembly executable; using
`--backend vm --platform wasm` produces VM artifacts intended for a WASM-hosted
runtime.

**Output artifact layout:**

Each module produces a semantic interface alongside module-scoped backend artifacts:

```text
<output>/
  Standard.runemodule          semantic interface (always produced)
  vm/
    Standard.runebc            VM bytecode (when --backend vm)
  c/
    Standard.c                 C translation unit (when --backend c)
    Standard.h                 C header (when --backend c)
  llvm/
    Standard.o                 object file (when --backend llvm)
```

The `.runemodule` semantic interface is the only artifact consumed by dependent module builds. Backend artifacts are consumed at link or execution time.

**Common errors:**

| Message | Cause |
|---|---|
| `Error: No source files specified` | Build command did not receive source files or a manifest. |
| `Error: --format must be binary` | Source build used an unsupported interface format. |
| `Error: --executable, --library, --unsafe, --name, --format, --dependency, --link-library, and --include-header are only valid for source builds` | Source-only flags were used with a manifest build. |
| `Source file specified more than once` | Duplicate path in the file list. |
| Semantic / parse diagnostics | Invalid Rune code or unresolved imports. |

Source builds do not read `dependencies` or platform-filtered sources from disk — use a manifest for multi-file modules with dependencies.

**Success output:**

```text
Module 'Standard' written to /path/to/Standard.runemodule
```

---

## `run`

Execute a Rune program from sources, a manifest, or a compiled executable `.runemodule`.

```bash
rune run Examples/Output/Hello.rune
rune run Code/Modules/Rune/Rune.runemanifest
rune run Binaries/<Platform>/<Config>/Rune/modules/App.runemodule
rune run Examples/Output/Args.rune -- first second
```

Implicit `run` shorthand also works when the first positional argument is a `.rune`, `.runemanifest`, or `.runemodule` path:

```bash
rune Examples/Output/Hello.rune
rune Code/Modules/Rune/Rune.runemanifest
rune Binaries/<Platform>/<Config>/Rune/modules/App.runemodule
```

**Arguments:**

- one or more `.rune` files, or
- exactly one `.runemanifest`, or
- exactly one executable `.runemodule` when using the VM backend.

**Options:**

| Option | Purpose |
|---|---|
| `--backend <name>` | Backend to use for execution: `vm` (default), `c`, or `llvm`. |
| `--stats` | Print build performance statistics before program execution. |
| `--stats-detail` | Print build performance statistics with nested phase categories before program execution. |
| `--profile` | Print backend execution profile information after the program runs. The VM backend includes bytecode instruction, opcode, function, and foreign-call counters. |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable). |
| `-d`, `--dependency <path>` | Explicit `.runemodule` or `.runemanifest` dependency path for source runs (repeatable). |
| `-l`, `--link-library <name>` | Native library to link (source runs only; repeatable). |
| `--include-header <name>` | C header the C backend must `#include` for `@symbol` FFI bindings (source runs only; repeatable). |
| `-v`, `--verbose <categories>` | Verbose logging categories. `run` traces VM instruction execution and native calls to stderr (`[vm] ...`). |

Example:

```bash
rune run Examples/Output/Hello.rune --verbose run
rune run --backend vm --profile Examples/Benchmarks/Particles.rune
```

**Common errors:**

| Message | Cause |
|---|---|
| `Cannot mix a .runemanifest file with .rune source files` | Manifest and source files were passed together. |
| `Cannot mix .rune source files with a .runemodule executable` | Source files and an executable module were passed together. |
| `rune run native backends require source files or a manifest, not a .runemodule executable` | `.runemodule` execution was requested with `--backend c` or `--backend llvm`. |
| `Unsupported input path ...` | Positional input does not end with `.rune`, `.runemanifest`, or `.runemodule`. |

---

## `inspect module`

Inspect a binary `.runemodule` semantic interface. This command displays the exported declarations, type table, dependency references, and foreign bindings contained in the interface section.

```bash
rune inspect module Binaries/<Platform>/<Config>/Rune/modules/Rune.runemodule
rune inspect module MyLib.runemodule --format json
rune inspect module MyLib.runemodule --format json -o MyLib.module.json
```

**Arguments:** exactly one `.runemodule` path.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |

Text output is intended for quick human inspection. JSON output includes module metadata, dependencies, identifier table, exported declarations, exported types, and foreign bindings.

A `.runemodule` file is a **semantic interface only**. It does not contain VM bytecode, object files, or any other backend payload. Use `rune inspect bytecode` for VM bytecode artifacts.

---

## `inspect bytecode`

Inspect a `.runebc` VM bytecode artifact.

```bash
rune inspect bytecode Binaries/<Platform>/<Config>/Rune/modules/vm/Rune.runebc
rune inspect bytecode Standard.runebc --format json
rune inspect bytecode Standard.runebc --format json -o Standard.bc.json
```

**Arguments:** exactly one `.runebc` path.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |

`.runebc` files contain VM bytecode functions, constants, type tables, and VM import/export tables. They are produced by `rune build --backend vm` and consumed by the VM for execution.

---

## `dev tokenize`

Lex one or more source files and print the token stream. Useful for debugging the lexer.

```bash
rune dev tokenize Examples/Output/Hello.rune
rune dev tokenize src/Foo.rune src/Bar.rune
rune dev tokenize Examples/Output/Hello.rune --format json -o tokens.json
```

**Arguments:** one or more `.rune` paths.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |

**Errors:** exits non-zero if no files are given or lexing fails.

---

## `dev parse`

Parse one or more source files without running semantic analysis. Useful for syntax-only checks.

```bash
rune dev parse Examples/Output/Hello.rune
rune dev parse Examples/Output/Hello.rune --format json -o ast.json
```

**Arguments:** one or more `.rune` paths.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |

**Errors:** exits non-zero if no files are given or parsing fails.

---

## `dev ril`

Lower one or more source files through semantic analysis and print the resulting RIL (Rune Intermediate Language) to stdout. RIL is the canonical lowered representation produced after Sema and consumed by all production backends.

```bash
rune dev ril Examples/Output/Hello.rune
rune dev ril Foo.rune --name Foo --library --format json
rune dev ril Code/Modules/Standard/Standard.runemanifest --format text
```

**Arguments:** one or more `.rune` paths, or exactly one `.runemanifest`.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |
| `--name <Name>` | Module name for source-file input. |
| `--library` | Treat source-file input as a library module. Source-file input defaults to executable. |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable). |
| `-d`, `--dependency <path>` | Explicit `.runemodule` or `.runemanifest` dependency path (repeatable). |

RIL output is a developer and diagnostic tool. It is not a build artifact and is not consumed by dependent module builds. Printing RIL does not produce a `.runemodule` or any backend artifact.

**Errors:** exits non-zero if no files are given, parsing fails, or semantic analysis fails.

---

## `dev bytecode`

Decode a `.runebc` artifact and emit its bytecode contents. This is the developer equivalent of `inspect bytecode`.

```bash
rune dev bytecode Binaries/<Platform>/<Config>/Rune/modules/vm/Rune.runebc
rune dev bytecode Standard.runebc --format json -o Standard.bc.json
```

**Arguments:** path to a `.runebc` file.

**Options:**

| Option | Purpose |
|---|---|
| `--format text\|json` | Output format (default: `text`). |
| `-o`, `--output <path>` | Write output to a file instead of stdout. |

---

## `manifest validate`

Load and validate a `.runemanifest` without building.

```bash
rune manifest validate Code/Modules/LibC/LibC.runemanifest
rune manifest validate LibC.runemanifest --platform windows
```

**Arguments:** path to a `.runemanifest` file.

**Options:** `--platform <name>` to validate source selection for a specific target.

On success:

```text
OK   LibC
     platform: darwin
     sources: 24
     dependencies: 1
     output: /path/to/LibC/LibC.runemodule
```

---

## `manifest write`

Create a new `.runemanifest` from `.rune` sources.

```bash
rune manifest write MyLib.runemanifest \
  --name MyLib \
  --library \
  Sources/Util.rune Sources/Api.rune
```

**Usage:**

```text
rune manifest write <output.runemanifest> --name <Name> [options] <file.rune>...
```

**Options:**

| Option | Purpose |
|---|---|
| `--name <Name>` | Module name (**required**). |
| `--library` / `--executable` | Module kind (default: library). |
| `--format binary` | Stored as `"format"` in the manifest. |
| `-o`, `--output <path>` | Stored as `"output"` in the manifest. |
| `--link-path <dir>` / `-L <dir>` | Native dynamic-library directory (repeatable). |
| `--debug` | Set `"configuration": "debug"` instead of the default `"release"`. |

The output path must end with `.runemanifest`. At least one `.rune` source is required.

---

## `lsp`

Start the Rune language server over stdio.

```bash
rune lsp --stdio
rune lsp --stdio -I Binaries/<Platform>/<Config>/Rune/modules
```

**Options:**

| Option | Purpose |
|---|---|
| `--stdio` | Use stdio transport. |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable). The compiler artifact module directory is searched first. |

---

## `debug`

Execute a Rune program under the VS Code Debug Adapter Protocol. This command is intended for editor/debug-adapter integration.

```bash
rune debug Examples/Output/Hello.rune
rune debug App.runemanifest --backend vm -I Binaries/<Platform>/<Config>/Rune/modules
```

**Arguments:**

- one or more `.rune` files, or
- exactly one `.runemanifest`, or
- exactly one executable `.runemodule`.

**Options:**

| Option | Purpose |
|---|---|
| `--backend <name>` | Backend to use for debugging: `vm` (default), `c`, or `llvm`. |
| `-I`, `--module-path <dir>` | Add a directory containing `.runemodule` dependency interfaces (repeatable). |
| `-d`, `--dependency <path>` | Explicit `.runemodule` dependency interface; source builds may also use `.runemanifest` dependencies (repeatable). |

---

## Artifact model

Rune distinguishes three kinds of build outputs:

| Artifact | Extension | Produced by | Consumed by |
|---|---|---|---|
| Semantic interface | `.runemodule` | `rune build` | Sema during dependent module builds |
| VM bytecode | `.runebc` | `rune build --backend vm` | VM execution |
| C translation unit | `.c` / `.h` | `rune build --backend c` | C compiler |
| Native object file | `.o` | `rune build --backend llvm` | Native linker |

`.runemodule` is always a **semantic interface only**. It contains exported declarations, type tables, conformances, and foreign binding metadata. It does not contain executable bytecode or any other backend payload.

`.runebc` is the VM bytecode artifact. It is separate from the `.runemodule` and lives under a `vm/` subdirectory of the module output directory.

RIL (Rune Intermediate Language) is the internal representation that backends encode. It lives only in memory during normal builds. Use `rune dev ril` to inspect it without producing build artifacts.

---

## Path Conventions

Rune distinguishes two path concepts on the command line:

| Flag | Used for |
|---|---|
| `-I`, `--module-path <dir>` | Finding `.runemodule` dependency semantic interfaces. |
| `-L`, `--link-path <dir>` | Finding native dynamic libraries. |

Module paths tell the compiler where to look when a source file `import`s another module. Link paths tell the runtime or native backend where dynamic libraries live.

---

## Exit status

| Code | Meaning |
|---|---|
| `0` | Success (including `rune` with no args, which prints usage). |
| `1` | Error: bad arguments, parse/sema failure, or build failure. |

---

## Examples

**Build the Standard library module from its manifest:**

```bash
rune build Code/Modules/Standard/Standard.runemanifest -v
```

**Build the Standard library for VM execution:**

```bash
rune build Code/Modules/Standard/Standard.runemanifest --backend vm -v
```

**Quick syntax check:**

```bash
rune dev parse MyFile.rune
```

**Inspect the RIL for a source file:**

```bash
rune dev ril Examples/Output/Hello.rune
```

**Decode VM bytecode through the developer command:**

```bash
rune dev bytecode Binaries/<Platform>/<Config>/Rune/modules/vm/Rune.runebc
```

**Inspect a compiled module interface:**

```bash
rune inspect module Binaries/<Platform>/<Config>/Rune/modules/Rune.runemodule
```

**Inspect VM bytecode:**

```bash
rune inspect bytecode Binaries/<Platform>/<Config>/Rune/modules/vm/Rune.runebc
```

**Scaffold a new library manifest:**

```bash
rune manifest write libs/Graphics.runemanifest \
  --name Graphics \
  --link-path ../native-libs \
  src/Graphics.rune src/Sprite.rune
```

**Validate platform-specific sources before CI:**

```bash
rune manifest validate Code/Modules/LibC/LibC.runemanifest --platform linux
```

**Start the language server:**

```bash
rune lsp --stdio -I Binaries/<Platform>/<Config>/Rune/modules
```

**Run through the debug adapter command:**

```bash
rune debug Examples/Output/Hello.rune --backend vm
```

---

## Related documentation

- [Module manifests](MODULE_MANIFEST.md) — full `.runemanifest` JSON reference.
- [Introduction](INTRODUCTION.md) — Rune language basics, `import`, and `use`.
- Repository [README](../../README) — building the toolchain, examples, and interpreter-oriented workflows.
