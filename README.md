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

##### Nested Object/Array

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

##### Nested Array/Object

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

You can also combine the declaration and the implementation directly.

##### Simple Object

```c++
struct Foo_Impl: Foo {
  int& x    = Foo::val<"foo-int">(1);
  float& y  = Foo::val<"foo-float">(0.5f);
//  double& z   = Foo::val<"foo-double">(0.25);
} foo_impl;

ctom::print(foo_impl);

foo_impl.get<"foo-int">() = 2;        // get-based assignment
foo_impl.get<"foo-float">() = 0.25f;

ctom::print(foo_impl);
```

```bash
val: "foo-int" = 1
val: "foo-float" = 0.5
val: "foo-double" = 
val: "foo-int" = 2
val: "foo-float" = 0.25
val: "foo-double" = 
```

##### Simple Array

```c++
struct Barr_Impl: Barr {
  int& a = Barr::val<0>(0);
  int& b = Barr::val<1>(1);
  int& c = Barr::val<2>(2);
  int& d = Barr::val<3>(3);
} barr_impl;

ctom::print(barr_impl);

barr_impl.a = 3;  // direct assignment
barr_impl.b = 2;
barr_impl.c = 1;
barr_impl.d = 0;

Barr_Impl new_barr_impl;    // instances are properly separated!

ctom::print(barr_impl);
ctom::print(new_barr_impl);
```

```bash
val: [0] = 0
val: [1] = 1
val: [2] = 2
val: [3] = 3
val: [0] = 3
val: [1] = 2
val: [2] = 1
val: [3] = 0
val: [0] = 0
val: [1] = 1
val: [2] = 2
val: [3] = 3
```

##### Nested Object/Array

```c++
struct Bar_Impl: Bar {
  Foo_Impl& foo     = Bar::obj<"bar-foo", Foo_Impl>();
  char& c           = Bar::val<"bar-char">('x');
  Barr_Impl& barr   = Bar::arr<"bar-barr", Barr_Impl>();
};

struct Baz_Impl: Baz {
  Bar_Impl& bar_impl  = Baz::obj<"baz-bar", Bar_Impl>();
  bool& b             = Baz::val<"baz-bool">(true);
} baz_impl;

ctom::print(baz_impl);
```

```c++
obj: "baz-bar" = 
  obj: "bar-foo" = 
    val: "foo-int" = 1
    val: "foo-float" = 0.5
    val: "foo-double" = 
  val: "bar-char" = x
  arr: "bar-barr" = [
    val: [0] = 0
    val: [1] = 1
    val: [2] = 2
    val: [3] = 3
  ]
val: "baz-bool" = 1
```

##### Nested Array/Object

```c++
struct Maz_Impl: Maz {
  char& c   = Maz::val<"maz-char">(' ');
};

struct Marr_Impl: Marr {
  Maz_Impl& maz0  = Marr::obj<0, Maz_Impl>();
  Maz_Impl& maz1  = Marr::obj<1, Maz_Impl>();
  Maz_Impl& maz2  = Marr::obj<2, Maz_Impl>();
};

struct MarrArr_Impl: MarrArr {
  Marr_Impl& marr0  = MarrArr::arr<0, Marr_Impl>();
  Marr_Impl& marr1  = MarrArr::arr<1, Marr_Impl>(); 
} marrarr_impl;

marrarr_impl.marr0.maz0.c = 'a';
marrarr_impl.marr0.maz1.c = 'b';
marrarr_impl.marr0.maz2.c = 'c';
marrarr_impl.marr1.maz0.c = 'd';
marrarr_impl.marr1.maz1.c = 'e';
marrarr_impl.marr1.maz2.c = 'f';

ctom::print(marrarr_impl);

marrarr_impl.get<0>().get<0>().get<"maz-char">() = 'g';
marrarr_impl.get<0>().get<1>().get<"maz-char">() = 'h';
marrarr_impl.get<0>().get<2>().get<"maz-char">() = 'i';
marrarr_impl.get<1>().get<0>().get<"maz-char">() = 'j';
marrarr_impl.get<1>().get<1>().get<"maz-char">() = 'k';
marrarr_impl.get<1>().get<2>().get<"maz-char">() = 'l';

ctom::print(marrarr_impl);
```

```bash
arr: [0] = [
  obj: [0] = 
    val: "maz-char" = a
  obj: [1] = 
    val: "maz-char" = b
  obj: [2] = 
    val: "maz-char" = c
]
arr: [1] = [
  obj: [0] = 
    val: "maz-char" = d
  obj: [1] = 
    val: "maz-char" = e
  obj: [2] = 
    val: "maz-char" = f
]
arr: [0] = [
  obj: [0] = 
    val: "maz-char" = g
  obj: [1] = 
    val: "maz-char" = h
  obj: [2] = 
    val: "maz-char" = i
]
arr: [1] = [
  obj: [0] = 
    val: "maz-char" = j
  obj: [1] = 
    val: "maz-char" = k
  obj: [2] = 
    val: "maz-char" = l
]
```

### Stream-Serialization

Different serialization formats have stream-modifiers, which allow you to pass an object-model instance directly to the stream.

#### yaml

```c++
std::cout << ctom::yaml::emit << foo_impl;
```

```yaml
"foo-int": 1
"foo-float": 0.5
"foo-double": null
```

`ctom::yaml::parse`: **WIP**

#### json

**WIP**

## Details

### todo

- better interface out of the ctom interface for defining our object-models, particularly array-types
- the various serialization formats / namespace should implement static iterators to validate the object-models (e.g.: valid json keys, valid yaml keys)
- swap some concepts with static assertions, for more helpful error messages
- moving and assignment into array and stl containers would be very convenient and ease development using `ctom`.

### other thoughts

There are a number of features which `c++20` doesn't have, which make a more elegant interface difficult. For instance, there are no static subscript operators, which mean that I can't properly index without using user-defined literals. But then to index with a number (e.g. `size_t`), this is not possible at all because of the way number-literals are (arbitrarily) restrained. What a mess.

Therefore, we **have** to use static `get<>` member functions.