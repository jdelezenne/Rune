# Memory in Rune

> For the proposed evolution of pools, specialized allocators, and engine-oriented memory routing, see [ALLOCATION.md](../Agents/ALLOCATION.md).

Rune follows a **value-first** model: ordinary values live on the stack by default. Heap memory is explicit. Low-level access uses raw references (`*T`, `&T`). Higher-level types such as `String` and `Array<T>` manage their own storage.

## Memory regions

| Region | What lives here | Lifetime | How it is allocated |
|---|---|---|---|
| **Stack** | Local variables, function parameters, struct and enum values | Lexical scope (function/block) | Automatic |
| **Heap (explicit)** | Values created with `make` | Until `dispose` | `make` / `dispose` |
| **Heap (managed)** | `String` data, `Array` elements, other collection backing storage | Managed by the runtime / container | Container operations |

```rune
func example() {
    let x: Int = 10              // stack
    let name: String = "hello"   // String handle on stack; text on heap

    let allocator = SystemAllocator()
    let particle = make!(allocator) Particle(id: 1)  // explicit heap
    dispose(allocator) particle
}
```

Stack space is sized per function frame — each call gets the locals and
parameters it needs, without manual management. Stack struct values call their
selected `init` when constructed and call `final` when their lexical scope exits
if the type declares a finalizer. This includes normal fallthrough and early
exits such as `return`, `throw`, `break`, and `continue`.

## Value semantics

Most types are **values**: assigning or passing them copies the bits (structs copy field-by-field). There is no hidden sharing for `Int`, `Bool`, structs, and other ordinary value types.

Reference types behave differently:

- `String`, `Array<T>`, and similar built-ins are **handles** (pointer-sized) to heap data managed for you.
- Raw references (`*T`, `&T`) point at storage you are responsible for understanding — stack locals, `make` allocations, or foreign memory from FFI.

### `let` vs `var`

`let` and `var` control whether the **binding** can be reassigned. They do not, by themselves, control whether a referred-to value is mutable:

```rune
var counter: Int = 0
let ptr: &Int = &counter   // binding `ptr` is immutable…
ptr^ = 42                  // …but the referent is mutable through `&Int`
```

Use `var` when you need to reassign the variable; use `&T` when you need to mutate storage through a reference.

## Raw references

Rune has two raw reference types. They are low-level capabilities, not ownership types:

| Type | Access | Typical use |
|---|---|---|
| `*T` | Read-only | Passing data without allowing mutation |
| `&T` | Read and write | In-out parameters, mutable views, `make` results |

```rune
func read(value: *Int) -> Int {
    return value^
}

func increment(value: &Int) -> Void {
    value^ = value^ + 1
}

func main() {
    var n: Int = 40
    increment(&n)
    let v = read(&n)   // `&T` coerces to `*T` where needed
}
```

### Taking and using references

| Syntax | Meaning |
|---|---|
| `&expr` | Take a **mutable** reference to a mutable lvalue |
| `ref^` | Read (or write, for `&T`) through a reference |
| `ref.field` | Auto-dereference for member access on reference receivers |
| `ref^.field` | Explicit dereference (equivalent for reads/writes on `&T`) |

### Nullable references

Raw references are **non-null by default**. Use `Option` (or postfix `?`) for nullable references:

```rune
let absent: *Int? = nil
let present: *Int? = &value

func read(ptr: *Int?) -> Int {
    return ptr!^          // unwrap, then dereference
}
```

`nil` is only valid for `Option` types — you cannot assign `nil` to a bare `*Int` or `&Int`.

### Reference rules enforced today

The compiler currently enforces several reference rules:

- `&T` implicitly converts to `*T`; the reverse is not allowed.
- Writes through `*T` are rejected (immutable reference).
- `dispose` requires a reference operand and a stable allocator expression.

A fuller **local borrow checker** (tracking active borrows, use-after-`dispose`, and similar) is planned but not yet complete.

## Heap allocation: `make` and `dispose`

Heap allocation is always explicit. Rune provides two built-in operations.
When no allocator is written, `make` uses `Memory.currentAllocator`.

The normal path uses the current allocator context:

```rune
let particle = make! Particle(id: 42)
dispose particle
```

Pass an allocator explicitly only when this allocation should use a different
allocator instance, such as a local arena, stack allocator, or subsystem-owned
global allocator:

```rune
var arena = ArenaAllocator(capacity: 4096)
let particle = make!(arena) Particle(id: 42)
dispose(arena) particle
```

### Semantics

- **`make`**, **`make!`**, and **`make?`** allocate storage for `T`, construct the value by calling the selected non-throwing initializer, and return `&T` or `Option<&T>`.
- Plain **`make`** (without `!` or `?`) is allowed only in `throws` functions or inside `try`. Use **`make!`** in non-throwing functions when allocation failure should trap, or **`make?`** when failure should return `nil`.
- Inside a **`try`** block, plain **`make`** turns allocation failure into a catchable error.
- Struct initializers used by `make` must be non-throwing. A throwing initializer must be used through stack construction in a `try` context.
- **`dispose`** calls `final` for struct values that declare one, then returns storage to the allocator.
- The **allocator used in `dispose` must match** the one used in `make`. Mismatch is undefined behavior.
- `dispose` accepts reference and optional reference operands.

```rune
struct Particle {
    var id: Int

    init(id: Int) {
        self.id = id
    }

    final() {
        // release resources owned by this particle
    }
}

func main() -> Int {
    let allocator = SystemAllocator()

    let particle = make!(allocator) Particle(id: 42) // calls init
    if particle.id != 42 {
        dispose(allocator) particle                  // calls final
        return 1
    }

    dispose(allocator) particle                      // calls final
    return 0
}
```

`make` can target structs, primitives, `String`, `Array<T>`, and other types
that support heap construction. Named arguments for struct heap construction
match initializer parameter names. When no allocator is written, the **current
allocator context** is used (see below).

## Allocators

Heap routing goes through the `Allocator` interface:

```rune
interface Allocator {
    func allocate(size: UInt, alignment: UInt) throws -> &RawPointer
    func deallocate(address: *RawPointer)
}
```

### `SystemAllocator`

The built-in `SystemAllocator` wraps the platform heap (`malloc` / `free` on most targets):

```rune
let allocator = SystemAllocator()
let box = make!(allocator) Box(value: 10)
dispose(allocator) box
```

### Allocator context

The `Memory` module exposes thread-local allocator state:

| Name | Role |
|---|---|
| `Memory.defaultAllocator` | Process-wide default (`SystemAllocator`) |
| `Memory.currentAllocator` | Allocator used by bare `make` / `dispose` |
| `Memory.allocator` | Optional override set for the current scope |

```rune
let previous = Memory.currentAllocator
Memory.currentAllocator = arena

let x = make! Foo()  // uses arena through Memory.currentAllocator
dispose x
Memory.currentAllocator = previous
```

Use `make!(allocator)` / `dispose(allocator)` when a single allocation should
use a specific allocator without changing the ambient allocator context.
Custom types can implement `Allocator` to plug in arena, pool, or engine-specific strategies. Built-in arena and frame allocators are planned.

## `RawPointer` and FFI

`RawPointer` is an opaque handle to untyped heap memory. It bridges Rune heap operations with foreign APIs and manual layout work:

```rune
// Reinterpret raw memory as typed references
let raw: &RawPointer = …
let imm: *Int = Memory.bind<Int>(raw: raw)
let mut: &Int = Memory.bindMutable<Int>(raw: raw)
```

For foreign interop, allocate and free through the appropriate imported module
or allocator abstraction, and keep ownership paired with the API that produced
the memory.

## Managed collections and strings

You do not call `make` / `dispose` for everyday string and array usage — the built-ins handle backing storage:

```rune
let text: String = "hello"       // heap-backed UTF-8; handle on stack
var items: Array<Int> = []      // grows as elements are appended
items.append(42)
```

`String` is a pointer-sized handle; character data lives on the heap as UTF-8. `Array<T>` stores elements in a contiguous heap buffer that may be reallocated when the array grows. Treat indices and slices as potentially invalidated after operations that resize storage.

## Slices (non-owning views)

`Slice<T>` and `StringSlice` are **non-owning views** over contiguous storage (typically `Array` or `String` data):

- Cheap to copy (they refer to existing storage, they do not own it).
- Do not allocate by themselves.
- Must not outlive the storage they view.

Slice types are defined in the Rune module. Borrow-checking rules for mutable vs shared slices are part of the planned safety model.

## Struct layout

Struct fields are laid out in declaration order with a stable alignment and padding model. Field offsets and total struct size are computed at compile time — nested structs, fixed-width primitives, and `Bool`/`Char` fields participate in the same layout rules used for FFI and bytecode.

Struct declarations may opt into explicit layout policy with `@layout`.
`@layout(kind: default)` keeps Rune layout, `@layout(kind: c)` follows the
target C ABI for C-compatible fields, and `@layout(kind: packed)` removes
implicit field padding. `@layout(alignment: N)` sets a minimum type alignment.
Explicit layout is valid only on closed structs that do not use `extends`; C
binding structs used in `@symbol` signatures must use `@layout(kind: c)`.
`sizeOf<T>()`, `alignmentOf<T>()`, and `strideOf<T>()` observe the selected
layout policy.

`distinct` types wrap a base type with **zero runtime cost**: a `distinct UserId = Int` occupies the same bytes as `Int`.

## Safety summary

| Mechanism | Status |
|---|---|
| Immutable vs mutable reference types (`*T` / `&T`) | Implemented |
| `make` / `dispose` with allocator matching | Implemented |
| Nullable references via `Option` | Implemented |
| Local borrow checker (use-after-free, conflicting borrows) | Planned |
| `@allocator` scope overrides, arena/pool allocators | Planned |
| `unsafe` blocks for raw pointer arithmetic | Planned |

When writing low-level code today, keep allocation and disposal balanced, do not use references after the storage they point to is freed, and prefer managed types (`String`, `Array`) unless you need explicit control.
