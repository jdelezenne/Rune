# Rune Language

## Introduction

Rune is a statically typed programming language aimed at systems and application development. Programs are composed of functions, structs, and other top-level declarations. The compiler performs type checking before execution or code generation.

Rune syntax includes optional semicolons, named arguments, range-based loops, pattern matching, and explicit mutability. The language also supports low-level features—including references, manual memory management, and FFI—for performance-sensitive code.

This document covers the language surface that every Rune programmer needs first: program structure, literals, types, variables, functions, operators, and control flow. Advanced topics (generics, enums, unions, interfaces, extensions, references, and memory allocation) are summarized briefly and covered in dedicated guides.

## Running a Program

Every executable program provides a `main` entry point:

```rune
func main() -> Int {
    return 0
}
```

When no return type is written, the function returns `Void`:

```rune
import Standard

func main() {
    Console.printLine("Hello, Rune!")
}
```

Import the standard library to access console I/O and other built-in modules:

```rune
import Standard
```

Additional modules are imported the same way.

## Source Files and Structure

- Source files use the `.rune` extension.
- A file may contain imports, type declarations, constants, and functions.
- Top-level declarations do not require a wrapping namespace or class.
- Code is read top to bottom; functions must be declared before use unless forward references are supported for that construct.

A minimal program:

```rune
func main() -> Int {
    let message: String = "Hello, Rune!"
    return 0
}
```

## Comments

```rune
// Line comment

/* Block comment */

/*
 * Multi-line block comment
 */
```

Block comments may nest: `/* outer /* inner */ still outer */`.

## Identifiers

Identifiers start with a letter or underscore and may contain letters, digits, and underscores. By convention:

- Types and protocols use `PascalCase` (`Point`, `Console`)
- Functions and variables use `camelCase` (`calculateArea`, `userName`)
- Constants often use `SCREAMING_SNAKE_CASE` at module scope (`MAX_SIZE`)

### Reserved Keywords

The following words are reserved and cannot be used as ordinary identifiers:

`alias`, `and`, `as`, `break`, `catch`, `const`, `continue`, `defer`,
`dispose`, `distinct`, `else`, `enum`, `extends`, `extension`, `false`,
`final`, `for`, `func`, `if`, `implements`, `import`, `in`, `init`,
`interface`, `internal`, `is`, `let`, `literal`, `loop`, `make`, `match`,
`mutating`, `nil`, `not`, `open`, `operator`, `or`, `override`, `private`,
`public`, `return`, `self`, `Self`, `static`, `struct`, `super`, `throw`,
`throws`, `true`, `try`, `union`, `var`, `while`

The discard wildcard `_` is also reserved: it may be used where the grammar
accepts a discard pattern or placeholder, but not as a bindable name.

## Semicolons

Semicolons are **optional** at the end of statements:

```rune
let x = 10
let y = 20
return x + y
```

Semicolons **are** used to separate multiple statements on one line:

```rune
let a = 1; let b = 2; let c = 3
```

## Types

### Primitive Types

Rune's primitive types have fixed sizes. With one exception (`String`), they are passed and stored by value. Use an explicit type annotation when you need a specific width; otherwise literals pick sensible defaults (see below).

#### Signed integers (fixed-width)

| Type | Size | Range |
|---|---|---|
| `Int8` | 1 byte | −128 … 127 |
| `Int16` | 2 bytes | −32,768 … 32,767 |
| `Int32` | 4 bytes | −2,147,483,648 … 2,147,483,647 |
| `Int64` | 8 bytes | −9,223,372,036,854,775,808 … 9,223,372,036,854,775,807 |

#### Unsigned integers (fixed-width)

| Type | Size | Range |
|---|---|---|
| `UInt8` | 1 byte | 0 … 255 |
| `UInt16` | 2 bytes | 0 … 65,535 |
| `UInt32` | 4 bytes | 0 … 4,294,967,295 |
| `UInt64` | 8 bytes | 0 … 18,446,744,073,709,551,615 |

#### `Int` and `UInt` (platform-width)

`Int` and `UInt` are the platform's native integer types. Unlike the fixed-width types above, their size and range depend on the compilation target.

| Type | On currently supported 64-bit targets |
|---|---|
| `Int` | 8 bytes; same range as `Int64`. **Default type for integer literals** (`42`, `0xFF`, …). |
| `UInt` | 8 bytes; same range as `UInt64`. |

Use `Int32`, `Int64`, `UInt32`, and the other fixed-width types when you need a size that stays the same regardless of platform (for example FFI, file formats, or network protocols).

#### Floating-point

| Type | Size | Notes |
|---|---|---|
| `Float32` | 4 bytes | IEEE 754 single precision (~7 decimal digits). |
| `Float` | 4 bytes | Alias for `Float32`. **Default type for floating-point literals** (`3.14`, `1e10`, …). |
| `Float64` | 8 bytes | IEEE 754 double precision (~15 decimal digits). Prefer for math that needs extra precision. |
| `Double` | 8 bytes | Alias for `Float64`. |

#### Other primitives

| Type | Size | Notes |
|---|---|---|
| `Bool` | 1 byte | `true` or `false`. |
| `Char` | 4 bytes | A single Unicode scalar value (U+0000 … U+10FFFF), stored as a 32-bit code point. Character literals use single quotes: `'A'`, `'ω'`, `'\n'`. |
| `String` | pointer-sized | A reference to heap-allocated text encoded in UTF-8. The value itself is a pointer (8 bytes on 64-bit targets); the string data lives elsewhere. |
| `Void` | 0 bytes | No value. Used when a function returns nothing. |

#### Compile-time limits

Every numeric type exposes `min` and `max` as compile-time constants. Floating-point types also provide `lowest`, `epsilon`, `inf`, and `nan`:

```rune
let biggest: Int32 = Int32.max
let tiny: Float64 = Float64.epsilon

if value >= Int16.min and value <= Int16.max {
    let narrowed = Int16(value)
}
```

Example declarations:

```rune
let count: Int = 100
let byte: UInt8 = 255
let ratio: Float64 = 3.14
let ok: Bool = true
let letter: Char = 'A'
let text: String = "Hello"
```

### Type Aliases

Use `alias` to give an existing type a descriptive name:

```rune
alias UserId = Int
alias Temperature = Float64

func getTemperature(sensor: UserId) -> Temperature {
    return 72.5
}
```

Aliases are also useful for callback signatures and other function types:

```rune
alias AudioCallback = (&Byte?, Int32) -> Void

func setCallback(callback: AudioCallback) {
    // store or register the callback
}
```

### Composite Types (Overview)

Rune provides structs, arrays, sets, maps, enums, unions, interfaces, generics, optionals, and distinct types. Struct syntax is covered below; homogeneous collections (dynamic arrays, fixed inline arrays, hash sets, and hash maps) are covered in [Collections](#collections).

## Literals

### Integer Literals

```rune
let decimal = 42
let grouped = 1_000_000
let hex = 0xFF
let binary = 0b1111_1111
let octal = 0o755
```

All of `255`, `0xFF`, `0b1111_1111`, and `0o377` represent the same value.

### Floating-Point Literals

```rune
let pi = 3.141_592_653_589_793
let large = 1e10
let small = 1.6e-19
```

### Boolean Literals

```rune
let yes: Bool = true
let no: Bool = false
```

### Nil Literal

`nil` represents the empty value of an optional type:

```rune
var selectedName: String? = nil
selectedName = "Rune"

if selectedName != nil {
    let name = selectedName!
}
```

### Character Literals

Character literals use single quotes and support Unicode:

```rune
let a: Char = 'a'
let newline: Char = '\n'
let tab: Char = '\t'
let quote: Char = '\''
let omega: Char = 'ω'
```

### String Literals

```rune
let simple = "Hello, Rune!"
let multiline = """
    Line one
    Line two
    """
```

#### String Interpolation

Embed expressions with `${...}`; values are converted to strings automatically:

```rune
let name = "Rune"
let version = 2.0
let message = "Welcome to ${name} version ${version}!"
let sum = "${x} + ${y} = ${x + y}"
```

#### Raw Strings

Raw strings disable interpolation and most escape processing. Use `#"..."#` syntax:

```rune
let path = #"/Users/name/file.txt"#
let literal = #"This is a literal ${not interpolated}"#
```

### Custom Literal Suffixes

Extensions can define custom suffixes for literal values with `literal`. The
suffix is declared on the literal's base type and returns the type you want the
suffixed literal to produce. Use `_suffix` at the call site:

```rune
distinct Meters = Float

extension Float {
    literal km: Meters => Meters(self * 1000.0)
    literal m: Meters => Meters(self)
}

let distance: Meters = 1.5_km
let height: Meters = 2.0_m
```

Literal suffixes are part of numeric literal syntax; they are not properties on
values:

```rune
let value: Float = 1.5
let distance = value.km // rejected: suffixes only apply to literal values
```

## Variables and Constants

Rune distinguishes three kinds of bindings:

| Keyword | Mutability | When evaluated | Typical use |
|---|---|---|---|
| `const` | Immutable | Compile time | Module-level constants, const expressions |
| `let` | Immutable | Runtime | Values that do not change after initialization |
| `var` | Mutable | Runtime | Values that are reassigned |

```rune
const PI: Float = 3.14159265359
const DOUBLE_PI: Float = PI * 2.0

func example(radius: Float) -> Float {
    let area = PI * radius * radius   // immutable local
    var counter = 0                   // mutable local
    counter = counter + 1
    return area
}
```

Rules:

- `const` bindings must be initialized with compile-time expressions and cannot be reassigned.
- `let` bindings cannot be reassigned after initialization.
- `var` bindings may be reassigned.
- A variable may be declared without an initializer if every code path assigns it before use:

```rune
var label: String
if score >= 60 {
    label = "pass"
} else {
    label = "fail"
}
```

### Type Annotations and Inference

Type annotations are optional when the initializer makes the type clear:

```rune
let x = 42          // inferred as Int
let name = "Alice"  // inferred as String
```

When there is no initializer, the type must be written explicitly:

```rune
var total: Int
```

## Functions

### Declaration Syntax

```rune
func add(a: Int, b: Int) -> Int {
    return a + b
}

func greet(name: String) -> String {
    return "Hello, " + name + "!"
}
```

- Parameters are written as `name: Type`.
- The return type follows `->`. Omit it when the function returns nothing (`Void`).
- Use `return` to produce a value. A function with a non-`Void` return type must return on all paths.

### Named Arguments

Function calls may use positional arguments:

```rune
greet("Alice")
add(10, 20)
```

Or named arguments, which may appear in any order:

```rune
greet(name: "Bob")
createPoint(x: 1.0, y: 2.0)
```

Rules:

- Named arguments use the form `parameterName: value`.
- Do **not** mix positional and named arguments in the same call.
- All required parameters must be supplied.

## Structs

Structs group named fields. Fields are declared with `let`, `var`, or `const`:

```rune
struct Point {
    var x: Float
    var y: Float
}

struct Circle {
    const PI: Float = 3.14159   // type-level constant; not part of the initializer
    var radius: Float = 1.0     // optional field with default
}
```

### Initialization

Struct types cannot be used as values without initialization. Initializers **must** use named arguments:

```rune
let p = Point(x: 3.0, y: 4.0)
let c = Circle(radius: 5.0)
let unit = Circle()            // all-default fields
```

Alternative brace syntax is also supported:

```rune
let origin = Point { x: 0.0, y: 0.0 }
```

Field rules:

| Field form | Initializer |
|---|---|
| `var x: T` or `let x: T` | **Required** — no default |
| `var x: T = value` or `let x: T = value` | **Optional** — default used when omitted |
| `const K = value` | **Excluded** — compile-time constant on the type |

Positional struct arguments are not allowed.

### Member Access

Access fields with dot notation:

```rune
let dx = p2.x - p1.x
```

## Declaration Keywords (Overview)

This section summarizes declaration and modifier keywords that appear in real
programs. Dedicated guides cover the deeper rules for inheritance, interfaces,
extensions, generics, and memory management.

### Access Control

Declarations may be marked `public`, `internal`, or `private`. When omitted,
declarations are `internal`.

```rune
public struct Point {
    public var x: Float
    public var y: Float

    private func magnitudeSquared() -> Float {
        return x * x + y * y
    }
}

internal func helper() {
}
```

Access modifiers can be used on top-level declarations and on struct members.

### Initializers, Finalizers, and `self`

Structs can define an `init` initializer and a `final` finalizer. Inside
instance methods, `self` refers to the current value.

```rune
struct Buffer {
    var count: Int

    init(count: Int) {
        self.count = count
    }

    final() {
        // release resources owned by this value
    }
}
```

Stack values call `init` when constructed and `final` when their lexical scope
exits. Heap values created with `make`, `make!`, or `make?` also call the
selected non-throwing `init`; `dispose` calls `final` before deallocating the
storage.

Methods are non-mutating by default. Use `mutating func` when a method changes
fields of the current value:

```rune
struct Counter {
    var value: Int

    mutating func increment() {
        self.value = self.value + 1
    }
}
```

`Self` names the containing type inside structs, extensions, and interfaces:

```rune
struct Point {
    var x: Int
    var y: Int

    func copy() -> Self {
        return self
    }
}
```

### Enums, Flags, and Unions

Use `enum` for named cases backed by an integer type:

```rune
enum Status: Int {
    ready = 0
    busy = 1
    done = 2
}

let current = Status.busy
```

Use `@flags enum` for bitmask values:

```rune
@flags
enum WindowFlags: UInt32 {
    shown = 0x00000004
    resizable = 0x00000020
}
```

Use `union` for tagged alternatives. Union values are commonly handled with
`match`:

```rune
union Command {
    step(Int, Int)
    halt
}

func energy(command: Command) -> Int {
    match command {
        .step(dx, dy) => return dx + dy
        .halt => return 0
    }
}
```

### Distinct Types

`distinct` creates a new type with the same representation as another type, but
without implicit compatibility. Use it for IDs, handles, and other values that
should not mix accidentally.

```rune
distinct UserId = Int
distinct ProductId = Int

let user = UserId(7)
let product = ProductId(7)
```

### Interfaces and Inheritance

An `interface` declares required methods. A struct uses `implements` to conform:

```rune
interface Drawable {
    func draw()
}

struct Circle implements Drawable {
    func draw() {
        // draw the circle
    }
}
```

Structs are closed to inheritance by default. Mark a base type `open`, inherit
with `extends`, and mark overridden methods with `override`. Use `super` inside
an instance method to call the base implementation.

```rune
open struct Shape {
    open func area() -> Int {
        return 0
    }
}

struct Square extends Shape {
    var side: Int

    override func area() -> Int {
        return side * side
    }

    func baseArea() -> Int {
        return super.area()
    }
}
```

### Extensions and Static Scope

`extension` adds methods to an existing type:

```rune
extension Point {
    func distanceSquared() -> Float {
        return x * x + y * y
    }
}
```

`static extension` adds type-level members, such as factories or constants:

```rune
static extension Point {
    func zero() -> Point {
        return Point(x: 0.0, y: 0.0)
    }
}
```

`static Name { ... }` groups declarations under a static namespace-like scope,
which is useful for modules and FFI bindings:

```rune
static Math {
    const tau: Float = 6.283185
}
```

### Manual Allocation

`make` constructs a heap-allocated value and returns a reference. Release it
with `dispose` when ownership is finished:

```rune
let queue = make AudioQueue(initialCapacity: 4096)
defer dispose queue
```

## Collections

Rune has four built-in homogeneous collection types. They share bracket literal syntax `[ … ]`, but the **type annotation** tells the compiler which kind you mean.

| Kind | Type syntax | Literal syntax | Size | Typical use |
|---|---|---|---|---|
| **Dynamic array** | `[T]` or `Array<T>` | `[1, 2, 3]` | Grows at runtime | Lists, buffers, unknown length |
| **Inline array** | `[T, N]` (and `_` forms below) | `[1, 2, 3]` | Fixed at compile time | Struct fields, lookup tables, stack data |
| **Hash set** | `HashSet<T>` | `[1, 2, 3]` | Grows at runtime | Unique values, membership tests |
| **Hash map** | `HashMap<K, V>` | `[key: value, …]` | Grows at runtime | Dictionaries, indexes |

All four are **ordered for iteration** where iteration is supported. Only hash containers require `Hashable` keys (sets: elements; maps: keys).

Trailing commas are allowed in collection literals.

### Dynamic arrays

A **dynamic array** stores elements in heap-backed storage that can grow or shrink. Use bracket shorthand `[T]` or the generic form `Array<T>` — they are the same type.

```rune
var numbers: [Int] = [1, 2, 3, 4, 5]
let empty: Array<Int> = []
let matrix: [Array<Int>] = [
    [1, 2, 3],
    [4, 5, 6],
]

let first = numbers[0]
numbers.append(element: 42)
numbers[1] = 99
```

Common properties and methods:

| Member | Meaning |
|---|---|
| `count` | Number of elements |
| `isEmpty` | `true` when `count == 0` |
| `first` / `last` | `Option` of the first or last element |
| `append(element:)` | Append at the end (mutable binding) |
| `insert(element:at:)` | Insert at an index |
| `remove(at:)` | Remove and return element at index |
| `contains(element:)` | Membership test |

Index with `array[i]`; bounds must be valid at runtime. Use `for value in array` to iterate elements.

### Inline arrays (fixed size)

An **inline array** has a fixed length known at compile time. The element type and length are part of the type, so it can live inside structs, on the stack, or in other fixed layouts without heap allocation for the elements themselves.

**Type syntax** uses a comma between element type and size:

| Form | Meaning |
|---|---|
| `[T, N]` | Element type `T`, exactly `N` elements (`N` is an integer literal) |
| `[T, _]` | Element type `T`; `N` inferred from an array literal initializer |
| `[_, N]` | `N` fixed; element type inferred from the literal |
| `[_, _]` | Both inferred from the literal |

The single-parameter form `[T]` (no comma) is **not** an inline array — it is a dynamic array (see above).

```rune
// Explicit type and size (common for constants and struct fields)
const DIGIT_0: [UInt8, 7] = [0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E]

// Infer length from the literal
const ROW: [UInt8, _] = [1, 2, 3, 4]

// Infer element type from the literal (here: Int)
const VALUES: [_, 3] = [10, 20, 30]
```

Rules for `_`:

- A `_` placeholder **requires** an array literal initializer on the same binding (or struct field). You cannot write `let data: [UInt8, _]` without `= [ … ]`.
- If you fix the size (`[_, N]`), the literal must have exactly `N` elements.
- If element type is inferred (`[_, N]` or `[_, _]`), every element must agree on a single type; mixed types (for example `[1, true]`) are rejected as ambiguous — specify the type explicitly (`[UInt8, _]`).
- Function parameters and struct fields without initializers must use fully explicit `[T, N]` (no `_`).

Inline arrays support indexing, `count`, `isEmpty`, `first`, `last`, and `contains(element:)` like dynamic arrays, but they do **not** support `append` or `remove` — the length is fixed.

```rune
struct Glyph {
    var pixels: [UInt8, 8]
}

let g = Glyph(pixels: [0, 1, 1, 0, 0, 1, 1, 0])
let bit = g.pixels[2]
```

Use inline arrays when the size is known at compile time (font glyphs, FFI structs, color tables). Use dynamic arrays when the size varies at runtime.

### Hash sets

A **hash set** holds **unique** elements. Elements must implement `Hashable` (built-in types such as `Int`, `String`, and `Char` already do; custom structs can implement `Hashable`).

```rune
let ids: HashSet<Int> = [1, 2, 3]
let empty: HashSet<String> = []
let chars: HashSet<Char> = ['a', 'b', 'c']
```

Use a **list literal** `[ … ]` for sets — the same brackets as arrays, but the annotated type is `HashSet<T>`, not `[T]`.

| Member | Meaning |
|---|---|
| `count` / `isEmpty` | Size |
| `insert(element:)` | Add an element (mutable binding) |
| `remove(element:)` | Remove by value |
| `contains(element:)` | Membership test |

Do not use dictionary syntax (`[key: value]`) for sets. Duplicate values in a literal collapse to one entry.

### Hash maps

A **hash map** maps **hashable keys** to values. Keys must implement `Hashable`.

```rune
let scores: HashMap<String, Int> = ["alice": 100, "bob": 85]
let empty: HashMap<Int, String> = [:]
let codes: HashMap<Char, Int> = ['a': 1, 'b': 2]
```

Use a **dictionary literal** with `key: value` pairs inside `[ … ]`. An empty map is written `[:]`, not `[]` (`[]` is a list literal and suits arrays or sets, not maps).

| Member | Meaning |
|---|---|
| `count` / `isEmpty` | Number of key/value pairs |
| `insert(key:value:)` | Insert or update (mutable binding) |
| `remove(key:)` | Remove by key |
| `contains(key:)` | Whether a key is present |
| `get(key:)` | `Option` of the value for a key |

Look up values with `get` (not subscript syntax). The result is an `Option`:

```rune
let maybeScore = scores.get(key: "alice")
if maybeScore != nil {
    let score = maybeScore!
    // use score
}
```

### Choosing a collection type

- **Dynamic array** — ordered sequence, length changes at runtime (`append`, indexed access).
- **Inline array** — ordered sequence, fixed length in the type (`[T, N]` or inferred `[T, _]`).
- **Hash set** — uniqueness and fast membership; iteration order is not specified.
- **Hash map** — key → value lookup; use dictionary literals and `get(key:)`.

When a literal uses `[ … ]`, always pair it with an explicit type annotation (`var xs: [Int] = …`, `let s: HashSet<Int> = …`, `let m: HashMap<String, Int> = …`) so the compiler selects the correct collection kind.

## Operators

### Arithmetic

| Operator | Meaning |
|---|---|
| `+` `-` `*` `/` `%` | Addition, subtraction, multiplication, division, remainder |
| `+=` `-=` `*=` `/=` `%=` | Compound assignment |

### Comparison

| Operator | Meaning |
|---|---|
| `==` `!=` | Equal, not equal |
| `<` `<=` `>` `>=` | Ordered comparison |

### Logical

Rune supports keyword-based logical operators (preferred in new code):

| Operator | Meaning |
|---|---|
| `and` | Logical AND (short-circuit) |
| `or` | Logical OR (short-circuit) |
| `not` | Logical NOT |

```rune
let ok = age >= 18 and hasLicense
let weekend = isSaturday or isSunday
let disabled = not isEnabled
```

### Bitwise

| Operator | Meaning |
|---|---|
| `&` `\|` `^` `~` | Bitwise AND, OR, XOR, NOT |
| `<<` `>>` | Left shift, right shift |
| `&=` `\|=` `^=` `<<=` `>>=` | Compound bitwise assignment |

### Ternary Conditional

```rune
let label = score >= 60 ? "pass" : "fail"
let abs = x >= 0 ? x : -x
```

### Casts and Type Checks

Use `as` for casts and `is` for runtime type checks:

```rune
let shape = square as Shape
let isSquare = shape is Square
let maybeSquare = shape as? Square
let squareAgain = shape as! Square
```

`as?` produces an optional value. `as!` forces the cast and should only be used
when failure is impossible or should stop execution.

### Operator Declarations

Use `operator` inside a struct or extension to overload supported operators:

```rune
struct Vector {
    var x: Int
    var y: Int

    operator +(other: Vector) -> Vector {
        return Vector(x: x + other.x, y: y + other.y)
    }

    operator ==(other: Vector) -> Bool {
        return x == other.x and y == other.y
    }
}
```

Use `mutating operator` when the operator changes `self`.

### Operator Precedence

When in doubt, use parentheses. In general, unary operators bind tighter than multiplicative operators, which bind tighter than additive operators; comparisons bind tighter than logical operators. User-defined operator overloads follow the same precedence rules as their built-in counterparts.

## Control Flow

All control statements with bodies use `{` `}` blocks.

### `if` / `else`

```rune
if temperature > 30.0 {
    status = "hot"
} else if temperature > 20.0 {
    status = "warm"
} else {
    status = "cool"
}
```

### `while`

```rune
var i = 0
while i < 10 {
    i = i + 1
}
```

### `for`

**Range iteration** (preferred for indexed loops):

```rune
for i in 0..10 {       // 0, 1, …, 9  (half-open)
    sum = sum + i
}

for i in 0..=10 {      // 0, 1, …, 10 (inclusive)
    sum = sum + i
}

for i in ..10 {        // from 0 up to (but not including) 10
    // ...
}
```

Rune does not have initializer/condition/increment `for` loops. Use `for … in` for ranges and collections, or a `while` loop when you need custom increment logic.

### `loop`

An infinite loop, exited with `break`:

```rune
loop {
    if done {
        break
    }
}
```

### `break` and `continue`

```rune
for i in 0..100 {
    if i == 50 {
        break
    }
    if i % 2 == 0 {
        continue
    }
}
```

### `defer`

`defer` schedules work to run when the current scope exits. Deferred statements
run for normal exits, `return`, `break`, and `continue`.

```rune
func writeFile(path: String) {
    let file = openFile(path)
    defer file.close()

    file.write("hello")
}
```

Multiple defers in the same scope run in last-in, first-out order.

Struct finalizers for stack values also run on scope exit. Control-flow bodies
such as `if`, `if let`, guard `else`, `while`, `loop`, and `for` are lexical
scopes for this purpose.

### Throwing Functions

Use `throws(ErrorType)` on a function that can fail and `throw` to leave it with
an error value:

```rune
func divide(a: Int, b: Int) throws(String) -> Int {
    if b == 0 {
        throw "division by zero"
    }
    return a / b
}
```

Handle throwing code with `try` / `catch` blocks:

```rune
try {
    let value = divide(a: 20, b: 4)
    Console.printLine("${value}")
}
catch {
    Console.printLine("division failed")
}
```

In expression context, use `try?` to turn failure into `nil`, or `try!` to force
success:

```rune
let maybeValue = try? divide(a: 10, b: 0)
let value = try! divide(a: 10, b: 2)
```

Throwing initializers use the same handling. Construct them with bare `try` from
a matching throwing function, with `try?` / `try!`, or inside a `try` block.
Constructing a value with a throwing initializer without a try context is
rejected.

### `match` (Overview)

`match` performs pattern matching on enums, unions, and other types:

```rune
match direction {
    .north => Console.printLine(simpleNameOf(Direction.north))
    .south => {
        Console.printLine(simpleNameOf(Direction.south))
    }
}
```

Compile-time name macros support two forms: `simpleNameOf<Type>()` for type names, and `simpleNameOf(Type.case)` for enum or flags cases (returning the case identifier, not its raw value). `qualifiedNameOf` works the same way with module-qualified paths.

Each arm uses `=>`. A single expression may follow directly; multiple statements require a block.

## Statements and Blocks

A block is a sequence of statements enclosed in braces. Blocks introduce scope: bindings declared inside a block are not visible outside it.

```rune
func example() -> Int {
    let outer = 1
    {
        let inner = 2
        // `inner` is visible here
    }
    // `inner` is not visible here
    return outer
}
```
