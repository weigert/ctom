# ctom

c++20 compile-time object model

fully static strict typing for compile-time serialization

stop iterating over parse-trees - statically emit from and parse into c++ variables, arrays and structs directly

`Note: This is similar to a strict-typed, compile-time boost property tree.`

**THIS REPOSITORY IS A WORK IN PROGRESS**

## overview

`ctom` is a single-header library which allows you to define compile-time object models, statically analze their structure, and use this to emit and parse various serialization formats.

`ctom` supports **value-types**, **sequence-types** and **object-types**.

Instead of iterating over `json` or `yaml` node trees, you can statically declare your object-model to directly emit from or parse into a **standard c++ struct or class**.

`ctom` allows for separation between object-model declarations and implementations, as well as the extension of existing models.

It works by using modern c++ techniques including *concepts*, *template meta-programming* and *class-template argument deduction* to provide a **declarative** api. Originally inspired by `golang semantic tags`.

```c++
struct Foo: ctom::obj<                              // declare model w. implementation
    ctom::key<"my-int", int>
>{
    int some_int = 2;
    Foo_Impl(){
      this->val<"my-int">() = some_int;
    }
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

#### Examples

<details>
  <summary>Simple Object</summary>

```c++
using Foo = ctom::obj<
    ctom::key<"foo-int", int>,
    ctom::key<"foo-float", float>,
    ctom::key<"foo-double", double>
>;

ctom::print<Foo>(); // NOTE: FULLY STATIC! NO INSTANCE!
```
```bash
val: [foo-int]
val: [foo-float]
val: [foo-double]
```
</details>

<details>
  <summary>Simple Array</summary>

```c++
using Barr = ctom::arr<4, int>;

ctom::print<Barr>();
```
```bash
val: [0]
val: [1]
val: [2]
val: [3]
```
</details>


<details>
  <summary>Nested Object/Array</summary>

```c++
using Bar = ctom::obj<
    ctom::key<"bar-foo", Foo>,
    ctom::key<"bar-char", char>,
    ctom::key<"bar-barr", Barr>
>;

using Baz = ctom::obj<
    ctom::key<"baz-bar", Bar>,
    ctom::key<"baz-bool", bool>
>;

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
</details>

<details>
  <summary>Nested Array/Object</summary>

```c++
using Maz = ctom::obj<
    ctom::key<"maz-char", char>
>;

using Marr = ctom::arr<3, Maz>;

using MarrArr = ctom::arr<2, Marr>;

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
</details>

<details>
  <summary>Extended Object</summary>

```c++
using FooExt = Foo::ext<
    ctom::key::val<"foo-ext-int", int>,
    ctom::key::obj<"foo-ext-foo", Foo>
>;

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
</details>

<details>
  <summary>Extended Array</summary>

```c++
using MarrExt = Marr::ext<3, Maz>;

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
</details>

### Run-Time Object-Model Implementations

`Note: All code in this section taken from examples/1_impl`

You can also combine the declaration and the implementation directly.

#### Examples

<details>
  <summary>Simple Object</summary>

```c++
struct Foo_Impl: Foo {
  int x = 1;
  float y = 0.5f;
  Foo_Impl(){
    this->val<"foo-int">() = x;
    this->val<"foo-float">() = y;
  }
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
</details>

<details>
  <summary>Simple Array</summary>

```c++
struct Barr_Impl: Barr {
  int barr[4] = {0, 1, 2, 3};
  Barr_Impl(){
    this->val<0>() = barr[0];
    this->val<1>() = barr[1];
    this->val<2>() = barr[2];
    this->val<3>() = barr[3];
  }
} barr_impl;

ctom::print(barr_impl);

barr_impl.barr[0] = 3;  // direct assignment
barr_impl.barr[1] = 2;
barr_impl.barr[2] = 1;
barr_impl.barr[3] = 0;

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
</details>

<details>
  <summary>Nested Object/Array</summary>

```c++
struct Bar_Impl: Bar {
  Foo_Impl foo;
  char c = 'x';
  Barr_Impl barr;
  Bar_Impl(){
    this->val<"bar-foo">() = foo;
    this->val<"bar-char">() = c;
    this->val<"bar-barr">() = barr;
  }
};

struct Baz_Impl: Baz {
  Bar_Impl bar_impl;
  bool b = true;
  Baz_Impl(){
    this->val<"baz-bar">() = bar_impl;
    this->val<"baz-bool">() = b;
  }
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
</details>

<details>
  <summary>Nested Array/Object</summary>

```c++
struct Maz_Impl: Maz {
  char c = ' ';
  Maz_Impl(){
     this->val<"maz-char">() = c;
  }
};

struct Marr_Impl: Marr {
  Maz_Impl maz[3];
  Marr_Impl(){
    this->val<0>() = maz[0];
    this->val<1>() = maz[1];
    this->val<2>() = maz[2];
  }
};

struct MarrArr_Impl: MarrArr {
  Marr_Impl marr[2];
  MarrArr_Impl(){
    this->val<0>() = marr[0];
    this->val<1>() = marr[1]; 
  }
} marrarr_impl;

marrarr_impl.marr[0].maz[0].c = 'a';
marrarr_impl.marr[0].maz[1].c = 'b';
marrarr_impl.marr[0].maz[2].c = 'c';
marrarr_impl.marr[1].maz[0].c = 'd';
marrarr_impl.marr[1].maz[1].c = 'e';
marrarr_impl.marr[1].maz[2].c = 'f';

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
</details>

### Stream-Serialization

Different serialization formats have stream-modifiers, which allow you to pass an object-model instance directly to the stream.

Note that parsing of serialized data is simplified in this library, because of the declarative object-model. Instead of infering structure from the data, structure is known and the data is expected to match.

#### yaml

##### emit

```c++
std::cout << ctom::yaml::emit << foo_impl;
```

```yaml
"foo-int": 1
"foo-float": 0.5
"foo-double": null
```

##### parse

```c++
std::ifstream yaml_file("config.yaml");
if(yaml_file.is_open()){

  try {
    yaml_file >> ctom::yaml::parse >> foo_impl;
  } catch(ctom::yaml::exception e){
    std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
  }

  yaml_file.close();

}
```

#### json

```c++
std::cout << ctom::json::emit << foo_impl;
```

```json
{
  "foo-int": "1",
  "foo-float": "0.5",
  "foo-double": ""
}
```

`ctom::json::parse`: **WIP**

## Details

### todo

- the various serialization formats / namespace should implement static iterators to validate the object-models (e.g.: valid json keys, valid yaml keys)
- swap some concepts with static assertions, for more helpful error messages
- moving and assignment into array and stl containers would be very convenient and ease development using `ctom`.

- handling of different quote types, scalar types

### other thoughts

There are a number of features which `c++20` doesn't have, which make a more elegant interface difficult. For instance, there are no static subscript operators, which mean that I can't properly index without using user-defined literals. But then to index with a number (e.g. `size_t`), this is not possible at all because of the way number-literals are (arbitrarily) restrained. What a mess.

Therefore, we **have** to use static `get<>` member functions.
