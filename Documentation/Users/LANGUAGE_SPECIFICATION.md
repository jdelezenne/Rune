# Rune Language Specification

This document describes the Rune language.

## Contents

1. [Source Model](#source-model)
2. [Lexical Structure](#lexical-structure)
3. [Program Structure](#program-structure)
4. [Names, Access, and Attributes](#names-access-and-attributes)
5. [Types](#types)
6. [Declarations](#declarations)
7. [Statements](#statements)
8. [Expressions](#expressions)
9. [Functions and Calls](#functions-and-calls)
10. [Structs and Members](#structs-and-members)
11. [Enums, Flags, and Unions](#enums-flags-and-unions)
12. [Interfaces and Extensions](#interfaces-and-extensions)
13. [Generics](#generics)
14. [Option, Result, and Error Handling](#option-result-and-error-handling)
15. [Collections and Ranges](#collections-and-ranges)
16. [References and Memory](#references-and-memory)
17. [Modules and Imports](#modules-and-imports)
18. [C Bindings](#c-bindings)
19. [Threading](#threading)
20. [Entry Points](#entry-points)

## Source Model

Rune source files use the `.rune` extension. A file is a sequence of
declarations and statements. Top-level code may declare imports,
variables, constants, functions, types, interfaces, extensions, static scopes,
and attributed declaration groups.

Semicolons are optional at statement and declaration boundaries where the
boundary can be inferred:

```rune
let x = 1
let y = 2;
return x + y
```

Semicolons are useful when multiple declarations or statements appear on one
line.

## Lexical Structure

### Whitespace and Comments

Whitespace separates tokens but is otherwise insignificant. Newlines are not
significant, although a few postfix forms are position-sensitive.

Rune supports line comments and block comments:

```rune
// line comment

/* block comment */
```

Block comments may nest.

### Identifiers

Identifiers name values, functions, types, modules, enum cases, union cases,
members, parameters, and labels. The single underscore `_` is the discard
wildcard. It is not a bindable name, but it is accepted in binding patterns and
some inference placeholders.

### Keywords

The following words are reserved:

```text
alias and as break catch const continue defer dispose distinct else enum
extends extension false final for func if implements import in init
interface internal is let literal loop make match mutating nil not open
operator or override private public return self Self static struct super
throw throws true try union var while
```

Reserved words cannot be used as ordinary identifiers. Some reserved words are
used only in specific declaration or expression forms.

### Literals

Rune has the following literal types:

| Type | Example |
|---|---|
| Integer | `42` |
| Floating-point | `3.14` |
| String | `"hello"` |
| Character | `'x'` |
| Boolean | `true`, `false` |
| Nil | `nil` |
| Array | `[1, 2, 3]` |
| Set | `[1, 2, 3]` (typed as `HashSet`) |
| Map | `["alice": 10, "bob": 20]` |
| Tuple | `(1, "one")` |

Numeric literals may use `_` as a digit separator. Integer literals support decimal, hexadecimal, and binary notation. Floating-point literals support exponent notation.

| Notation | Example |
|---|---|
| Decimal | `42`, `1_000_000` |
| Hexadecimal | `0xFF`, `0xFF_FF` |
| Binary | `0b1010`, `0b1111_0000` |
| Floating-point | `3.14`, `1.5e10`, `1.5e-3` |

String literals use double quotes. Interpolation is written with `${...}`:

```rune
let message = "name=${name}, count=${count}"
```

Multiline and raw string literals use triple double quotes. The content spans multiple lines and is treated as raw (no escape sequences are processed):

```rune
let path: String = """C:\Users\name\file.txt"""

let query: String = """
    SELECT *
    FROM users
    WHERE id = 42
    """
```

The closing `"""` determines the indentation level; leading whitespace up to that column is stripped from each line.

Character literals use single quotes and represent one Unicode scalar value.
Common escape sequences and Unicode escapes are supported in character literals.

### Operators and Delimiters

Rune recognizes:

| Category | Operators |
|---|---|
| Unary / Binary | `+ - * / % ! not ~ & ^ \|` |
| Comparison | `== != < <= <=> > >=` |
| Logic | `and or && \|\|` |
| Assignment | `= += -= *= /= %= &= \|= ^= <<= >>=` |
| Range | `start..end` `start..=end` `start..` `..end` `..=end` |
| Optional | `? ?? ?. !` |
| Cast / Check | `as as? as! is` |
| Punctuation | `( ) { } [ ] , ; : -> => . ... @ #` |

`&&` and `||` are accepted as logical operators. The keyword forms `and` and
`or` are preferred. In expression context, `*` and `&` can also create
immutable and mutable references, while postfix `^` dereferences a reference.

## Program Structure

Example:

```rune
import Standard.*
import Standard.Math

public const value: Int = 42

public struct Point {
    var x: Int
    var y: Int
}

func main() -> Int {
    return Math.abs(point.x + point.y) / value
}
```

Top-level declarations default to `internal` unless another access modifier is
written. In executable files, top-level executable statements form an implicit
entry point unless an explicit `main` function is present.

Declaration order is generally not significant for types and functions in a
file or module. A function may mention a type declared later, and a static scope
may be extended by multiple declarations with the same scope name.

## Names, Access, and Attributes

### Access Modifiers

Declarations and members may be marked:

```rune
public
internal
private
```

`public` declarations are exported from a module. `internal` declarations are
module-local. `private` declarations are file- or enclosing-context-local. The
default is `internal`.

Access modifiers must come after attributes.

### Attributes

Attributes begin with `@` and may optionally carry parenthesized arguments:

```rune
@link("c")
@include("string.h")
static CStringLib {
    @symbol("strlen")
    func length(value: CString) -> UInt64
}
```

Common implementation attributes include:

| Attribute | Purpose |
|---|---|
| `@builtin(...)` | Binds a Rune declaration to a compiler/runtime built-in identity. |
| `@symbol(...)` | Names a native C symbol. |
| `@link(...)` | Names a native library to link. |
| `@include(...)` | Names a C header required for C binding emission. |
| `@platform(...)` | Restricts declarations to a platform in supported contexts. |
| `@layout(...)` | Controls explicit layout policy for supported type declarations. |

Attributes can also be applied to a braced declaration group. The group is
expanded and the attributes are inherited onto each compatible child
declaration:

```rune
@link("c")
@include("math.h")
{
    @symbol("sqrt")
    func sqrt64(value: Float64) -> Float64
}
```

Attributed declaration groups are allowed in declaration contexts, not inside
ordinary statement blocks.

## Types

Rune is statically typed. Type annotations use `name: Type`:

```rune
let count: Int = 42
func length(value: String) -> Int { return value.length }
```

### Primitive Types

Rune provides the following built-in primitive types:

| Type | Meaning |
|---|---|
| `Int8`, `Int16`, `Int32`, `Int64` | Signed integers. |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Unsigned integers. |
| `Int` | Native signed integer. |
| `UInt` | Native unsigned integer. |
| `Float32`, `Float` | 32-bit floating point. |
| `Float64`, `Double` | 64-bit floating point. |
| `Bool` | Boolean. |
| `Char` | Unicode scalar value. |
| `Void` | No value. |

The following standard types are also built-in and receive special treatment:

```text
String CString StringSlice Option Result Array Slice InlineArray HashMap HashSet
```

### Named and Qualified Types

Types are named by identifiers:

```rune
Point
Array<Int>
Standard.Console
```

Qualified type names use `.` between components.

### Generic Types

Generic type arguments use angle brackets:

```rune
Array<Int>
HashMap<String, Int>
Result<Int, String>
```

Nested generic closing brackets written as `>>` are automatically split in type position.

### Optional Types

`T?` is shorthand for `Option<T>`. Repeated suffixes nest:

```rune
let maybeNumber: Int?
let nested: String??
```

`nil` is only valid for optional contexts. A non-optional value may implicitly
wrap into `Option<T>` when the destination type expects it:

```rune
let missing: Int? = nil
let present: Option<Int> = 42

func first() -> Int? {
    return 1
}
```

### Reference Types

Rune has immutable and mutable reference types:

```rune
*Int  // immutable reference to Int
&Int  // mutable reference to Int
```

Mutable references can be used where immutable references are expected when the
underlying element type matches.

References can also be optional:

```rune
let immutable: *Int? = nil
let mutable: &Int? = nil
```

### Function Types

Function types use parameter type lists and an arrow:

```rune
(Int, Int) -> Int
() -> Void
```

In all function type declarations, a parenthesized parameter type list without an explicit return type defaults to `Void`:

```rune
func greet(name: String) { }
```

### Tuple Types

Tuple types use parenthesized comma-separated type lists:

```rune
(Int, String)
```

### Bracket Type Syntax

Bracket type syntax maps to standard collection types:

```rune
[T]       // Array<T>
[T, N]    // InlineArray<T, N>
[T, _]    // InlineArray<T, inferred size>
[_, N]    // InlineArray<inferred element, N>
[_, _]    // InlineArray<inferred element, inferred size>
```

Inline array size arguments are integer literals, const integer names, or `_`.

### Type Identity and Conversion

Aliases are alternate names for an existing type and do not create new nominal
identity. Distinct declarations do create new nominal identity:

```rune
alias Celsius = Float64
distinct UserId = Int

let temperature: Celsius = 100.0
let id = UserId(7)
```

Numeric conversion is expressed with type construction:

```rune
let half: Float = 12.5
let rounded = Int(half)
```

The `as`, `as?`, `as!`, and `is` operators are for inheritance-related runtime
type operations. A plain `as` cast accepts same-type casts but rejects value
upcasts that would slice a derived struct into a base struct. Use references for
dynamic base/interface views.

## Declarations

### Variables and Constants

Bindings use `var`, `let`, or `const`:

```rune
var counter = 0
let immutable = 10
const base: Int = 21
```

`var` creates a mutable binding. `let` creates an immutable binding. `const`
creates a compile-time constant.

A variable declaration must have a type annotation, an initializer, or both.
`const` declarations must have an initializer unless they are type-annotated
declarations used for built-in declarations.

Tuple destructuring is supported for local binding declarations:

```rune
let (x, y) = pointPair()
```

Tuple destructuring requires an initializer and does not take a type annotation.
The discard wildcard may be used in binding positions:

```rune
let (_, value) = pair
```

### Type Aliases

Aliases introduce an alternate name for an existing type:

```rune
alias UserId = Int
alias Predicate = (Int) -> Bool
```

Aliases do not create a distinct type.

### Distinct Types

Distinct declarations create new nominal types. A distinct type with a base type wraps that base type but is not implicitly convertible back to it:

```rune
distinct Meters = Float
```

A distinct type without a base type is an opaque type with no known representation, typically used for C interoperability:

```rune
distinct Token
```

Construction uses normal initializer syntax:

```rune
let distance = Meters(1.5)
```

### Static Scopes

Static scopes group declarations under a named scope:

```rune
public static Math {
    public const PI: Float64 = 3.141592653589793

    public func sin(x: Float64) -> Float64
}
```

Static scopes may contain nested declarations, including nested static scopes.
Members are accessed by qualification:

```rune
Math.PI
Math.sin(x: 1.0)
```

## Statements

### Blocks

Blocks use braces and contain declarations or statements:

```rune
{
    let x = 1
    return x
}
```

### If Statements

```rune
if condition {
    return 1
} else {
    return 0
}
```

The condition must be a `Bool`.

An `if` condition may bind a non-`nil` optional value with `if let`:

```rune
if let value = maybeValue {
    use(value)
} else {
    useFallback()
}
```

The initializer must have type `Option<T>` or `T?`. When the optional contains a
value, the `then` block runs with `value` bound as `T`. When the optional is
`nil`, the `else` branch runs if present. The binding is visible only inside the
`then` block, and the optional initializer is evaluated once.

The `then` and `else` bodies are normal lexical scopes. Values declared inside
either body are finalized when that body exits, including exits by `return`,
`throw`, `break`, or `continue`.

### Guard Statements

`guard` is a contextual statement keyword. It is recognized as a guard only at
the start of a statement, so it does not become a reserved identifier everywhere
in the language.

```rune
guard condition else {
    return
}
```

The condition must be a `Bool`. The `else` block is required and must exit the
current path with `return`, `throw`, `break`, or `continue`.

`guard let` unwraps an optional and binds the value after the guard:

```rune
guard let value = maybeValue else {
    return
}

use(value)
```

The initializer must have type `Option<T>` or `T?`. It is evaluated once. When it
contains a value, execution continues after the guard with `value` bound as `T`.
When it is `nil`, the `else` block runs and must exit.

The `else` body is a normal lexical scope. Values declared in it are finalized
before the required exit completes. A successful `guard let` binding is visible
after the guard and is finalized with its enclosing scope.

### While and Loop

```rune
while condition {
    step()
}

loop {
    break
}
```

`while` checks a boolean condition before each iteration. `loop` is an
unconditional loop.

### For-In

```rune
for value in values {
    total = total + value
}
```

The iterable expression must provide the collection/iterator behavior required
by the standard library.

Built-in arrays, slices, string slices, hash sets, hash maps, and ranges use the
standard iterable interfaces. Non-iterable values are rejected:

```rune
for value in 0..10 {
    total = total + value
}
```

Ranges may include an optional `by` stride:

```rune
for i in 0..10 by 2 {
    total = total + i
}

for y in 0.0..=height by 64.0 {
    drawRow(y: y)
}
```

The stride expression must be compatible with the range element type.

### Break and Continue

```rune
break
continue
```

These are valid in loop bodies.

### Return

```rune
return
return value
```

Returning a value from a `Void` function is rejected. Non-`Void` functions must
return a value on all paths that complete normally.

### Throw

```rune
throw "error message"
throw .failed
```

`throw` is valid in functions declared with `throws`.

### Try Statement

```rune
try {
    risky()
}
catch let error {
    Console.printLine(error)
}
catch {
    return
}
```

Catch clauses support a bound error, a pattern, or a catch-all block. A `try`
block must have at least one `catch` clause. Variables declared inside a catch
body follow normal block scoping, and `catch let name` binds the thrown error
value with the function's declared error type.

### Match

```rune
match command {
    .step(dx, dy) => return dx + dy
    .halt => return 0
    _ => return -1
}
```

Match supports any type with expression patterns plus nominal leading-dot cases:

- `._case` and `._case(...)` for enum, flags, and union values
- expression patterns (for example `0`, `"ok"`, `true`)
- `_` as the default case

Matches over enum, flags, and union values remain exhaustiveness-checked by
declared cases unless a default case is present. For non-nominal values (for
example integers, strings, and booleans), use expression patterns and include a
default case `_`.

Pattern expression types must be compatible with the matched expression type.
Duplicate cases, unknown nominal cases, missing cases, and bindings on unit
cases are rejected.

Example with expression patterns:

```rune
func classify(value: Int) -> String {
    match value {
        0 => return "zero"
        1 => return "one"
        _ => return "many"
    }
}

func answer(text: String) -> Int {
    match text {
        "yes" => return 1
        "no" => return 0
        _ => return -1
    }
}
```

Associated values are bound by position. The binding arity must match the case:

```rune
union Shape {
    circle(Float64)
    rectangle(Float64, Float64)
}

func area(shape: Shape) -> Float64 {
    match shape {
        .circle(radius) => return radius * radius
        .rectangle(width, height) => return width * height
    }
}
```

### Defer

`defer` schedules work for scope exit:

```rune
defer cleanup()
defer {
    close(file)
}
defer dispose resource
```

`defer` is valid in function bodies and nested statement blocks. It is rejected
at module level, in initializers, and in finalizers.

### Dispose

`dispose` releases a value allocated with `make`. See [References and Memory](#references-and-memory).

## Expressions

### self and super

`self` refers to the current instance inside a method or initializer. `super` refers to the inherited implementation in a subtype context.

### Leading-Dot Case References

Enum and union cases can be referenced with a leading dot when the type can be inferred from context:

```rune
let direction = .north
throw .divisionByZero
```

### Operator Precedence

From lowest to highest precedence:

| Level | Operators and forms | Associativity |
|---|---|---|
| Assignment | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=` | right |
| Ternary | `condition ? a : b` | right |
| Nil coalescing | `??` | left |
| Logical or | `or`, `||` | left |
| Logical and | `and`, `&&` | left |
| Bitwise or | `|` | left |
| Bitwise xor | `^` | left |
| Bitwise and | `&` | left |
| Equality | `==`, `!=` | left |
| Type check and comparison | `is`, `<`, `<=`, `<=>`, `>`, `>=` | left |
| Range | `..`, `..=` | non-chaining |
| Shift | `<<`, `>>` | left |
| Additive | `+`, `-` | left |
| Multiplicative | `*`, `/`, `%` | left |
| Unary | `try?`, `try!`, `!`, `not`, `~`, unary `-`, `*`, `&` | right |
| Cast | `as`, `as?`, `as!` | left |
| Postfix | calls, subscripts, `!`, `.`, `?.`, postfix `^` | left |

### Assignment

Assignment targets may be variables, members, indexes, or dereferenced
references:

```rune
x = 1
point.x = 2
array[i] = value
ref^ = 3
```

Compound assignment is accepted for variable targets:

```rune
x += 1
mask &= flag
```

### Calls and Construction

Calls use parentheses:

```rune
add(a: 1, b: 2)
Point(x: 3, y: 4)
Box<Int>(value: 21)
```

Arguments may be positional or named, but one call cannot mix positional and
named arguments. Spread arguments use `...`:

```rune
sum(...values)
```

Named arguments bind by parameter name and may be reordered:

```rune
func render(id: Int, enabled: Bool = false, title: String = "untitled") -> Int {
    if enabled { return id }

    return 0
}

let a = render(id: 1)
let b = render(title: "ok", id: 3)
```

Struct field initializers use named arguments. Required fields without defaults
must be supplied, extra field names are rejected, and defaulted fields may be
omitted:

```rune
struct Point {
    var x: Int = 0
    var y: Int = 0
}

let origin = Point()
let shifted = Point(y: 4, x: 2)
```

### Member Access

Members use `.`:

```rune
point.x
point.sum()
Status.ready
```

Optional member access uses `?.` and returns an optional member result:

```rune
maybeUser?.name
maybeUser?.tags[0]
maybeUser?.upperName()
```

### Optional Unwrap and Nil Coalescing

Postfix `!` force-unwraps an option:

```rune
let value = maybeValue!
```

`??` supplies a fallback:

```rune
let count = maybeCount ?? 0
```

The left operand of `??` must be optional, and the fallback value must match the
optional's wrapped type. The fallback is evaluated only when the optional is
`nil`.

### Casts and Type Checks

```rune
let same = value as ValueType
let maybe = shape as? Square
let forced = shape as! Square

if shape is Square {
    return 1
}
```

`as` is not a numeric conversion operator. Numeric primitive conversions use
Swift-style initializer families and are never implicit. The unlabeled
initializer form, such as `Int(value)`, is value-preserving and must reject or
trap when the source value is not representable by the destination type.
`value as! Int` is the same checked conversion and traps on failure. `value as?
Int` returns `nil` when the source value cannot be represented by the
destination type. `Int.exactly(value)` is the static helper spelling for the same
optional conversion. `Int.clamping(value)` clamps integer values to the
destination range. `Int.truncating(value)` preserves the source integer bit
pattern as far as the destination width permits by extending or truncating.

For related open structs, `as?` returns an optional downcast result, `as!` forces
a downcast, and `is` returns a `Bool`. Checks and casts between unrelated
structs are rejected.

### References

Unary `&` forms a mutable reference, unary `*` forms an immutable reference,
and postfix `^` dereferences:

```rune
func increment(value: &Int) {
    value^ = value^ + 1
}

func read(value: *Int) -> Int {
    return value^
}

var count = 41
increment(&count)
let copy = read(*count)
```

Member access through a reference implicitly dereferences for field reads,
method calls, and mutable field assignment when the reference is mutable:

```rune
struct Counter {
    var value: Int

    mutating func increment() {
        value = value + 1
    }
}

func bump(counter: &Counter) {
    counter.value = counter.value + 1
    counter.increment()
}
```

The explicit form is also accepted:

```rune
counter^.value = counter^.value + 1
```

### Lambdas

Lambda expressions use parameter lists and `->`:

```rune
let double = (x: Int) -> Int x * 2
let action = () -> {
    Console.printLine("done")
}
```

If the token after `->` is not a type name, the lambda return type defaults to
`Void` and the following expression or block is parsed as the body.

### Array, Set, Map, and Tuple Literals

Array and set literals use brackets with comma-separated elements:

```rune
let numbers: Array<Int> = [1, 2, 3]
let ids: HashSet<Int> = [1, 2, 3, 3]
```

Map literals use key/value pairs:

```rune
let scores: HashMap<String, Int> = ["alice": 10, "bob": 20]
let empty: HashMap<String, Int> = [:]
```

Tuple literals use parentheses with commas:

```rune
let pair = (1, "one")
```

### Ranges

Rune supports half-open and inclusive ranges:

```rune
0..10
0..=10
..10
..=10
10..
```

## Functions and Calls

Function declarations use `func`:

```rune
func add(a: Int, b: Int) -> Int {
    return a + b
}
```

If no return type is written, the return type defaults to `Void`:

```rune
func log(value: String) {
    Console.printLine(value)
}
```

### Parameters

Parameters are named and typed:

```rune
func move(dx: Int, dy: Int)
```

Default parameter values are supported. Once a parameter has a default value,
later parameters must also have defaults:

```rune
func connect(host: String, port: Int = 80)
```

At call sites, arguments may be positional or named. Positional calls follow the
declaration order. Named calls bind by name and may omit defaulted parameters:

```rune
func clamp(x: Int, min: Int = 0, max: Int = 100) -> Int {
    if x < min { return min }
    if x > max { return max }

    return x
}

let value = clamp(x: 6, min: 2)
```

Variadic parameters use `...` after the parameter type and must be last:

```rune
func sum(values: Int...)
```

### Compile-Time Evaluation

Functions are ordinary declarations. Compile-time function evaluation is not
spelled with `const func`; a const binding cannot call a function unless the
call is part of an explicit compile-time evaluation feature.

### Throwing Functions

Throwing functions write `throws` before the return arrow:

```rune
func divide(a: Int, b: Int) throws(String) -> Int {
    if b == 0 {
        throw "division by zero"
    }

    return a / b
}
```

`throws` without an explicit error type is accepted and uses `Void` as the
marker error type.

### Operators

Operators are declared with `operator`:

```rune
operator +(other: Vector) -> Vector { ... }
operator !() -> Bool { ... }       // prefix
operator ()! -> T { ... }          // postfix
operator [](index: Int) -> T { ... }
operator []=(index: Int, value: T)
```

Instance operators cannot be `static`. Subscript assignment `[]=` requires at
least one parameter.

## Structs and Members

Structs declare fields, properties, initializers, finalizers, and methods:

```rune
open struct Point {
    var x: Int
    var y: Int

    init(x: Int, y: Int) {
        self.x = x
        self.y = y
    }

    func lengthSquared() -> Int {
        return x * x + y * y
    }
}
```

Finalizers use the `final` keyword and run when a value is destroyed:

```rune
struct Resource {
    var name: String

    init(name: String) {
        self.name = name
    }

    final() {
        // release resources owned by this value
    }
}
```

For stack values, initializer and finalizer calls are part of the value's
lexical lifetime. Constructing a struct value calls the selected `init`; leaving
the declaring scope calls `final` if the type declares one. This applies to
function bodies and nested control-flow bodies such as `if`, `if let`, guard
`else`, `while`, `loop`, `for`, and `match` case bodies. Finalizers also run
when a scope exits early with `return`, `throw`, `break`, or `continue`.

For heap values, `make`, `make!`, and `make?` allocate storage and then call the
selected non-throwing initializer. `dispose` calls `final` before returning the
storage to the allocator.

```rune
struct Resource {
    var value: Int

    init(value: Int) {
        self.value = value
    }

    final() {
        // release resources owned by this value
    }
}

func stackLifetime() {
    let value = Resource(value: 1) // calls init
}                                 // calls final

func heapLifetime() {
    let value = make! Resource(value: 1) // allocates, then calls init
    dispose value                       // calls final, then deallocates
}
```

`final` has no parameters and no return type. It is implicitly mutating.

Struct fields use `var`, `let`, or `const`. Properties omit these keywords:

```rune
struct Counter {
    var storage: Int

    value: Int {
        get { return storage }
        set(newValue) { storage = newValue }
    }

    doubled: Int => storage * 2
}
```

Auto-properties use accessor declarations without bodies:

```rune
name: String { get, private set } = "anonymous"
```

Properties may use block accessors or arrow accessors. Setters may name their
incoming value with `set(name)`. If omitted, the default setter parameter name is used.

Computed properties may appear on structs, static scopes, interfaces, and
extensions:

```rune
struct Rectangle {
    var width: Float64
    var height: Float64

    area: Float64 {
        get { return width * height }
    }

    perimeter: Float64 {
        get { return 2.0 * (width + height) }
        set(newValue) {
            let side = newValue / 4.0
            width = side
            height = side
        }
    }
}
```

Property declarations require an explicit type annotation. A property declared
with `var`, `let`, or `const` is parsed as a field declaration, not a computed
property.

Instance methods can be `mutating`. Initializers and finalizers are implicitly
mutating. Methods can be marked `open` or `override` in struct inheritance
contexts.

Non-mutating methods may read fields but cannot assign to fields or to `self`.
Mutating methods may modify fields directly or through `self`:

```rune
struct Counter {
    var value: Int

    mutating func increment() {
        value = value + 1
    }

    func get() -> Int {
        return value
    }
}
```

Struct inheritance uses `open struct`, `extends`, and `override`:

```rune
open struct Shape {
    open func area() -> Int { return 0 }
}

struct Square extends Shape {
    var side: Int

    override func area() -> Int {
        return side * side
    }
}
```

Structs are closed by default.

Default memberwise initializers are available for fields. `const` fields with
initializers are excluded from the memberwise initializer. Fields with defaults
may be omitted; fields without defaults are required:

```rune
struct Config {
    var port: Int = 8080
    var retries: Int = 3
}

let config = Config()
```

Structs and extensions can declare interface conformance:

```rune
struct User implements Hashable {
    func hash() -> Int { ... }
}
```

Struct declarations may use `@layout` to choose an explicit layout policy:

```rune
@layout(kind: c)
struct NativeHeader {
    var tag: UInt8
    var length: UInt32
}

@layout(kind: packed)
struct WireHeader {
    var tag: UInt8
    var length: UInt32
}

@layout(alignment: 64)
struct CacheLine {
    var bytes: [UInt8, 64]
}
```

The `kind` parameter may be `default`, `c`, or `packed`. `default` is Rune's
normal declaration-order layout, `c` follows the target C ABI for C-compatible
field types, and `packed` removes implicit padding between fields. The optional
`alignment` parameter sets a minimum alignment for the struct type; it must be a
compile-time power-of-two integer.

`@layout` is valid only on closed structs that do not use `extends`. Layout
policy is not inherited through struct inheritance, and open/vtable-capable
structs cannot declare explicit layout policy.

## Enums, Flags, and Unions

### Enums

Enums have a raw type and cases:

```rune
enum Status: Int {
    ready = 0
    busy = 1
    done = 2
}

let current = Status.busy
let inferred: Status = .ready
```

Case values are optional. Cases may be separated by commas or by newlines.
Implicit enum values require an integer raw type. When explicit values are used,
their expressions must be convertible to the raw type.

### Flags

Flags are enums marked with `@flags` and an integer raw type:

```rune
@flags
enum Permissions: UInt32 {
    read = 1,
    write = 2,
    execute = 4
}
```

Cases on the same line require commas. Cases on separate lines may omit commas.
Flags require an integer raw type and support bitwise operations with values of
the same flags type:

```rune
let selected = Permissions.read | Permissions.write
let canRead = (selected & Permissions.read) == Permissions.read
let withoutWrite = selected & ~Permissions.write
```

Bitwise operations between different flags types are rejected.

### Unions

Unions define cases with optional associated values:

```rune
union Command {
    step(Int, Int)
    move(dx: Int, dy: Int)
    halt
}
```

Union cases can be constructed with leading-dot syntax when the contextual type
is known:

```rune
let command: Command = .step(10, 20)
```

Pattern matching destructures associated values:

```rune
match command {
    .step(dx, dy) => return dx + dy
    .halt => return 0
}
```

Union cases may also be constructed through case constructors when explicit type
arguments are needed:

```rune
let value: Option<Int> = Option.some<Int>(42)
```

## Interfaces and Extensions

Interfaces declare requirements:

```rune
interface Printable {
    func print() -> Void
}

interface Shape {
    area: Float { get }
    func scale(factor: Float) -> Void
}
```

Interface requirements may include functions, initializers, operators, mutating
methods, and get/set property requirements. Requirement bodies are not allowed.

Interfaces are used through references. A bare interface type cannot be used as
a value type, parameter type, return type, or variable type:

```rune
interface Drawable {
    func draw() -> Int
}

struct Circle implements Drawable {
    var radius: Int

    func draw() -> Int {
        return radius * radius
    }
}

func render(shape: *Drawable) -> Int {
    return shape.draw()
}

let circle = Circle(radius: 5)
let result = render(*circle)
```

Both immutable and mutable interface references are accepted. A mutable
interface reference is required when calling a mutating interface requirement.

Extensions add members to existing types and can declare interface conformance:

```rune
extension Int {
    func squared() -> Int {
        return self * self
    }
}

extension Point implements Printable {
    func print() -> Void {
        Console.printLine("(${x}, ${y})")
    }
}
```

Extension members may be functions, operators, properties, and `const` fields.
`var` and `let` fields are rejected in extensions. Static extension methods are
allowed; static methods cannot be mutating.

An extension method cannot duplicate an existing field or method name, and two
extensions of the same type cannot introduce duplicate members. Extension
members may use `self` and `Self` the same way methods declared in the original
type can:

```rune
extension Point {
    static func zero() -> Point {
        return Point(x: 0, y: 0)
    }

    func copy() -> Self {
        return self
    }
}
```

Literal suffix extension properties use `literal` before the property declaration. The suffix is applied at the call site with an underscore between the literal and the suffix name:

```rune
distinct Meters = Float

extension Float {
    literal km: Meters => Meters(self * 1000.0)
    literal m: Meters => Meters(self)
}

let distance: Meters = 1.5_km
let height: Meters = 2.0_m
```

Literal suffixes may only be applied to numeric literals, not variables. Duplicate suffix names across extensions are rejected.

## Generics

Structs, functions, interfaces, and unions may declare type parameters:

```rune
struct Box<T> {
    var value: T
}

func identity<T>(value: T) -> T {
    return value
}
```

Type parameters may constrain with `extends` to an interface or an integer
primitive type:

```rune
struct SortedSet<T extends Comparable> {
}

struct StackAllocator<N extends UInt> {
    let storage: [Byte, N] = []
}
```

A constraint to an integer primitive (`Int`, `UInt`, `UInt8`, …) declares a
compile-time value parameter. Use sites supply a non-negative integer literal or
const name for value parameters and a type for type parameters:

```rune
let allocator = StackAllocator<256>()
```

Generic use sites provide type arguments with angle brackets:

```rune
let box = Box<Int>(value: 1)
```

## Option, Result, and Error Handling

`Option<T>` is the standard optional type:

```rune
union Option<T> {
    none
    some(T)
}
```

`nil` represents no value for an optional. Values of type `T` implicitly wrap to
`Option<T>` in a context that expects the optional.

`Result<T, E>` is the standard throwing-result type:

```rune
union Result<T, E> {
    ok(T)
    error(E)
}
```

### Throwing Functions

A function that can fail declares `throws`, optionally with an explicit error type:

```rune
union MathError {
    divisionByZero
    overflow
}

func validateAge(age: Int) throws { }

func divide(a: Int, b: Int) throws(MathError) -> Int {
    if b == 0 {
        throw .divisionByZero
    }

    return a / b
}
```

A throwing function is treated semantically as returning a result whose success type is the declared return type and whose error type is the declared error type. Returning a value implicitly wraps it as the success case.

### Throw

`throw` produces the error case. The thrown value must match the function's declared error type. It can be an enum case, a union case, a string, or any other value of the declared type:

```rune
throw .divisionByZero
throw "Input must be non-negative"
throw
```

`throw` without a value is valid when the function declares `throws` without an explicit error type.

### Try

`try?` converts a thrown error into `nil`, returning an optional:

```rune
let result = try? divide(10, 0)   // result is Int?
```

`try!` forces success and traps at runtime if the call throws:

```rune
let value = try! divide(10, 2)
```

Bare `try` propagates a throwing expression from a throwing function whose error
type matches the expression's error type:

```rune
func parse(text: String) throws(ParseError) -> Value {
    return try parseValue(text)
}
```

Bare `try` is rejected outside a matching throwing function. Use `try { } catch`
to handle the error locally, `try?` to convert failure to `nil`, or `try!` to
trap on failure.

Throwing initializers follow the same rule as throwing functions. A struct with
`init(...) throws` or `init(...) throws(Error)` must be constructed in a try
context:

```rune
struct Checked {
    var value: Int

    init(value: Int) throws(String) {
        if value < 0 {
            throw "negative"
        }
        self.value = value
    }
}

func build() throws(String) -> Checked {
    return try Checked(value: 7)
}

let forced = try! Checked(value: 7)
let optional = try? Checked(value: -1) // Checked?

try {
    let local = Checked(value: 7)
}
catch let message {
    // message is String
}
```

A throwing initializer cannot be used without `try`, `try!`, `try?`, or a
`try` block. The compiler's internal result value for a throwing initializer is
not a user-storable value.

### Try-Catch

`try { } catch { }` handles errors with catch clauses. Multiple catch clauses are evaluated in order:

```rune
try {
    let result = divide(10, b)
}
catch .divisionByZero {
    // handle specific enum case
}
catch let error {
    // bind the error value
}
catch {
    // catch-all
}
```

A `catch` clause without a pattern matches any error. A `catch .case` clause matches a specific enum or union case. `catch let name` binds the error value for use in the body. The error may be rethrown from within a catch clause in a throwing function.

## Collections and Ranges

Rune's core collection vocabulary is declared in the standard Rune module.
Important collection types include:

```text
Array<T>
Slice<T>
InlineArray<T, N>
HashMap<K, V>
HashSet<T>
String
StringSlice
```

Array and inline array values can implicitly convert to `Slice<T>` when element
types match. `Slice<T>` can be explicitly copied into `Array<T>` through
initializer syntax:

```rune
let owned = Array<Int>(slice)
```

Common collection literals infer their concrete collection type from context:

```rune
let numbers: Array<Int> = [1, 2, 3]
let ids: HashSet<Int> = [1, 2, 3, 3]
let scores: HashMap<String, Int> = ["alice": 10, "bob": 20]
let emptyScores: HashMap<String, Int> = [:]
```

Hash maps return optional values from lookup:

```rune
let alice = scores.get(key: "alice")
if alice != nil {
    return alice!
}
```

String can implicitly convert to `StringSlice`. `String` and `StringSlice` can
implicitly convert to `CString` for C bindings.

Indexing uses `[]`:

```rune
let first = array[0]
let middle = array[1..3]
let textPart: StringSlice = text[0..4]
```

Range indexing over `String` returns `StringSlice`. Range indexing over
`Array<T>`, `Slice<T>`, or `InlineArray<T, N>` returns `Slice<T>`. Indexing an
optional collection produces an optional element or optional slice result.

Ranges use `..` for half-open bounds and `..=` for inclusive bounds. Missing
lower or upper bounds are accepted in slicing contexts:

```rune
let prefix = values[..3]
let suffix = values[2..]
let inclusive = values[0..=2]
```

## References and Memory

### References

Rune has two reference types:

| Type | Meaning |
|---|---|
| `*T` | Immutable reference — allows reading but not mutation |
| `&T` | Mutable reference — allows reading and mutation |

References are created with unary `*` (immutable) and `&` (mutable), and dereferenced with postfix `^`:

```rune
var x = 1
let immutable: *Int = *x
let mutable: &Int = &x
mutable^ = 2
```

Immutable references cannot call mutating methods. Mutating methods require mutable storage or a mutable reference.

The borrow checker permits multiple active immutable borrows of the same local,
but rejects a mutable borrow while an immutable borrow is active and rejects two
simultaneous mutable borrows:

```rune
func run() {
    var value = 10
    let first: *Int = *value
    let second: *Int = *value
    let _ = first^ + second^
}
```

Taking a mutable reference to an immutable binding is rejected. Reassigning or
disposing a value while an active borrow of that value exists is also rejected.

Reference member access automatically dereferences for field reads and method
calls. Assignment through a member access requires a mutable reference:

```rune
func set(counter: &Counter) {
    counter.value = 20
}
```

### Allocation with `make`

`make` allocates and constructs a value. On success it returns a mutable
reference `&T`. Rune provides three allocation spellings that differ in how
allocation failure is handled:

| Form | Result on success | Allocation failure |
|---|---|---|
| `make` | `&T` | Traps, or is catchable inside `try` |
| `make!` | `&T` | Traps |
| `make?` | `Option<&T>` | Returns `nil` |

Plain `make` (without `!` or `?`) is allowed only in `throws` functions or
inside a `try` block. In non-throwing functions, use `make!` or `make?`:

```rune
func run() -> Int {
    let ptr = make! Box(value: 42)
    defer dispose ptr
    return ptr.value
}
```

`make?` returns an optional reference when allocation may fail:

```rune
let maybe = make? Box(value: 42)
if maybe != nil {
    defer dispose maybe!
    return maybe!.value
}
return 0
```

Inside a `try` block, plain `make` turns allocation failure into a catchable
error:

```rune
try {
    let ptr = make Box(value: 42)
    defer dispose ptr
}
catch {
    return 0
}
```

By default, `make` uses `Memory.currentAllocator`. This is the normal allocation
path:

```rune
let ptr = make! Box(value: 42)
defer dispose ptr
```

An explicit allocator conforming to the `Allocator` interface can be supplied in
parentheses when this allocation should use a different allocator instance. The
same failure modifiers apply:

```rune
var arena = ArenaAllocator(capacity: 4096)
let ptr = make!(arena) Box(value: 42)
let maybe = make?(arena) Box(value: 42)
defer dispose(arena) ptr
```

The explicit allocator can be a local variable, a global variable, a field, or
any other stable allocator lvalue. The allocator passed to `dispose` must match
the allocator used by `make`.

`make` can target structs, primitives, `String`, `Array<T>`, and other types
that support heap construction. For structs, `make` resolves initializer
arguments the same way stack construction does: named arguments are matched to
initializer parameters, not to source order. If the struct declares a custom
non-throwing initializer, `make` calls it after allocation. If the struct has no
custom initializer, field initialization uses the implicit memberwise
initializer.

Throwing initializers are not valid heap initializers. Construct a throwing
value on the stack in a `try` context, or provide a non-throwing initializer for
heap construction:

```rune
struct Box {
    var value: Int

    init(value: Int) {
        self.value = value
    }
}

let ptr = make! Box(value: 42) // calls Box.init(value:)
```

### Deallocation with `dispose`

`dispose` destroys and releases a value previously allocated with `make`. For a
struct that declares `final`, `dispose` calls `final` before deallocating the
storage. The operand must be a reference or an optional reference:

```rune
dispose ptr
dispose(allocator) ptr
dispose maybePtr
dispose maybePtr!
```

The allocator passed to `dispose` must match the one used for `make`.

`defer dispose` is the idiomatic pattern for scope-bound lifetimes:

```rune
let ptr = make! Box(value: 42)
defer dispose ptr
```

### Allocator

`Memory.currentAllocator` is the ambient allocator used by bare `make` and bare
`dispose`. It defaults to `Memory.defaultAllocator` and can be replaced when a
scope wants a different default:

```rune
let previous = Memory.currentAllocator
Memory.currentAllocator = arena

let ptr = make! Box(value: 42)  // uses arena through Memory.currentAllocator
dispose ptr                    // disposes through the same current allocator
Memory.currentAllocator = previous
```

Passing an allocator explicitly is for individual allocation sites that should
not use the ambient allocator:

```rune
let ptr = make!(arena) Box(value: 42)
defer dispose(arena) ptr
```

## Modules and Imports

Rune has four import forms:

| Form | Effect |
|---|---|
| `import Module` | Exposes the module namespace for qualified access (`Module.symbol`). |
| `import Module.*` | Brings the module's public exports into unqualified scope. |
| `import Module.Static` | Exposes `Module.Static` for qualified access. |
| `import Module.Static.*` | Brings the static scope's exports into unqualified scope. |

`import Module` makes the module accessible via its qualified name:

```rune
import Standard

let x = Standard.Math.sqrt(2.0)
Standard.Console.printLine("ok")
```

`import Module.*` brings the module's exports into unqualified scope:

```rune
import Standard.*

let x = Math.sqrt(2.0)
Console.printLine("ok")
```

`import Module.Static` and `import Module.Static.*` target a specific static scope:

```rune
import Standard.Math.*

let x = sqrt(2.0)   // Math.sqrt brought into scope
```

Only public exports are visible across modules. Private and internal top-level
declarations are hidden from both unqualified and qualified access.

Two wildcard imports that bring the same name into scope is a compile error.

Modules are described by `.runemanifest` files and compiled into
`.runemodule` files.

## C Bindings

Rune functions can be bound to C symbols using three attributes:

- `@link(...)` — the native library to link against
- `@include(...)` — the C header that declares the symbol
- `@symbol(...)` — the exact C symbol name to bind

The function body is omitted; Rune calls the C symbol directly:

```rune
@link("c")
@include("string.h")
@symbol("strlen")
func stringLength(value: CString) -> UInt64
```

Multiple bindings can be grouped in a braced block to share `@link` and `@include`:

```rune
@link("c")
@include("math.h")
{
    @symbol("sqrt")
    func sqrt64(value: Float64) -> Float64

    @symbol("fabs")
    func abs64(value: Float64) -> Float64
}
```

Struct types used in an `@symbol` parameter or return type must declare
`@layout(kind: c)`, including nullable pointer/reference forms such as
`*Native?` and `&Native?`. This makes C ABI layout a checked source contract
instead of an implicit assumption.

### Type Mappings

| Rune Type | C Type |
|---|---|
| `Int8` | `int8_t` |
| `Int16` | `int16_t` |
| `Int32` | `int32_t` |
| `Int64` | `int64_t` |
| `UInt8` | `uint8_t` |
| `UInt16` | `uint16_t` |
| `UInt32` | `uint32_t` |
| `UInt64` | `uint64_t` |
| `Int` | `int64_t` / `ptrdiff_t` (platform native) |
| `UInt` | `uint64_t` / `size_t` (platform native) |
| `Float32` / `Float` | `float` |
| `Float64` / `Double` | `double` |
| `Bool` | `bool` |
| `CString` | `const char*` |
| `&Byte` | writable `void*` |
| `*Byte` | opaque or read-only `void*` |

Opaque C types are declared with `distinct` types without a base type:

```rune
distinct FILE
distinct DIR
```

## Async and Await

Rune provides **Swift-like structured concurrency** with `async` and `await`.
Async functions can suspend at `await` points and resume cooperatively on the
current thread. This is distinct from `Standard.Thread`, which spawns **OS
threads**.

### Declaring async functions

Mark a function with the `async` modifier after its parameter list. It may be
combined with `throws`:

```rune
func fetch() async -> Int {
    return 42
}

func load() async throws -> Data {
    let bytes = await fetch()
    return bytes
}
```

### Awaiting results

Use `await` to call another async function and suspend until its result is
ready. `await` is only permitted inside async functions:

```rune
func work() async -> Int {
    let value = await fetch()
    return value + 1
}
```

Calling an async function without `await`, or calling one from a synchronous
function, is a compile-time error.

### Entry points

`main` may be async and return `Int` or `Void`:

```rune
func main() async -> Int {
    return await work()
}
```

The runtime drives async entry functions to completion on the **current OS
thread** using a cooperative task scheduler. Each `await` spawns the callee as
an in-flight task and suspends the caller until that task finishes; other ready
tasks may run at `await` and `Task.yield()` points.

### Cooperative tasks (`Standard.Task`)

`Standard.Task` mirrors Swift's unstructured `Task` on the **current thread**
(distinct from `Standard.Thread`, which spawns OS threads):

| Rune | Swift analogue |
|------|----------------|
| `Task.spawn(body:)` | `Task { await body() }` |
| `await Task.value(handle)` | `await task.value` |
| `Task.yield()` | `Task.yield()` |

`spawn` takes a `() async -> Void` function reference and returns a `TaskHandle`.
Use `await Task.value(handle)` to suspend until that child task finishes.

```rune
import Standard.*

func worker() async -> Void {
    Task.yield()
}

func main() async -> Int {
    let handle = Task.spawn(body: worker)
    await Task.value(handle)
    return 0
}
```

Prefer plain `async`/`await` when calls can run sequentially; use `Task` when
several async functions should stay in flight and interleave at suspension points.

## Threading

Rune uses **real OS threads** (pthread / Win32). The interpreter gives each OS
thread its own call stack while **module globals are shared** across threads in
the same process.

### Standard library

`Standard.Thread` wraps platform threads:

```rune
import Standard

func worker() {
    // runs on a new OS thread
}

func main() -> Int {
    let thread = Thread(body: worker)
    thread.join()
    return 0
}
```

`Thread(body:)` starts a joinable thread running Rune bytecode. `join()` waits
for completion; `detach()` releases the handle without waiting.

Use `Standard.Mutex` (and future atomics) to synchronize access to shared module
`var` globals or other data reachable from multiple threads.

### FFI callbacks

Foreign libraries may invoke Rune callbacks on **their** threads (for example SDL
audio). The VM resolves the **current OS thread** to a private interpreter
context that shares module globals with the rest of the process. Register
callbacks as today; they remain valid when invoked from native worker threads.

### Rules

| Rule | Detail |
|---|---|
| One active bytecode executor per OS thread | Do not call `execute()` on the same VM context from two threads at once |
| Shared globals | Module-level `var` is process-wide; protect with `Mutex` when mutated concurrently |
| References | Do not share `&T` across threads unless guarded by a lock (full `Send`/`Sync` typing is planned) |
| Allocators | `Memory.currentAllocator` is thread-local |

## Entry Points

An executable program can define a parameterless `main` function returning `Int`
or `Void`:

```rune
func main() -> Int {
    return 0
}

func main() {
    Console.printLine("Hello")
}
```

`main` parameters are rejected. A `Void` `main` runs successfully without an
exit code value; an `Int` `main` returns its value as the executable result.

Top-level executable statements are also valid. Code written outside any
declaration runs as an implicit entry point:

```rune
Console.printLine("Hello")
```

Top-level executable statements and a `main` function cannot be combined in the
same program. Top-level declarations such as `const`, `let`, `var`, `func`,
`struct`, `interface`, and `extension` declarations may still appear in a file
with `main` as long as there is no executable top-level statement.

## Built-in Macros

Built-in macros are compiler-provided operations prefixed with `#`. The `#` prefix visually distinguishes them from regular function calls and signals that the operation may have special compile-time semantics.

### Layout macros

These macros return memory layout information for a type at compile time. They accept a single type argument and return `UInt`.

```rune
let size      = #sizeOf<Int32>()
let alignment = #alignmentOf<Int32>()
let stride    = #strideOf<Int32>()
```

### Naming macros

These macros return the name of a type or static member as a `String`.

```rune
let name  = #nameOf<Int32>()             // "Int32"
let qname = #qualifiedNameOf<Int32>()    // "Rune.Int32"
let cname = #nameOf(Direction.north)     // "north"
```

`#nameOf` and `#qualifiedNameOf` accept either a type argument or a static member reference (enum/flags case). Instance variable references are rejected.

### Assertion macros

```rune
#assert(condition)
#assert(condition, "message")

#debugAssert(condition)          // stripped in release builds
#debugAssert(condition, "message")

#panic()                         // unconditional abort
#panic("message")
```

`#assert` aborts the program when the condition is `false`. `#debugAssert` is a no-op in release builds. `#panic` always aborts. All three may only be used as statements.

### Print macros

```rune
#print("value=", x)
#printLn("value=", x)
```

`#print` writes its arguments to standard output without a trailing newline. `#printLn` appends a newline. Both accept zero or more arguments that conform to `CustomStringConvertible`, plus string and numeric literals. Both may only be used as statements.
