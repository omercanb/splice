# Value semantics

Every name in Python is a reference and if a program compiled from Python would like to preserve reference semantics, they would have to wrap every value in a pointer. This leads to lots of memory allocations, prevents having a flat memory layout, and makes it very expensive to interface between Python and C++. 

## Goal
Be able to represent all user defined data as flat value types in C++.
Don't use any refcounting or memory allocation for data.
Be able to interface directly between a value in Python and a value in C++.

## Solution
Ban all forms of aliasing in Python. Concretely, Splice has static checks that makes sure reference semantics can never be observed and nothing can alias eachother. This enables all of the above conditions, most importantly, flat value types.

### An example of issues with references
```python
class Entry:
    def __init__(self, items: list[int]):
        self.items = items

def main():
    nums = [1, 2, 3]
    entry = Entry(nums)
    nums.append(4)
```
To support these exact semantics in C++, you would need `Entry.items` to be a pointer, then you'd have to use refcounting or garbage collection for it. So we don't allow this type of code to be written. Instead, the compiler raises an error saying `self.items = items` needs to be replaced with `self.items = copy(items)`. In general the solution is to explicitly require a copy, at any point that will normally create a reference. 
The result of this is that all used data types, 

It's very difficult to use C++ value types here and also support these exact semantics. 
If `arr` uses a flat layout like `std::vector`, the first append can reallocate, then the parameter `num` becomes undefined behavior, because `arr[0]` no longer points to anything. So to have the benefit of using a flat layout while also ensuring that the semantics of Python stay the same, we must prevent aliasing and use `extend(arr, copy(arr[0]))`.

## The Benefits


## The two obvious choices, and why both are wrong

**Option one: make every object a `shared_ptr` (or similar) under the hood**, so assignment can keep meaning "point at the same thing," exactly like Python. This is correct — it reproduces Python's aliasing behavior perfectly — but it throws away the entire reason you wanted C++ in the first place. Every object is now a separate heap allocation. Every copy is an atomic refcount bump. Every field access is a pointer chase through memory the cache doesn't have. You've built a C++ program shaped like a Python interpreter's object model, not a fast C++ program.

**Option two: make everything a plain value**, stack-allocated, embedded directly in its container, no indirection. This is fast — it's how you'd actually write performance-sensitive C++ by hand. But now `a = b` *copies*, silently, which is a different operation than what the Python source says. If a program actually depended on that assignment aliasing — mutating through one name and reading it back through the other — the compiled program does something quietly different from the interpreted one. That's not a slow program, it's a *wrong* one, and it's wrong in a way that's invisible at the call site.

Splice takes option two, but refuses to leave the gap silent. Every spot where a plain-value translation could diverge from Python's real behavior is caught at compile time and forced to be explicit.

## A concrete example

```python
class Order:
    def __init__(self) -> None:
        self.fills: list[int] = []

def process() -> None:
    pending = [10, 20]
    order = Order()
    order.fills = pending
    pending.append(30)
    print(order.fills)
```

In real Python, `order.fills = pending` doesn't copy the list — it makes `order.fills` and `pending` two names for the *same* list object. So `pending.append(30)` is visible through `order.fills` too, and this prints `[10, 20, 30]`.

If this got compiled straight to C++ with `fills` as a plain `std::vector<int>` member, `order.fills = pending` would compile to a real copy. `pending.append(30)` would only touch `pending`'s own storage. The program would print `[10, 20]` instead — silently wrong, and nothing about reading the Python source would tell you that.

Splice doesn't guess which behavior you meant. It rejects `order.fills = pending` outright:

```
error: `pending` needs an explicit copy here

  8 |     order.fills = pending
    |     ^^^^^^^^^^^^^^^^^^^^^

  help: wrap it in copy():
          copy(pending)
```

Note what this error is *not* doing: it isn't offering you a way to get the real Python aliasing behavior back. Splice doesn't support that at all — there's no pointer/reference type you can reach for here. `copy()` is the only path forward, and it gives you the independent-copy behavior: after `order.fills = copy(pending)`, appending to `pending` no longer touches `order.fills`, on purpose, because that's the only relationship a plain-value field can actually have with something assigned into it. The error exists so that decision is something you make on purpose, at the one line where it matters, instead of something the compiler makes for you silently three hundred lines away.

This is the same rule everywhere a name gets handed off — assigned into a field, assigned into a fresh local, or returned. A fresh value with no other name (`[1, 2, 3]`, a function's return value, `a.method()`) never needs `copy()`, because nothing else could possibly have a reference to it to alias in the first place — the rule only fires on names that already exist and might still be read elsewhere.

## What this buys you

Because the compiler *proves* two mutable things can never alias — rather than hoping the programmer got it right, or paying for a runtime mechanism (refcounting, GC) to make it safe regardless — the generated code can do what hand-written fast C++ does: keep objects inline, pass by reference without fear during a call, mutate in place, and never touch the allocator except where you actually asked for a new container. The check costs you something real: some Python programs that lean on assignment-aliasing on purpose won't compile as-is, and need an explicit `copy()` (or a restructure) to say what they meant. In exchange, every program that *does* compile gets C++'s performance characteristics for free, and the one class of bug that plain-value translation would otherwise introduce silently — an aliasing assumption the source no longer honors — can't happen at all.

For the full rule set (function arguments, loops, globals, lambdas) and the analysis that enforces it, see [`docs/design/value_semantics_plan.md`](design/value_semantics_plan.md).
