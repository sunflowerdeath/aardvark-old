pub mod ffi;

use crate::jsi::*;
use std::ffi::CString;

// QjsString

#[derive(Clone)]
struct QjsString {
    str: String,
}

impl PointerData for QjsString {
    fn box_clone(&self) -> Box<dyn PointerData> {
        return Box::new(self.clone());
    }
}

impl QjsString {
    fn new(rs_str: String) -> QjsString {
        QjsString { str: rs_str }
    }
}

// QjsValue

struct QjsValue {
    ctx: *mut ffi::JSContext,
    val: ffi::JSValue,
}

impl Clone for QjsValue {
    fn clone(&self) -> QjsValue {
        unsafe {
            ffi::JS_DupValue_noinl(self.ctx, self.val);
        }
        QjsValue {
            ctx: self.ctx,
            val: self.val,
        }
    }
}

impl Drop for QjsValue {
    fn drop(&mut self) {
        unsafe {
            ffi::JS_FreeValue_noinl(self.ctx, self.val);
        }
    }
}

impl PointerData for QjsValue {
    fn box_clone(&self) -> Box<dyn PointerData> {
        return Box::new(self.clone());
    }
}

impl QjsValue {
    fn new(ctx: &QjsContext, qjs_val: ffi::JSValue) -> QjsValue {
        QjsValue {
            ctx: ctx.ctx,
            val: qjs_val,
        }
    }
}

// Helpers

fn qjs_null() -> ffi::JSValue {
    ffi::JSValue {
        u: ffi::JSValueUnion { int32: 0 },
        tag: ffi::JS_TAG_NULL as i64,
    }
}

fn value_from_qjs(ctx: &QjsContext, qjs_val: ffi::JSValue) -> Value {
    Value {
        ptr: Pointer {
            ctx,
            data: Box::new(QjsValue::new(ctx, qjs_val)),
        },
    }
}

fn object_from_qjs(ctx: &QjsContext, qjs_val: ffi::JSValue) -> Object {
    Object {
        ptr: Pointer {
            ctx,
            data: Box::new(QjsValue::new(ctx, qjs_val)),
        },
    }
}

fn pointer_to_qjs(ptr: &Pointer) -> ffi::JSValue {
    ptr.data.downcast_ref::<QjsValue>().unwrap().val
}

fn value_to_qjs(val: &Value) -> ffi::JSValue {
    pointer_to_qjs(&val.ptr)
}

fn object_to_qjs(obj: &Object) -> ffi::JSValue {
    pointer_to_qjs(&obj.ptr)
}

// NativeFunction

static mut FUNCTION_CLASS_ID: u32 = 0;

extern "C" fn native_function_finalizer(
    _: *mut ffi::JSRuntime,
    val: ffi::JSValue,
) {
    unsafe {
        let ptr = ffi::JS_GetOpaque(val, 0);
        std::alloc::dealloc(
            ptr as *mut u8,
            std::alloc::Layout::new::<Function>(),
        );
    }
}

extern "C" fn native_function_call(
    ctx: *mut ffi::JSContext,
    func_obj: ffi::JSValue,
    this_val: ffi::JSValue,
    argc: i32,
    argv: *mut ffi::JSValue,
) -> ffi::JSValue {
    unsafe {
        let func =
            ffi::JS_GetOpaque(func_obj, FUNCTION_CLASS_ID) as *mut Function;
        let jsi_ctx = QjsContext::get(ctx);
        ffi::JS_DupValue_noinl(ctx, this_val);
        let jsi_this = value_from_qjs(jsi_ctx, this_val);
        let mut jsi_args = Vec::new();
        for i in 0..argc {
            let qjs_arg = *argv.offset(i as isize);
            ffi::JS_DupValue_noinl(ctx, qjs_arg);
            jsi_args.push(value_from_qjs(jsi_ctx, qjs_arg));
        }
        let res = (*func)(jsi_this, jsi_args);
        match res {
            Ok(val) => {
                let qjs_res = value_to_qjs(&val);
                ffi::JS_DupValue_noinl(ctx, qjs_res);
                return qjs_res;
            }
            Err(err) => {
                let qjs_ex = value_to_qjs(err.val.as_ref());
                ffi::JS_DupValue_noinl(ctx, qjs_ex);
                return ffi::JS_Throw(ctx, qjs_ex);
            }
        }
    }
}

// QjsContext

pub struct QjsContext {
    rt: *mut ffi::JSRuntime,
    ctx: *mut ffi::JSContext,
}

impl Drop for QjsContext {
    fn drop(&mut self) {
        unsafe {
            ffi::JS_FreeContext(self.ctx);
            ffi::JS_FreeRuntime(self.rt);
        }
    }
}

impl QjsContext {
    pub fn new() -> QjsContext {
        unsafe {
            let rt = ffi::JS_NewRuntime();
            let ctx = ffi::JS_NewContext(rt);
            ffi::JS_SetContextOpaque(ctx, ctx as *mut std::ffi::c_void);

            ffi::JS_NewClassID(&mut FUNCTION_CLASS_ID); // TODO once
            let class_name = CString::new("NativeFunction").unwrap();
            let function_class_def = ffi::JSClassDef {
                class_name: class_name.as_ptr(),
                finalizer: Some(native_function_finalizer),
                gc_mark: None,
                call: Some(native_function_call),
                exotic: std::ptr::null_mut(),
            };
            ffi::JS_NewClass(rt, FUNCTION_CLASS_ID, &function_class_def);

            return QjsContext { rt, ctx };
        }
    }

    fn get(ctx: *mut ffi::JSContext) -> &'static QjsContext {
        unsafe {
            let ptr = ffi::JS_GetContextOpaque(ctx) as *mut QjsContext;
            return &*ptr;
        }
    }

    fn get_error(&self) -> Error {
        let val;
        unsafe {
            val = ffi::JS_GetException(self.ctx);
        }
        Error {
            val: Box::new(value_from_qjs(self, val)),
        }
    }
}

impl Context for QjsContext {
    fn eval(&self, source: &str, sourceurl: &str) -> Result<Value, Error> {
        let source_size = source.len();
        let source = CString::new(source).unwrap();
        let source_url = CString::new(sourceurl).unwrap();
        let res;
        unsafe {
            res = ffi::JS_Eval(
                self.ctx,
                source.as_ptr(),
                source_size,
                source_url.as_ptr(),
                ffi::JS_EVAL_TYPE_GLOBAL as i32,
            );
            if ffi::JS_IsException_noinl(res) == 1 {
                return Err(self.get_error());
            }
        }
        Ok(value_from_qjs(&self, res))
    }

    // String

    fn string_make_from_utf8(&self, rs_str: &str) -> JsString {
        let qjs_str = Box::new(QjsString::new(rs_str.to_owned()));
        JsString {
            ptr: Pointer {
                ctx: self,
                data: qjs_str,
            },
        }
    }

    fn string_to_utf8(&self, jsi_str: &JsString) -> String {
        let qjs_str = jsi_str.ptr.data.downcast_ref::<QjsString>().unwrap();
        qjs_str.str.clone()
    }

    // Value

    fn value_make_bool(&self, val: bool) -> Value {
        unsafe {
            let qjs_val = ffi::JS_NewBool_noinl(self.ctx, val as i32);
            value_from_qjs(&self, qjs_val)
        }
    }

    fn value_make_number(&self, val: f64) -> Value {
        unsafe {
            let qjs_val = ffi::JS_NewFloat64_noinl(self.ctx, val);
            value_from_qjs(&self, qjs_val)
        }
    }

    fn value_get_type(&self, val: &Value) -> ValueType {
        let qjs_val = value_to_qjs(val);
        unsafe {
            if ffi::JS_IsBool_noinl(qjs_val) == 1 {
                return ValueType::Bool;
            }
            if ffi::JS_IsNumber(qjs_val) == 1 {
                return ValueType::Number;
            }
            if (ffi::JS_IsNull_noinl(qjs_val)) == 1 {
                return ValueType::Null;
            }
            if (ffi::JS_IsUndefined_noinl(qjs_val)) == 1 {
                return ValueType::Undefined;
            }
            if (ffi::JS_IsString_noinl(qjs_val)) == 1 {
                return ValueType::String;
            }
            if (ffi::JS_IsObject_noinl(qjs_val)) == 1 {
                return ValueType::Object;
            }
        }
        ValueType::Unknown
    }

    fn value_to_bool(&self, val: &Value) -> Result<bool, Error> {
        let res;
        unsafe {
            res = ffi::JS_ToBool(self.ctx, value_to_qjs(val));
        }
        Ok(res == 1)
    }

    fn value_to_number(&self, val: &Value) -> Result<f64, Error> {
        let mut num = 0.0_f64;
        let res;
        unsafe {
            res = ffi::JS_ToFloat64(self.ctx, &mut num, value_to_qjs(val));
        }
        if res == -1 {
            return Err(self.get_error());
        }
        Ok(num)
    }

    fn value_to_object<'a>(
        &self,
        val: &Value<'a>,
    ) -> Result<Object<'a>, Error> {
        Ok(Object {
            ptr: val.ptr.clone(),
        })
    }

    // Object

    fn object_make_func<'a>(&self, func: Function<'a>) -> Object {
        let qjs_obj;
        let func_ptr = Box::into_raw(Box::new(func));
        unsafe {
            qjs_obj =
                ffi::JS_NewObjectClass(self.ctx, FUNCTION_CLASS_ID as i32);
            ffi::JS_SetOpaque(qjs_obj, func_ptr as *mut std::ffi::c_void);
        }
        object_from_qjs(self, qjs_obj)
    }

    fn object_get_prop(
        &self,
        obj: &Object,
        prop: &str,
    ) -> Result<Value<'_>, Error> {
        let cprop = CString::new(prop).unwrap();
        let res;
        unsafe {
            res = ffi::JS_GetPropertyStr(
                self.ctx,
                object_to_qjs(obj),
                cprop.as_ptr(),
            );
            if ffi::JS_IsException_noinl(res) == 1 {
                return Err(self.get_error());
            }
        }
        Ok(value_from_qjs(&self, res))
    }

    fn object_set_prop(
        &self,
        obj: &Object,
        prop: &str,
        val: &Value,
    ) -> Result<(), Error> {
        let cprop = CString::new(prop).unwrap();
        let res;
        unsafe {
            res = ffi::JS_SetPropertyStr(
                self.ctx,
                object_to_qjs(obj),
                cprop.as_ptr(),
                value_to_qjs(val),
            );
        }
        if res == -1 {
            return Err(self.get_error());
        }
        Ok(())
    }

    fn object_call_as_function<'a>(
        &self,
        object: &Object,
        this: Option<&Value>,
        args: &Vec<Value>,
    ) -> Result<Value, Error> {
        let qjs_object = object_to_qjs(object);
        let qjs_this = match this {
            Some(val) => value_to_qjs(val),
            None => qjs_null(),
        };
        let mut qjs_args = Vec::new();
        for arg in args {
            qjs_args.push(value_to_qjs(arg));
        }
        let qjs_res;
        unsafe {
            qjs_res = ffi::JS_Call(
                self.ctx,
                qjs_object,
                qjs_this,
                args.len() as i32,
                qjs_args.as_mut_ptr(),
            );
        }
        // TODO check err
        Ok(value_from_qjs(self, qjs_res))
    }
}
