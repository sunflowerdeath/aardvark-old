#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

extern crate downcast_rs;

use std::ffi::CString;

mod quickjs;

#[derive(PartialEq, Debug)]
enum ValueType {
    Null,
    Undefined,
    Bool,
    Number,
    String,
    Object,
    Unknown,
}

trait PointerData: downcast_rs::Downcast {
    fn box_clone(&self) -> Box<dyn PointerData>;
}
downcast_rs::impl_downcast!(PointerData);

impl Clone for Box<dyn PointerData> {
    fn clone(&self) -> Box<dyn PointerData> {
        self.box_clone()
    }
}

struct JsError<'a> {
    val: Box<Value<'a>>,
}

#[derive(Clone)]
struct Pointer<'a> {
    ctx: &'a dyn Context<'a>,
    data: Box<dyn PointerData>,
}

#[derive(Clone)]
struct Value<'a> {
    ptr: Pointer<'a>,
}

impl<'a> Value<'a> {
    fn get_type(&self) -> ValueType {
        return self.ptr.ctx.value_get_type(self);
    }

    fn to_bool(&self) -> Result<bool, JsError> {
        return self.ptr.ctx.value_to_bool(self);
    }

    fn to_number(&self) -> f64 {
        return self.ptr.ctx.value_to_number(self);
    }

    fn to_object(&'a self) -> Result<Object, JsError> {
        return self.ptr.ctx.value_to_object(self);
    }
}

#[derive(Clone)]
struct Object<'a> {
    ptr: Pointer<'a>,
}

trait Context<'a> {
    fn eval(&'a self, source: &str) -> Value<'a>;

    // fn value_make_null(&self) -> JsValue;
    // fn value_make_undefined(&self) -> JsValue;
    fn value_make_bool(&self, val: bool) -> Value;
    // fn value_make_number(&self, val: f64) -> JsValue;
    // fn value_make_string(&self, str: &JsString) -> JsValue;
    // fn value_make_object(&self, obj: &JsObject) -> JsValue;

    fn value_get_type(&self, val: &Value<'a>) -> ValueType;

    fn value_to_bool(&self, val: &Value<'a>) -> Result<bool, JsError>;
    fn value_to_number(&self, val: &Value<'a>) -> f64;
    fn value_to_object(&'a self, val: &Value<'a>) -> Result<Object, JsError>;
}

// QJS

struct QjsValue {
    ctx: *mut quickjs::JSContext,
    val: quickjs::JSValue,
}

impl Clone for QjsValue {
    fn clone(&self) -> QjsValue {
        unsafe {
            quickjs::JS_DupValue_noinl(self.ctx, self.val);
        }
        return QjsValue {
            ctx: self.ctx,
            val: self.val,
        };
    }
}

impl Drop for QjsValue {
    fn drop(&mut self) {
        unsafe {
            quickjs::JS_FreeValue_noinl(self.ctx, self.val);
        }
    }
}

impl PointerData for QjsValue {
    fn box_clone(&self) -> Box<dyn PointerData> {
        return Box::new(self.clone());
    }
}

impl QjsValue {
    fn new(ctx: &Qjs_Context, qjs_val: quickjs::JSValue) -> Value {
        let data = Box::new(QjsValue {
            ctx: ctx.ctx,
            val: qjs_val,
        });
        return Value {
            ptr: Pointer { ctx, data },
        };
    }
}

fn get_qjs_val(val: &Value) -> quickjs::JSValue {
    return val.ptr.data.downcast_ref::<QjsValue>().unwrap().val;
}

struct Qjs_Context {
    rt: *mut quickjs::JSRuntime,
    ctx: *mut quickjs::JSContext,
}

impl Drop for Qjs_Context {
    fn drop(&mut self) {
        unsafe {
            quickjs::JS_FreeContext(self.ctx);
            quickjs::JS_FreeRuntime(self.rt);
        }
    }
}

impl Qjs_Context {
    fn new() -> Qjs_Context {
        unsafe {
            let rt = quickjs::JS_NewRuntime();
            let ctx = quickjs::JS_NewContext(rt);
            return Qjs_Context { rt, ctx };
        }
    }
}

impl<'a> Context<'a> for Qjs_Context {
    fn eval(&'a self, source: &str) -> Value<'a> {
        let source_size = source.len();
        let source = CString::new(source).unwrap();
        let source_url = CString::new("SOURCEURL").unwrap();
        unsafe {
            let val = quickjs::JS_Eval(
                self.ctx,
                source.as_ptr(),
                source_size,
                source_url.as_ptr(),
                quickjs::JS_EVAL_TYPE_GLOBAL as i32,
            );
            return QjsValue::new(&self, val);
        }
    }

    fn value_make_bool(&self, val: bool) -> Value {
        unsafe {
            let qjs_val = quickjs::JS_NewBool_noinl(self.ctx, val as i32);
            return QjsValue::new(&self, qjs_val);
        }
    }

    fn value_get_type(&self, val: &Value<'a>) -> ValueType {
        let qjs_val = get_qjs_val(val);
        unsafe {
            if quickjs::JS_IsBool_noinl(qjs_val) == 1 {
                return ValueType::Bool;
            }
            if quickjs::JS_IsNumber(qjs_val) == 1 {
                return ValueType::Number;
            }
            if (quickjs::JS_IsNull_noinl(qjs_val)) == 1 {
                return ValueType::Null;
            }
            if (quickjs::JS_IsUndefined_noinl(qjs_val)) == 1 {
                return ValueType::Undefined;
            }
            if (quickjs::JS_IsString_noinl(qjs_val)) == 1 {
                return ValueType::String;
            }
            if (quickjs::JS_IsObject_noinl(qjs_val)) == 1 {
                return ValueType::Object;
            }
            return ValueType::Unknown;
        }
    }

    fn value_to_bool(&self, val: &Value<'a>) -> Result<bool, JsError> {
        unsafe {
            let res = quickjs::JS_ToBool(self.ctx, get_qjs_val(val));
            return Ok(res == 1);
        }
    }

    fn value_to_number(&self, val: &Value<'a>) -> f64 {
        let mut num = 0.0_f64;
        let qjs_val = val.ptr.data.downcast_ref::<QjsValue>().unwrap();
        unsafe {
            quickjs::JS_ToFloat64(self.ctx, &mut num, qjs_val.val);
        }
        return num;
    }

    fn value_to_object(&self, val: &Value<'a>) -> Result<Object<'a>, JsError> {
        return Ok(Object {
            ptr: val.ptr.clone(),
        });
    }
}

fn main() {
    println!("Hello World!");

    let hello = String::from("2 + 3");
    let source_size = hello.len();
    let source = CString::new(hello).unwrap();
    let source_url = CString::new("SOURCEURL").unwrap();
    let mut num = 0.0_f64;
    unsafe {
        let rt = quickjs::JS_NewRuntime();
        let ctx = quickjs::JS_NewContext(rt);
        let res = quickjs::JS_Eval(
            ctx,
            source.as_ptr(),
            source_size,
            source_url.as_ptr(),
            quickjs::JS_EVAL_TYPE_GLOBAL as i32,
        );
        quickjs::JS_ToFloat64(ctx, &mut num, res);
    }
    println!("{}", num);

    let ctx = Qjs_Context::new();
    let res = ctx.eval("2 + 3");
    println!("{}", res.to_number());

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

    let res3 = ctx.eval("a={}");
    println!("Type: {:?}", res3.get_type());
    let o = res3.to_object();
    match o {
        Ok(obj) => {
            println!("object");
        }
        _ => (),
    }
}
