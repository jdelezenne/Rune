# Rune

Rune is a statically typed programming language for systems and application development, implemented in C++23. It is cross-platform on Windows, macOS, and Linux, with a WebAssembly-hosted compiler and VM for running Rune in the browser. Rune supports three code-generation backends: LLVM (native executable), C23 (generated code), and VM (custom bytecode and virtual machine).

## Features

- **Structured source model** - top-level declarations and statements, optional semicolons, nested block comments, interpolation-ready string literals, tuples, arrays, sets, and maps.
- **Explicit access and attributes** - `public`/`internal`/`private` visibility, declaration attributes, and attributed declaration groups for concise native bindings.
- **Static type system** - fixed-width and native-width integers, floating-point aliases, `Bool`, `Char`, `String`, `Void`, plus aliases, distinct types, and type-safe conversions/checks (`as`, `as?`, `as!`, `is`).
- **Rich declarations** - constants, mutable variables, functions, structs, enums, flags, unions, interfaces, extensions, static scopes, and computed properties.
- **Generics across core constructs** - parameterized functions, structs, interfaces, and unions for reusable strongly typed APIs.
- **Control flow and matching** - `if`, `while`, `for`, `loop`, ranges (`..`, `..=` forms), and `match` over values and algebraic data forms.
- **Lambdas and closures** - first-class anonymous functions, function references, and callbacks.
- **Option, result, and error model** - first-class option/result handling with `try`, `throw`, `catch`, optional chaining/coalescing (`?.`, `??`), and explicit failure paths.
- **Async/await** - `async` functions and `await` expressions for asynchronous programming.
- **Threading** - threads, tasks, and mutexes for concurrent programs.
- **Compile-time execution and macros** - `eval` for VM-backed const initializers, plus `#` macros for compiler-provided operations.
- **Introspection** - stable type identities, compile-time `#typeInfoOf` metadata, and a runtime type registry covering structs, enums, flags, unions, and distinct types.
- **References and manual memory** - reference creation/dereference, `make`/`dispose`, allocator-aware construction, and `defer` for deterministic cleanup.
- **Module system and reusable artifacts** - imports, `.runemodule` outputs, and `.runemanifest` dependency/link metadata for multi-module projects.
- **Native C interop** - direct symbol binding through `@symbol`, with the C header and native library named in the module manifest (or `-l`/`--include-header` CLI flags for source builds).
- **SDL3 and WebGPU bindings** - bundled binding modules and example applications for native SDL3 development and browser graphics/compute through WebGPU.
- **WebAssembly support** - the Rune compiler, VM, and standard library can be packaged as a WASM application and run entirely in the browser, as demonstrated by the Playground.
- **Three backends** - LLVM backend, C23 backend, and VM backend.

## Playground

[Rune Playground](https://jdelezenne.github.io/Rune/Playground/index.html)

## Quick look

```rune
import Standard.*

struct Point {
    var x: Float = 0.0
    var y: Float = 0.0

    func distanceTo(other: Point) -> Float {
        let dx = other.x - x
        let dy = other.y - y
        return sqrt(dx * dx + dy * dy)
    }
}

union Shape {
    circle(Float)
    rect(Float, Float)
}

func area(shape: Shape) -> Float {
    match shape {
        .circle(r)    => return 3.14159 * r * r
        .rect(w, h)   => return w * h
    }
}

func main() {
    let a = Point(y: 2.0)
    let b = Point(x: 3.0, y: 4.0)
    Console.printLine("distance: ${a.distanceTo(other: b)}")
    Console.printLine("area: ${area(shape: Shape.circle(5.0))}")
}
```

### Generics

```rune
struct Box<T> {
    var value: T
}

func wrap<T>(value: T) -> Box<T> {
    return Box<T>(value)
}

func main() {
    let answer = wrap<Int>(42)
    let name = wrap<String>("Rune")
}
```

### Interfaces

```rune
interface Describable {
    func describe() -> String
}

struct Document implements Describable {
    var title: String

    func describe() -> String {
        return title
    }
}

func main() {
    let document = Document(title: "README")
    #printLn(document.describe())
}
```

### Constants

Use `const` for values that are known at compile time. Constants can reference
earlier constants and ordinary constant expressions:

```rune
const base: Int = 21
const doubled: Int = base * 2
const isEven: Bool = doubled % 2 == 0
const zero: Int = default

func main() -> Int {
    if not isEven || doubled != 42 {
        return 1
    }

    return 0
}
```

### `default` and `uninit`

Use `default` to initialize a value with the type's default value:

```rune
let n: Int = default     // 0
let ok: Bool = default   // false
```

Use `uninit` only in unsafe module mode when you intentionally want
uninitialized storage and will fully initialize it before reading:

```rune
let value: Int = uninit
value = 42
```

`uninit` is rejected unless:

- the module is unsafe (`--unsafe` for source execution)
- a contextual type is known
- the contextual type is primitive, or a struct without explicit `init` and `final`

### Compile-Time Evaluation

Functions are ordinary declarations. Use `eval` in a `const` initializer to run
ordinary Rune code during compilation.

The evaluation is done through the VM backend and the result is embedded as readonly
data and works with every backend.

```rune
func makeTable() -> Array<Int> {
    var values: Array<Int> = [1, 2, 3]
    values.append(5)
    return values
}

const TABLE = eval makeTable()
```

### Async

```rune
func fetch() async -> Int {
    return await remoteCall()
}

func main() async {
    let result = await fetch()
    #printLn("got ${result}")
}
```

### C Interop

```rune
@symbol
func sqrt(x: Float64) -> Float64

func hypotenuse(a: Float64, b: Float64) -> Float64 {
    return sqrt(a * a + b * b)
}
```

The C header (`math.h`) and native library (`m`) come from the module manifest's `includeHeaders` / `linkLibraries`, or `--include-header` / `-l` for source builds.

### Allocation

Heap allocation uses `make!` and `dispose`:

```rune
struct Node {
    var value: Int

    init(value: Int) {
        self.value = value
    }

    final() {
        // release resources owned by this node
    }
}

func main() {
    let node = make! Node(42)
    #printLn("${node.value}")
    dispose node
}
```

Bare `make!` uses `Memory.currentAllocator`. Pass any value implementing the
`Allocator` interface only when a specific allocation should use another
allocator instance:

```rune
// Stack-backed allocator instance
var allocator = StackAllocator()
let node = make!(allocator) Node(value: 42)
defer dispose(allocator) node
```

Custom allocators implement a simple interface:

```rune
struct ArenaAllocator implements Allocator {
    let storage: &Byte
    let capacity: UInt
    var offset: UInt = 0

    init(capacity: UInt) {
        self.capacity = capacity
        self.storage = try! Memory.systemAllocate(size: capacity)
    }

    final() {
        Memory.systemDeallocate(address: self.storage)
    }

    public mutating func allocate(size: UInt, alignment: UInt) throws -> &Byte {
        let start = Memory.alignUp(value: self.offset, alignment: alignment)
        let end = start + size
        if end > self.capacity { throw }
        self.offset = end
        return (Int(self.storage) + Int(start)) as &Byte
    }

    public mutating func deallocate(address: *Byte) {}
}
```

## Run a program

```bash
# Windows (PowerShell)
$env:RUNE = "Binaries/Windows/Debug/Rune/rune.exe"

# macOS
export RUNE=Binaries/Darwin/Debug/Rune/rune

# Linux
export RUNE=Binaries/Linux/Debug/Rune/rune

# Run with the VM backend
$RUNE run Examples/Standard/Hello.rune

# Run with the LLVM backend and performance stats
$RUNE run Examples/Standard/Hello.rune --backend llvm --stats

# Build a C23 translation unit
$RUNE build Examples/Standard/Hello.rune --executable --name Hello --backend c -o /tmp/Hello/

# Build a native executable with the LLVM backend
$RUNE build Examples/Standard/Hello.rune --executable --name Hello --backend llvm -o /tmp/Hello/
```

## Examples

| Directory | What it shows |
|-----------|---------------|
| `Examples/Language/` | Language feature showcases (generics, async, match, interop, ...) |
| `Examples/Standard/` | Programs with standard module (console, storage, network, ...) |
| `Examples/LibC/` | Programs with LibC module (memory, strings, io, ...) |
| `Examples/Modules/SDL3/` | SDL3 bindings for Rune |
| `Examples/Programs/SDL3/` | SDL3 demos (Breakout, Tetris, Asteroids, Mandelbrot, ...) |
| `Examples/Modules/WebGPU/` | WebGPU bindings for browser/WASM applications |
| `Examples/Programs/WebGPU/` | WebGPU graphics and compute demos (Wipeout, particles, Game of Life, ...) |
| `Examples/Programs/Network/` | Network demos (UDP and TCP echo, TCP Chat with client / server, ...) |
| `Examples/Programs/SDL3/AsteroidsMultiplayer/` | Multiplayer demo with SDL3 and TCP client / server |

## Modules

Rune programs can be compiled to `.runemodule` artifacts and reused across projects. A `.runemanifest` file describes sources, dependencies, and native link settings:

```bash
# Build a module
$RUNE build Examples/Modules/SDL3/SDL3.runemanifest

# Run a program that depends on it
$RUNE run Examples/Programs/SDL3/Breakout.rune --module-path /path/to/modules
```

## Editor support

The [`vscode-rune`](https://github.com/jdelezenne/vscode-rune) extension provides full language and debug support for VS Code.

**Language features** (via the built-in LSP - `rune lsp`):
- Syntax highlighting and bracket matching
- Inline diagnostics (errors and warnings)
- Go-to-definition
- Hover documentation
- Code completions

**Run**:
- Run the current file in the integrated terminal or as a VS Code task (`Cmd+Shift+R` / `Ctrl+Shift+R`)
- Supported backend: `vm`
- Automatic discovery of module paths from workspace build directories

**Debugging** (via the built-in DAP - `rune debug`):
- Breakpoints in `.rune` source files
- Call stack with function names and source locations (file, line, column)
- Local variables panel with names, types, and formatted values
- Expandable aggregate types (structs) with per-field inspection
- Collection and array element previews (up to 8 elements inline)
- Inline editing of scalar variable values (bool, integer, float) from the Variables panel
- Step over, step into, step out, and continue
- Stop on entry
- Program output forwarded to the Debug Console
- Launch configuration via `launch.json` with `rune` debug type; configurable backend, working directory, and module paths per configuration

## Tests

```bash
cmake --build --preset ninja-debug --target RuneFeatureTests RuneExampleSmokeTests
ctest --preset ninja-debug --output-on-failure
```

## Documentation

Documentation is organized into two primary directories:

- [Documentation/Users/](Documentation/Users/) - language reference, guides, and command line usage
- [Documentation/Developers/](Documentation/Developers/) - compiler/runtime architecture and contributor-facing docs

### User documentation

- [Introduction](Documentation/Users/INTRODUCTION.md)
- [Language Specification](Documentation/Users/LANGUAGE_SPECIFICATION.md)
- [Module Manifests](Documentation/Users/MODULE_MANIFEST.md)
- [Command Line](Documentation/Users/COMMAND_LINE.md)

### Developer documentation

- [Introduction](Documentation/Developers/INTRODUCTION.md)
- [Lexer Internals](Documentation/Developers/LEXER_INTERNALS.md)
- [Parser Internals](Documentation/Developers/PARSER_INTERNALS.md)
- [Sema Internals](Documentation/Developers/SEMA_INTERNALS.md)

## Roadmap

- Metaprogramming
- Builtin ECS

## License

Source available. See [LICENSE](LICENSE) for terms.
