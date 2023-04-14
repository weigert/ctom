# ctom

c++20 compile-time object model

fully static strict typing for compile-time serialization

`Note: This is similar to a strict-typed, compile-time boost property tree.`

**THIS REPOSITORY IS A WORK IN PROGRESS**

## overview

`ctom` is a single-header library which allows you to define compile-time object models, statically analze their structure, and use this to emit and parse various serialization formats.

`ctom` supports **value-types**, **sequence-types** and **object-types**.

Instead of iterating over `json` or `yaml` node trees, you can statically declare your object-model to directly emit from or parse into a **standard c++ struct or class**.

`ctom` allows for separation between object-model declarations and implementations, as well as the extension of existing models.

It works by using modern c++ techniques including *concepts*, *template meta-programming* and *class-template argument deduction* to provide a **declarative** api. Originally inspired by `golang semantic tags`.

```c++
struct Foo: ctom::obj<
    ctom::key::val<"my-int", int>           // declare object-model
>{
    int& some_int = Foo::val<"my-int">(2);  // type-safe key tie-to ref member (moved)
};

int main(){
    Foo foo;                                        // create-instance 
    std::cout<<ctom::yaml::emit<<foo;               // emit yaml to stream

    foo.some_int = 1;                               // change member via struct ref
    std::cout<<ctom::yaml::emit<<foo;               // emit yaml to stream

    foo.get<"my-int">() = 3;                        // change member via ctom::key
    std::cout<<ctom::yaml::emit<<foo;               // emit yaml to stream
    std::cout<<"some_int = "<<foo.some_int<<"\n";   // output the struct member 
}
```

```yaml
"my-int": 2
"my-int": 1
"my-int": 3
some_int = 3
```

## utilization

### Compile-Time Object-Model Declarations

`Note: All code in this section taken from examples/0_def`

Note that the following examples are **fully-static**, showing only the declarations. They are separated from the implementation for simplicity. For implementations and tying to actual values, as well as serialization, see further below.

##### Simple Object

```c++
struct Foo: ctom::obj<
    ctom::key::val<"foo-int", int>,
    ctom::key::val<"foo-float", float>,
    ctom::key::val<"foo-double", double>
>{};

ctom::print<Foo>(); // NOTE: FULLY STATIC! NO INSTANCE!
```

```bash
val: [foo-int]
val: [foo-float]
val: [foo-double]
```

##### Simple Array

```c++
struct Barr: ctom::arr<
    ctom::ind::val<0, int>,
    ctom::ind::val<1, int>,
    ctom::ind::val<2, int>,
    ctom::ind::val<3, int>
>{};

ctom::print<Barr>();
```

```bash
val: [0]
val: [1]
val: [2]
val: [3]
```

##### Nested Objects/Array

```c++
struct Bar: ctom::obj<
    ctom::key::obj<"bar-foo", Foo>,
    ctom::key::val<"bar-char", char>,
    ctom::key::arr<"bar-barr", Barr>
>{};

struct Baz: ctom::obj<
    ctom::key::obj<"baz-bar", Bar>,
    ctom::key::val<"baz-bool", bool>
>{};

ctom::print<Baz>();
```

```bash
obj: [baz-bar]
  obj: [bar-foo]
    val: [foo-int]
    val: [foo-float]
    val: [foo-double]
  val: [bar-char]
  arr: [bar-barr]
    val: [0]
    val: [1]
    val: [2]
    val: [3]
val: [baz-bool]
```

##### Nested Arrays/Objects

```c++
struct Maz: ctom::obj<
    ctom::key::val<"maz-char", char>
>{};

struct Marr: ctom::arr<
    ctom::ind::obj<0, Maz>,
    ctom::ind::obj<1, Maz>,
    ctom::ind::obj<2, Maz>
>{};

struct MarrArr: ctom::arr<
    ctom::ind::arr<0, Marr>,
    ctom::ind::arr<1, Marr>
>{};

ctom::print<MarrArr>();
```
```bash
arr: [0]
  obj: [0]
    val: [maz-char]
  obj: [1]
    val: [maz-char]
  obj: [2]
    val: [maz-char]
arr: [1]
  obj: [0]
    val: [maz-char]
  obj: [1]
    val: [maz-char]
  obj: [2]
    val: [maz-char]
```

##### Extended Object

```c++
struct FooExt: Foo::ext<
    ctom::key::val<"foo-ext-int", int>,
    ctom::key::obj<"foo-ext-foo", Foo>
>{};

ctom::print<FooExt>();
```

```bash
val: [foo-int]
val: [foo-float]
val: [foo-double]
val: [foo-ext-int]
obj: [foo-ext-foo]
  val: [foo-int]
  val: [foo-float]
  val: [foo-double]
```

##### Extended Array

```c++
struct MarrExt: Marr::ext<
    ctom::ind::obj<3, Maz>,
    ctom::ind::obj<4, Maz>,
    ctom::ind::obj<5, Maz>
>{};

ctom::print<MarrExt>();
```

```bash
obj: [0]
  val: [maz-char]
obj: [1]
  val: [maz-char]
obj: [2]
  val: [maz-char]
obj: [3]
  val: [maz-char]
obj: [4]
  val: [maz-char]
obj: [5]
  val: [maz-char]
```

### Run-Time Object-Model Implementations

`Note: All code in this section taken from examples/1_impl`

To implement a compile-time object-model declaration, 

You can also combine the declaration and the implementation directly.

### Stream-Serialization

Note: The

##### yaml

##### json

## Details

### todo

- better interface out of the ctom interface for defining our object-models, particularly array-types
- the various serialization formats / namespace should implement static iterators to validate the object-models (e.g.: valid json keys, valid yaml keys)
- swap some concepts with static assertions, for more helpful error messages
- moving and assignment into array and stl containers would be very convenient and ease development using `ctom`.

### other thoughts

There are a number of features which `c++20` doesn't have, which make a more elegant interface difficult. For instance, there are no static subscript operators, which mean that I can't properly index without using user-defined literals. But then to index with a number (e.g. `size_t`), this is not possible at all because of the way number-literals are (arbitrarily) restrained. What a mess.

Therefore, we **have** to use static `get<>` member functions.