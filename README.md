# ctom

compile-time object model

fully static strict typing for compile-time yaml / json parsing

Similar to a strict-typed, compile-time boost property tree.

## todo
- static check keys for valid strings
- implement aliases for easy array construction

Nice to Have:
- Better Indexing with Literals Somehow!
- Different Format Marshalling
- Format Unmarshalling

this namespace lets you define a compile-time object model using template
meta-programming techniques. The object-model allows for fast (un)marshalling
of various data formats, and is similar to semantic tags in golang.

the original intent was a type-safe compile-time yaml parser.
