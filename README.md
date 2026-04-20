## Simple Runtime System in C

This is a dumb little project I made which may get mildly expanded on, but mostly exists as a tutorial on how to implement dynamic typing in C.

## What This Contains:

- [x] 0% AI Generated Bullshit. Every crash is of my own volition
- [x] Runtime type system (union-based TypedValue)
- [x] Option type (some presenting a boolean, which if true can be unwrapped)
- [x] Typed, resizeable, heap dynamic arrays
- [x] Safe API for all operations (option types, dynamic arrays, and value creation)
- [x] Ownership semantics and recursive freeing for pointer types
- [x] Array indexing
- [x] Array slicing (mistakenly committed as string slicing)
- [x] Safe Iterables (can be iterated without causing UB)
- Safe casting between types [70%]
- Runtime error system [70%]

## What I Can Think of Adding:

- Allow Options to be instantiated as Values (this can be recursive, an Option can unwrap to another Option)
- Proper String type (with length and capacity)
- Safe getters for union values (checking `value.type` before extraction)
- Other types of fat pointer
- Custom heap & API (malloc, realloc, free, ref/deref and others)
- Hash maps / Dictionaries

## Known Pitfalls:

- Slicing is not fully safe yet. It can point to deinitialized data, and uninitialized data can be in the slice range (which is mostly prevented by the safety of array insertions).
- The generic AS_VAL does not encompass char pointers because of a missize. Use as_str("data") instead.

## Why Did I Make This?

Quite honestly, this semester has burnt out all desire of mine to code. I have floated switching majors at least once. However, I realize this is what I see myself doing, and I hope to find my love for coding back with some slow, small projects.

An increasing amount of my projects have used a bit TOO much LLM usage (an epiphany I found out while learning C myself) and I realize that while I understand exactly what the code written in front of me can do, a lot of it I probably could not write without a base created by an LLM. I don't know whether to call that incompetence or "the new developer mindset" but I don't feel like I want to keep racing to ship new projects I don't understand or am not completely proud of.

So, I decided to sit down for a couple hours and write something I actually enjoy doing. Hopefully more small, fun projects will come in the future, but I do mainly plan to keep working on the language as I just haven't had enough time nor motivation right now.

---

<div align="center">
    <sub style="margin:0">Made with 💓 by <a href="https://github.com/gamerjamer43">gamerjamer43</a></sub>
</div>