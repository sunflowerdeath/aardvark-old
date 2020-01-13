use aardvark_jsi as jsi;
use aardvark_jsi::Context;
use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

#[test]
fn eval() {
    let ctx = jsi::QjsContext::new();
    let res = ctx.eval("2 + 3", "source_url");
    assert!(res.is_ok());
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

#[test]
fn function() {
    let ctx = jsi::QjsContext::new();

    let is_called = Rc::new(RefCell::new(false));
    let out_this = Rc::new(RefCell::new(None));
    let out_args = Rc::new(RefCell::new(vec![]));

    let func;
    {
        let is_called = is_called.clone();
        let out_this = out_this.clone();
        let out_args = out_args.clone();
        func = move |ctx: &dyn Context,
                     this: jsi::Value,
                     args: Vec<jsi::Value>| {
            is_called.replace(true);
            out_this.replace(Some(this));
            out_args.replace(args);
            Ok(ctx.value_make_number(1.0))
        };
    }
    let obj = ctx.object_make_func(Rc::new(func));

    assert!(!ctx.object_make(None).is_function());
    assert!(obj.is_function());

    let this = Some(ctx.value_make_number(2.0));
    let args = vec![ctx.value_make_number(3.0)];
    let res = obj.call_as_function(this.as_ref(), &args).unwrap();

    assert!(*is_called.borrow());
    assert_eq!(res.to_number().unwrap(), 1.0);
    assert_eq!(
        out_this.borrow().as_ref().unwrap().to_number().unwrap(),
        2.0
    );
    assert_eq!(out_args.borrow().len(), 1);
    assert_eq!(out_args.borrow()[0].to_number().unwrap(), 3.0);
}

#[test]
fn private() {
    let ctx = jsi::QjsContext::new();

    let def = jsi::ClassDefinition {
        name: "TestClass".to_string(),
        ..Default::default()
    };
    let cls = ctx.class_make(&def);
    let inst = ctx.object_make(Some(&cls));
    let mut data: i32 = 25;

    inst.set_private_data(&mut data as *mut _ as *mut std::ffi::c_void);
    let res = inst.get_private_data();
    let get_data = unsafe { *(res as *mut i32) };
    assert_eq!(data, get_data);
}

#[test]
fn class() {
    let finalizer_called = Rc::new(RefCell::new(false));

    {
        let ctx = jsi::QjsContext::new();

        let prop_value = Rc::new(RefCell::new(1.0));
        let get = {
            let prop_value = prop_value.clone();
            move |ctx: &dyn jsi::Context, _obj: jsi::Object| {
                Ok(ctx.value_make_number(*prop_value.borrow()))
            }
        };
        let set = {
            let prop_value = prop_value.clone();
            move |_ctx: &dyn jsi::Context,
                  _obj: jsi::Object,
                  val: jsi::Value| {
                prop_value.replace(val.to_number().unwrap());
                Ok(())
            }
        };
        let method = {
            let prop_value = prop_value.clone();
            move |_ctx: &dyn jsi::Context,
                  _this: jsi::Value,
                  args: Vec<jsi::Value>| {
                prop_value.replace(args[0].to_number().unwrap());
                Ok(args[0].clone())
            }
        };
        let finalizer = {
            let finalizer_called = finalizer_called.clone();
            move |_obj: jsi::Object| {
                finalizer_called.replace(true);
            }
        };

        let mut methods: HashMap<String, jsi::Function> = HashMap::new();
        methods.insert("method".to_string(), Rc::new(method));

        let mut props = HashMap::new();
        props.insert(
            "prop".to_string(),
            jsi::ClassPropertyDefinition {
                get: Some(Rc::new(get)),
                set: Some(Rc::new(set)),
            },
        );

        let def = jsi::ClassDefinition {
            name: "TestClass".to_string(),
            methods,
            props,
            finalizer: Some(Rc::new(finalizer)),
        };

        let cls = ctx.class_make(&def);
        let inst = ctx.object_make(Some(&cls));

        let getter_res = inst.get_prop("prop").unwrap();
        assert_eq!(getter_res.to_number().unwrap(), 1.0);

        let _ = inst.set_prop("prop", &ctx.value_make_number(2.0));
        assert_eq!(*prop_value.borrow(), 2.0);

        let _ = ctx.get_global_object().set_prop("inst", &inst.to_value());
        let res = ctx.eval("inst.method(3)", "sourceurl").unwrap();
        assert_eq!(*prop_value.borrow(), 3.0);
        assert_eq!(res.to_number().unwrap(), 3.0);
    }

    assert!(*finalizer_called.borrow());
}
