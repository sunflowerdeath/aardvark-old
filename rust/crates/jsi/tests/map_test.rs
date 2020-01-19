use aardvark_jsi as mjsi;
use aardvark_jsi::Context;
use aardvark_jsi::Mapper;

fn make_err_params() -> mjsi::CheckErrorParams {
    mjsi::CheckErrorParams {
        kind: "kind".to_owned(),
        name: "name".to_owned(),
        target: "target".to_owned(),
    }
}

#[test]
fn primitive() {
    let ctx = mjsi::QjsContext::new();

    // to_js
    let val = mjsi::NUMBER_MAPPER.to_js(ctx.as_ref(), &2.0);
    assert_eq!(val.to_number().unwrap(), 2.0);

    // from_js
    let val = ctx.value_make_number(3.5);
    let res = mjsi::NUMBER_MAPPER.from_js(ctx.as_ref(), &val);
    assert_eq!(res, 3.5);

    let err_params = make_err_params();

    // try_from_js ok
    let val = ctx.value_make_number(2.0);
    let res = mjsi::NUMBER_MAPPER.try_from_js(ctx.as_ref(), &val, &err_params);
    assert!(res.is_ok());
    assert_eq!(res.unwrap(), 2.0);

    // try_from_js err
    let val = ctx.value_make_bool(true);
    let res = mjsi::NUMBER_MAPPER.try_from_js(ctx.as_ref(), &val, &err_params);
    assert!(res.is_err());
}

#[derive(Default)]
struct TestStruct {
    pub num: f64,
    pub str: String,
}

#[test]
fn struct_mapper() {
    let ctx = mjsi::QjsContext::new();

    let mapper = mjsi::struct_mapper!(TestStruct {
        num: f64 => mjsi::NUMBER_MAPPER,
        str: String => mjsi::STRING_MAPPER,
    });

    // to_js
    {
        let val = TestStruct {
            num: 2.0,
            str: "test".to_owned(),
        };
        let res = mapper.to_js(ctx.as_ref(), &val);
        let obj = res.to_object().unwrap();
        assert_eq!(obj.get_prop("num").unwrap().to_number().unwrap(), 2.0);
        assert_eq!(
            obj.get_prop("str").unwrap().to_string().unwrap().to_utf8(),
            "test"
        );
    }

    let obj = ctx.object_make(None);
    let _ = obj.set_prop("num", &ctx.value_make_number(2.0));
    let _ = obj.set_prop(
        "str",
        &ctx.value_make_string(&ctx.string_make_from_utf8("test")),
    );

    // from_js
    {
        let res = mapper.from_js(ctx.as_ref(), &obj.to_value());
        assert_eq!(res.num, 2.0);
        assert_eq!(res.str, "test");
    }

    let err_params = make_err_params();

    // try_from_js ok
    {
        let res =
            mapper.try_from_js(ctx.as_ref(), &obj.to_value(), &err_params);
        assert!(res.is_ok());
        assert_eq!(res.unwrap().num, 2.0);
    }

    // try_from_js err
    {
        let val = ctx.value_make_number(2.0);
        let res = mapper.try_from_js(ctx.as_ref(), &val, &err_params);
        assert!(res.is_err());
    }

    // try_from_js field missing
    {
        let obj = ctx.object_make(None);
        let res =
            mapper.try_from_js(ctx.as_ref(), &obj.to_value(), &err_params);
        assert!(res.is_err());
    }

    // try_from_js field type err
    {
        let obj = ctx.object_make(None);
        let _ = obj.set_prop("num", &ctx.value_make_bool(true));
        let _ = obj.set_prop("str", &ctx.value_make_bool(true));
        let res =
            mapper.try_from_js(ctx.as_ref(), &obj.to_value(), &err_params);
        assert!(res.is_err());
    }
}
