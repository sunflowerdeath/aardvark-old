extern crate aardvark_jsi;

use aardvark_jsi as jsi;
use aardvark_jsi::Context;

fn main() {
    println!("Hello World!");

    let ctx = jsi::QjsContext::new();
    let res = ctx.eval("2 + 3", "").unwrap();
    if let Ok(num) = res.to_number() {
        println!("{}", num);
    }

    let res2 = ctx.eval("2+3 == 5", "").unwrap();
    let b = res2.to_bool();
    match b {
        Ok(val) => {
            println!("{}", val);
            let res3 = res2.clone();
            if let Ok(b2) = res3.to_bool() {
                println!("{}", b2);
            }
        }
        _ => (),
    }

    let res3 = ctx.eval("a={prop: 42}", "").unwrap();
    println!("Type: {:?}", res3.get_type());
    let obj = res3.to_object().unwrap();
    println!("object");
    let num = obj.get_prop("prop").unwrap().to_number().unwrap();
    println!("prop: {}", num);
    let _ = obj.set_prop("test", &ctx.value_make_number(24.0));
    let test = obj.get_prop("test").unwrap().to_number().unwrap();
    println!("test: {}", test);

    let func = |ctx: &dyn Context, _this: jsi::Value, args: Vec<jsi::Value>| {
        println!("CALL FN");
        Ok(ctx.value_make_number(
            args[0].to_number().unwrap() + args[1].to_number().unwrap(),
        ))
    };
    let jsfn = ctx.object_make_func(std::rc::Rc::new(func));
    let res = jsfn
        .call_as_function(
            None,
            &vec![ctx.value_make_number(5.0), ctx.value_make_number(6.0)],
        )
        .unwrap();
    println!("CALL RESULT: {}", res.to_number().unwrap());
}
