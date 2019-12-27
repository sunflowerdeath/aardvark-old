use aardvarkjsi as jsi;
use aardvarkjsi::Context;

#[test]
fn eval() {
    let ctx = jsi::QjsContext::new();
    let res = ctx.eval("2 + 3", "source_url");
    assert_eq!(res.unwrap().to_number().unwrap(), 5.0);
}

#[test]
fn eval_error() {
    let ctx = jsi::QjsContext::new();

    let res1 = ctx.eval("a/b/", "source_url");
    assert!(res1.is_err());

    let res2 = ctx.eval("2+2", "source_url");
    assert!(res2.is_ok());

    let res3 = ctx.eval("a/b/", "source_url");
    assert!(res3.is_err());
}

#[test]
fn string() {
    let ctx = jsi::QjsContext::new();

    let str = ctx.string_make_from_utf8("test");
    assert_eq!(str.to_utf8(), "test");
}

#[test]
fn value() {
    let ctx = jsi::QjsContext::new();

    let null_val = ctx.value_make_null();
    assert_eq!(null_val.get_type(), jsi::ValueType::Null);

    let undef_val = ctx.value_make_undefined();
    assert_eq!(undef_val.get_type(), jsi::ValueType::Undefined);

    let num_val = ctx.value_make_number(1.5);
    assert_eq!(num_val.get_type(), jsi::ValueType::Number);
    assert_eq!(num_val.to_number().unwrap(), 1.5);

    let bool_val = ctx.value_make_bool(true);
    assert_eq!(bool_val.get_type(), jsi::ValueType::Bool);
    assert!(bool_val.to_bool().unwrap());

    let str = ctx.string_make_from_utf8("test");
    let str_val = ctx.value_make_string(&str);
    assert_eq!(str_val.get_type(), jsi::ValueType::String);
    assert_eq!(str_val.to_string().unwrap().to_utf8(), "test");
}
