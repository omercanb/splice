# Value semantics

Every name in Python is a reference and if a program compiled from Python would like to preserve reference semantics, they would have to wrap every value in a pointer. This leads to lots of memory allocations, prevents having a flat memory layout, and makes it very expensive to interface between Python and C++. Splice is designed around making it impossible to observe reference semantics in Python and adds some real constraints onto Python to be able to do this. But as a result it can compile everything into C++ value types.

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

## Benefits
- Python can be compiled to C++ without using any pointers and without refcounting or garbage collection.
- Passing a value between Python and C++ doesn't require any code to convert between different representations, so a value passed from Python can be written directly to a buffer.
- Every function parameter can be marked `restrict` for more C++ compiler optimization potential.

## Model
- When passing an object to a function you pass in a reference and functions can mutate their arguments but a function can never return a reference.
- When assigning a value into an object field, assigning an object field into a variable, or returning one of the parameters from a function you have to `copy` to prevent aliasing.

## Constraints
- Globals can only be scalars and are immutable. Globals only being scalar prevents aliasing, otherwise we would have to analyze what globals each function accesses and ban an alias to that global from being passed in as an argument (This can be implemented, but I just didn't think there would be too much benefit).
- You can't pass in multiple locations that could alias each other to functions. For example you can't pass in `f(obj, obj.field)`. The compiler requires you to call copy on one. Again this is to prevent aliasing.
