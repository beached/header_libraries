# string_view

An enhanced string_view implementation with parsing helpers.

[API Documentation](https://beached.github.io/string_view/api_docs/html/structdaw_1_1sv2_1_1basic__string__view.html)

## Additions not in std::string_view

One can build simple BNF like parsers quickly via methods like `parse_front_until`. The interface supports predicates
which have a couple predefined predicates `any_of<Char...>` and `none_of<Char...>`.

```c++
struct uri_parts {
   string_view scheme;
   string_view authority;
   string_view path;
   string_view query;
   string_view fragment;
};
// scheme://authority:port/path/?query#fragment
uri_parts parse_url( string_view uri_string ) {
    return {
       /* scheme    */ uri_string.pop_fron_until( "://" ),
       /* authority */ uri_string.pop_front_until( any_of<'/', '?', '#'>, nodiscard ),
       /* path      */ uri_string.pop_front_until( any_of<'?', '#'> ),
       /* query     */ uri_string.pop_front_until( '#' ),
       /* fragment  */ uri_string
    };
}
```

By default the `pop_front_until` methods will discard the delimiter. This behaviour can be controlled by supplying an
addition parameter `nodiscard`. The result being that the delimiter is left in the original `string_view`

# Easy string splitting

String splitting is trivial with the `pop_front_until` and `trim_copy` methods. It does not require allocation either.

```c++
string_view2 foo = "1,2,3,4";
while( not foo.empty( ) ) {
    auto part = foo.pop_front_until( ',' ).trim_copy( );
    // part will equal "1", "2", "3", and "4"
}
```

# Storage of end of range

The storage of the start/end of the string range is optimized, by default, for a `remove_prefix` like work load. There
are two schemes, pointer/pointer(the default) and pointer/size_t. They are optionally changed via the second template
parameter for basic_string_view with the enumeration `string_view_bounds_type { pointer, size }`. With pointer/pointer,
a remove_prefix only has to mutate the start of range pointer, not the end of range. However, this means that the
calculation of `size( )` is a subtraction.

# Construction via null pointers

The range formed by the pair (nullptr, nullptr) or (nullptr, 0) is a valid empty range. It is useful for indicating no
value and is often returned by c-api methods as a no-value return. With that, the
constructor `string_view( const_pointer )` will check for null and form an empty string range with `data( ) == nullptr`
and `size( ) == 0`. In many cases this check will be elided by compilers or is small compared to the cost of a `strlen`
like call.

# Remove prefix

Remove prefix will default to removing 1 element from range.  
