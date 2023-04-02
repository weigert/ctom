# ctom

compile-time object model

fully static strict typing for compile-time yaml / json parsing

Similar to a strict-typed, compile-time boost property tree.

## todo
- static check keys for valid strings
- potentially unify refs and inds using templates
- implement aliases for esay array construction

this namespace lets you define a compile-time object model using template
meta-programming techniques. The object-model allows for fast (un)marshalling
of various data formats, and is similar to semantic tags in golang.

the original intent was a type-safe compile-time yaml parser.
