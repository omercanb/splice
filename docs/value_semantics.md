# Value semantics design

**Purpose: Stay close to Python's semantics but provide native C++ performance by limiting the programs that you can write**

Main Goal: Prevent aliasing.
```python
def f(a: list[int], b: list[int]):
    a.append(1)
    print(b[0])
a = [1, 2, 3]
b = a
f(a, b)
```
Then if `a.append()` causes a to reallocate, `b[0]` will be pointing to garbage.
By preventing any way in which this sort of aliasing can happen, we can use C++ like values in classes instead of Python like pointers. This enables us to have directly C++ like performance. 

## Core Rules
**Note: These are all checked by the Splice compiler**
- Whenever you assign or return an lvalue (`a`, `a.field`, `a.items[i]`, etc, but not a literal `0`, `function()`, or `a.method()`) you have to assign or return it inside `copy()`. So `return copy(a)` or `b = copy(a.field)`. This return or reassign is called a 'hand-off'.
- When passing in arguments to a function, they cannot be structural aliases. 
    Examples of banned function calls: 
    - `function(self, self.items)` is not allowed because `self.items` contains `self` 
    - `function(array, array[i])` is not allowed because `array[i]` contains `array`
    - `function(array[i], array[j])` is not allowed because `i` and `j` could be the same number (but `function(array[0], array[1]` is allowed and its also allowed if `i` and `j` are known at compile time)
    All other calls are allowed.
- Global variables can only be `int`s or `float`s and cannot be reassigned.
- Function parameters cannot be reassigned. 
- Classes cannot contain themselves as members.

## Core model

- All types are **value types** by default. Assignment always conceptually copies; there is no first-class reference type in ordinary code.
- **`copy()` is a free function**, not a method — mandatory, explicit, at every genuine hand-off of an **lvalue** (a name that persists and could be read again: a variable, `a.b`, `container[i]`).
- **Hand-off = assignment or return**, specifically. `x = source` and `return source` require `copy()` if `source` is an lvalue. Passing an lvalue as a *function argument* does **not** require `copy()` — arguments are passed as mutable references, scoped to the call, never copies.
- **Prvalues** (fresh values with no other name: a literal, a call's return value, a constructor, anything chained directly off another prvalue) never need `copy()`, anywhere. Nothing else could ever alias them, so the compiler just constructs/moves them directly.
- **Moves are never written by the user.** The compiler elides a `copy()` into a move whenever it can prove the source is dead afterward. (Deferred for v1 — see below.)

## Function parameters

- **No visible annotation.** For now, every parameter is passed as a mutable reference in the generated C++ — no inference of read-only vs. mutable yet (that's a future refinement, not part of the current plan).
- Parameters are references **scoped strictly to the call** — never copies. This matches Python's own semantics (mutating a passed mutable object is visible to the caller) at zero cost.
- A parameter reference can **never be named** (bound to a variable), **never returned bare**, and **never passed onward** to another function's parameter unless it passes the exclusivity check below. This is what keeps it from ever outliving the call it belongs to.
- A **prvalue argument** is just constructed/moved directly into the parameter's storage — no reference machinery needed, since there's nothing to alias. Falls out of the lvalue/prvalue distinction for free; no separate rule.
- **Reassigning a parameter's own name (`x = new_value`) is not allowed.** In Python this only rebinds the local name — the caller's object is untouched. Since a parameter compiles to a C++ reference, the same line there would assign *through* the reference and mutate the caller's object — the opposite of what the Python source means. Rather than detect or reconcile that mismatch, the rebind is simply rejected. Mutating *through* the parameter (`x.field = 5`) is unaffected and remains the correct, intended way to mutate the caller's object.

## The exclusivity check (the core safety mechanism)

- **Purely structural and syntactic. No dataflow analysis, no call-graph analysis, anywhere.**
- At every call: compare all arguments — including the implicit `self` on a method — pairwise. Reject the call if any two share a structural base that isn't *provably* distinct.
- `a.items[i]` alone as an argument is completely fine.
- `a.items[i]` and `a.items[j]` as two arguments in the *same* call is only fine if `i` and `j` are provably distinct — both sides must be compile-time constants (literals or `constexpr` globals) **and** unequal. A variable or a function-call index is conservatively assumed to possibly overlap, and the call is rejected.
- Field access (`a.b.c`) is always a fixed, distinguishable path — never confused with a different field path.
- This composes across nested/recursive calls by induction: every call site is checked, so a function's own parameters are guaranteed non-aliased on entry, and it never has to re-derive that guarantee when passing them onward.

## Globals

- **Only ints and floats are allowed at module scope.** Nothing else — no containers, no mutable structs, no shared state of any kind as a bare global.
- Everything else must live inside a class instance, threaded explicitly via `self`/parameters — never reached by bare name. This is what removes the need for any interprocedural "which globals does this function touch" analysis; every check stays purely local to one call site.

## Explicitly not allowed

- Bare `b = a` (no `copy()`) for any lvalue of non-trivial type.
- A reference bound to a variable, returned bare, or forwarded to another parameter without passing the exclusivity check.
- Two arguments (`self` included) to one call sharing a structural base, unless provably disjoint.
- Any non-int/float mutable state at module scope.
- Reassigning a parameter's own name inside the function body.

## Genuinely won't be implemented (not open, not deferred — out of scope)

- **Recursive/self-referential types** (e.g. a `Node` containing itself). No indirection mechanism exists in this design, so these have unbounded size and are rejected outright.
- **Closures/lambdas capturing outer mutable state.**
- **Generics/templates.**

## Deferred for v1 (safe to defer — costs precision, never soundness)

- **Last-use analysis / move elision.** Ship with none at first — every `copy()` is a real copy, always correct regardless of how much gets elided later.
- **Read-only vs. mutable parameter distinction.** Everything is a mutable reference for now; narrowing this later only removes some spurious rejections, never changes what's accepted.

## Python-side requirement

- `copy()` needs a real interpreted implementation (`copy.deepcopy`) so the same source stays runnable directly in CPython for research/backtesting, not just when compiled.
- The exclusivity check and last-use analysis need nothing on the Python side — they're proofs about the compiled output only.
