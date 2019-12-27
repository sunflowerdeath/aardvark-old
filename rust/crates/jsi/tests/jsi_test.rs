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
