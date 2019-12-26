pub mod ffi;

use crate::jsi;
use std::alloc::{dealloc, Layout};
use std::ffi::CString;
use std::ptr;

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

impl jsi::PointerData for QjsValue {
    fn box_clone(&self) -> Box<dyn jsi::PointerData> {
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

fn qjs_null() -> ffi::JSValue {
    ffi::JSValue {
        u: ffi::JSValueUnion { int32: 0 },
        tag: ffi::JS_TAG_NULL as i64,
    }
}

fn value_from_qjs(ctx: &QjsContext, qjs_val: ffi::JSValue) -> jsi::Value {
    jsi::Value {
        ptr: jsi::Pointer {
            ctx,
            data: Box::new(QjsValue::new(ctx, qjs_val)),
        },
    }
}

fn object_from_qjs(ctx: &QjsContext, qjs_val: ffi::JSValue) -> jsi::Object {
    jsi::Object {
        ptr: jsi::Pointer {
            ctx,
            data: Box::new(QjsValue::new(ctx, qjs_val)),
        },
    }
}

fn pointer_to_qjs(ptr: &jsi::Pointer) -> ffi::JSValue {
    ptr.data.downcast_ref::<QjsValue>().unwrap().val
}

fn value_to_qjs(val: &jsi::Value) -> ffi::JSValue {
    pointer_to_qjs(&val.ptr)
}

fn object_to_qjs(obj: &jsi::Object) -> ffi::JSValue {
    pointer_to_qjs(&obj.ptr)
}

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

static mut FUNCTION_CLASS_ID: u32 = 0;

extern "C" fn native_function_finalizer(
    _: *mut ffi::JSRuntime,
    val: ffi::JSValue,
) {
    unsafe {
        let ptr = ffi::JS_GetOpaque(val, 0);
        let _ = Box::from_raw(ptr);
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
        let func = ffi::JS_GetOpaque(func_obj, FUNCTION_CLASS_ID)
            as *mut jsi::Function;
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

    fn get_error(&self) -> jsi::Error {
        let val;
        unsafe {
            val = ffi::JS_GetException(self.ctx);
        }
        jsi::Error {
            val: Box::new(value_from_qjs(self, val)),
        }
    }
}

impl jsi::Context for QjsContext {
    fn eval(&self, source: &str) -> jsi::Value {
        let source_size = source.len();
        let source = CString::new(source).unwrap();
        let source_url = CString::new("SOURCEURL").unwrap(); // TODO arg
        let val;
        unsafe {
            val = ffi::JS_Eval(
                self.ctx,
                source.as_ptr(),
                source_size,
                source_url.as_ptr(),
                ffi::JS_EVAL_TYPE_GLOBAL as i32,
            );
        }
        // TODO check err
        value_from_qjs(&self, val)
    }

    // Value
    fn value_make_bool(&self, val: bool) -> jsi::Value {
        unsafe {
            let qjs_val = ffi::JS_NewBool_noinl(self.ctx, val as i32);
            value_from_qjs(&self, qjs_val)
        }
    }

    fn value_make_number(&self, val: f64) -> jsi::Value {
        unsafe {
            let qjs_val = ffi::JS_NewFloat64_noinl(self.ctx, val);
            value_from_qjs(&self, qjs_val)
        }
    }

    fn value_get_type(&self, val: &jsi::Value) -> jsi::ValueType {
        let qjs_val = value_to_qjs(val);
        unsafe {
            if ffi::JS_IsBool_noinl(qjs_val) == 1 {
                return jsi::ValueType::Bool;
            }
            if ffi::JS_IsNumber(qjs_val) == 1 {
                return jsi::ValueType::Number;
            }
            if (ffi::JS_IsNull_noinl(qjs_val)) == 1 {
                return jsi::ValueType::Null;
            }
            if (ffi::JS_IsUndefined_noinl(qjs_val)) == 1 {
                return jsi::ValueType::Undefined;
            }
            if (ffi::JS_IsString_noinl(qjs_val)) == 1 {
                return jsi::ValueType::String;
            }
            if (ffi::JS_IsObject_noinl(qjs_val)) == 1 {
                return jsi::ValueType::Object;
            }
        }
        jsi::ValueType::Unknown
    }

    fn value_to_bool(&self, val: &jsi::Value) -> Result<bool, jsi::Error> {
        let res;
        unsafe {
            res = ffi::JS_ToBool(self.ctx, value_to_qjs(val));
        }
        Ok(res == 1)
    }

    fn value_to_number(&self, val: &jsi::Value) -> Result<f64, jsi::Error> {
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
        val: &jsi::Value<'a>,
    ) -> Result<jsi::Object<'a>, jsi::Error> {
        Ok(jsi::Object {
            ptr: val.ptr.clone(),
        })
    }

    // Object
    fn object_make_func<'a>(&self, func: jsi::Function<'a>) -> jsi::Object {
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
        obj: &jsi::Object,
        prop: &str,
    ) -> Result<jsi::Value<'_>, jsi::Error> {
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

    fn object_call_as_function<'a>(
        &self,
        object: &jsi::Object,
        this: Option<&jsi::Value>,
        args: &Vec<jsi::Value>,
    ) -> Result<jsi::Value, jsi::Error> {
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
