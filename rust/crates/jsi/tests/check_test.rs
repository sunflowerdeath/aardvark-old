use aardvark_jsi as jsi;
use aardvark_jsi::Context;

fn require_result(
    checker: &dyn jsi::Checker, ctx: &dyn jsi::Context, val: &jsi::Value,
    res: bool,
) {
    let err_params = jsi::CheckErrorParams {
        kind: "kind".to_string(),
        name: "name".to_string(),
        target: "target".to_string(),
    };
    let result = checker.check(ctx, val, &err_params);
    if res {
        assert!(result.is_ok());
    } else {
        assert!(result.is_err());
    }
}

fn require_err(
    checker: &dyn jsi::Checker, ctx: &dyn jsi::Context, val: &jsi::Value,
) {
    require_result(checker, ctx, val, false);
}

fn require_ok(
    checker: &dyn jsi::Checker, ctx: &dyn jsi::Context, val: &jsi::Value,
) {
    require_result(checker, ctx, val, true);
}

#[test]
fn primitive() {
    let ctx = jsi::QjsContext::new();
    require_ok(
        jsi::NUMBER_CHECKER.as_ref(),
        ctx.as_ref(),
        &ctx.value_make_number(21.0),
    );
    require_err(
        jsi::NUMBER_CHECKER.as_ref(),
        ctx.as_ref(),
        &ctx.value_make_bool(true),
    );
}

#[test]
fn objects() {
    let ctx = jsi::QjsContext::new();
    let obj = ctx.object_make(None).to_value();

    require_ok(jsi::OBJECT_CHECKER.as_ref(), ctx.as_ref(), &obj);
    // object is not array
    require_err(jsi::ARRAY_CHECKER.as_ref(), ctx.as_ref(), &obj);

    let arr = ctx.object_make_array().to_value();
    require_ok(jsi::OBJECT_CHECKER.as_ref(), ctx.as_ref(), &arr);
    // but array is an object
    require_ok(jsi::ARRAY_CHECKER.as_ref(), ctx.as_ref(), &arr);
    require_err(jsi::NUMBER_CHECKER.as_ref(), ctx.as_ref(), &arr);
}
