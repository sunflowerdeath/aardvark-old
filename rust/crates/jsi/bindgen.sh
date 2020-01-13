bindgen \
    ../../externals/quickjs/quickjs.h \
    --raw-line "\
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]\
"\
    -o ./src/qjs/ffi.rs \
