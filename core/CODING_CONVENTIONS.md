# Aardvark C++ coding conventions

## Files

Use `.cpp` and `.hpp` extension.
Name files using `snake_case`.

Prefer to have one class per file.
Related structs/enums can be placed in the same file.

## Formatting

Code should be formatted with clang-format using `.clang-format` config
in the project dir.

## Naming

Naming should be like this:

```cpp
namespace namespace_name {

class ClassName {
  public:
    int method_name(int arg_name) {
        // ...
    }
};

int function_name() {
    auto variable_name = 1;
}

using TypeAlias = int;

enum class EnumName { enum_option1, enum_option2 };

}  // namespace namespace_name
```

Do not use prefixes in names.

```
// Good
class ClassName {
  public:
    int public_member;

  private:
    int private_member;
};

// Not very good
class ADV_ClassName {
  public:
    int m_public_member;

  private:
    int _private_member;
};

// If you need to distinguish between different types, use suffixes:
auto val = 1;
auto val_ptr = &val;
auto val_sptr = get_shared_ptr();

```

## Function parameters

Prefer this order of parameters:

1. Immutable parameters. Pass by const reference or by value for small types,
2. Optional immutable parameters. Pass by pointer to const.
3. Mutable/output parameters, pass as pointers.

When calling function or initializing struct with multiple parameters,
add comments to improve readability.

```c++
some_function(/* arg1 */ 1, /* arg2 */ 2);

// OR

some_function(
    1,  // arg1
    12  // arg2
);

auto some_struct = SomeStruct{
    12,  // member1
    2    // member1
};
```
