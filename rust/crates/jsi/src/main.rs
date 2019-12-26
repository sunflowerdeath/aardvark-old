pub mod jsi;
pub mod qjs;

use crate::jsi::Context;

fn main() {
    println!("Hello World!");

    let ctx = qjs::QjsContext::new();
    let res = ctx.eval("2 + 3");
    if let Ok(num) = res.to_number() {
        println!("{}", num);
    }

    let res2 = ctx.eval("2+3 == 5");
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

    let res3 = ctx.eval("a={prop: 42}");
    println!("Type: {:?}", res3.get_type());
    let o = res3.to_object();
    match o {
        Ok(obj) => {
            println!("object");
            let num = obj.get_prop("prop").unwrap().to_number().unwrap();
            println!("prop: {}", num);
        }
        _ => (),
    }

    let func = |this: jsi::Value, args: Vec<jsi::Value>| {
        println!("CALL FN");
        Ok(ctx.value_make_number(
            args[0].to_number().unwrap() + args[1].to_number().unwrap(),
        ))
    };
    let jsfn = ctx.object_make_func(Box::new(func));
    let res = jsfn.call_as_function(
        None,
        &vec![ctx.value_make_number(5.0), ctx.value_make_number(6.0)],
    ).unwrap();
    println!("CALL RESULT: {}", res.to_number().unwrap());
}
