### Contributing to Maya to Hydra

## Identation

The project follows the 1TBS variant of K&R style, with a limit of 80 columns per line. To make sure your code follows our guidelines run `clang-format` (we use version 6.0.0) on the codebase before submitting a pull request.

## Naming scheme

We follow USD's naming style:

Classes are named CamelCase, starting with a module specific prefix unless they are not in a header. Struct is used instead of a class when storing related data in a compact way, without visibility or member functions and they only need the module specific prefix if exposed in a header.

For the base library we use HdMaya as the prefix, and Mtoh for the maya plugin.

```
// Base library
class HdMayaMyClass; // ok
class MyClass; // not ok
class myClass; // not ok

// Maya plugin
class MtohMyClass; // ok
```

Functions are also CamelCase, private functions on classes and source only functions are prefixed with an _ .

```
void MyFunction(); // ok
void myFunction(); // not ok
void my_function(); // not ok

class HdMayaMyClass {
public:
    void MyFunction(); // ok
private:
    void MyOtherFunction(); // not ok
    void _MyFunction(); // ok
};

```

Variables are camelCase, class variables are prefixed with an _ .

```
int myVariable; // ok
int my_varialbe; // not ok

class HdMayaMyClass {
private:
    int myVariable; // not ok
    int _myVariable; // ok
};

struct MyStruct {
    int _myVariable; // not ok
    int myVariable; // ok
};
```

File names are camelCase, header files use the "h" extensions, source files use "cpp".

```
delegate.cpp // ok
Delegate.cpp // not ok
delegate.cxx // not ok

delegate.hpp // not ok
```

## Includes

Include guards are preferred, because we follow USD's method of copying headers during build. Include guards should be named `__HDMAYA_CAMEL_CASE_H__` or `__MTOH_CAMEL_CASE_H__`, where camel case represents each word in the header file's name.

Prefer using `"fileName.h"` for relative, same directory, includes and `<fileName.h>` for includes coming from outside the current directory. Note, headers from delegates and adapters are outside the current directory, due to the header copying.

## Coding style

We use c++11 as our c++ standard, and switching to c++14 once Maya caught up with the vfx platform.

- Prefer the use of type inference wherever possible.
- Use west const.
- Prefer range-based for loops.
- Use RAII wherever it makes sense.
- Prefer the use of lambdas and STL algorithms.
- Prefer the use constexpr wherever possible.

## Documentation

Always precede public classes and functions with a Doxygen-style comment. Include a
brief description every time.

Doxygen commands and tags are prefixed with a backslash.

Always and a short description to each header file after the license.

Use three slashes `///` for C++ doxygen comments. After member documentation
is done with `///<`.
