bindgen \
    ../../../newcore/externals/quickjs/quickjs.h \
    -o ./src/quickjs.rs \
    -- -DBINDGEN=1
