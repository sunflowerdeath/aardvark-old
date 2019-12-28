pub mod ffi;

use crate::jsi::*;
use std::collections::HashMap;
use std::ffi::CStr;
use std::ffi::CString;
use std::rc::{Rc, Weak};

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
    fn new(str: String) -> QjsString {
        QjsString { str }
    }
}

// QjsClass

#[derive(Clone)]
struct QjsClass {
    id: u32,
}

impl PointerData for QjsClass {
    fn box_clone(&self) -> Box<dyn PointerData> {
        return Box::new(self.clone());
    }
}

impl QjsClass {
    fn new(id: u32) -> QjsClass {
        QjsClass { id }
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

fn qjs_undefined() -> ffi::JSValue {
    ffi::JSValue {
        u: ffi::JSValueUnion { int32: 0 },
        tag: ffi::JS_TAG_UNDEFINED as i64,
    }
}

fn value_from_qjs(ctx: &QjsContext, qjs_val: ffi::JSValue) -> Value {
    Value {
        ptr: Pointer {
            ctx: ctx.weak.clone(),
            data: Box::new(QjsValue::new(ctx, qjs_val)),
        },
    }
}

fn object_from_qjs(
    ctx: &QjsContext,
    qjs_val: ffi::JSValue,
) -> Object {
    Object {
        ptr: Pointer {
            ctx: ctx.weak.clone(),
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

fn string_to_utf8(jsi_str: &JsString) -> &str {
    let qjs_str = jsi_str.ptr.data.downcast_ref::<QjsString>().unwrap();
    &qjs_str.str
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

// Class

extern "C" fn class_finalizer(rt: *mut ffi::JSRuntime, val: ffi::JSValue) {
    /*
    auto it = Qjs_Context::class_instances.find(JS_VALUE_GET_PTR(value));
    if (it == Qjs_Context::class_instances.end()) return;
    auto ctx = it->second.ctx;
    auto class_id = it->second.class_id;
    auto finalizer = ctx->class_finalizers[class_id];
    if (finalizer) finalizer(ctx->object_from_qjs(value, true));
    Qjs_Context::class_instances.erase(it);
    */
}

// QjsContext

pub struct QjsContext {
    rt: *mut ffi::JSRuntime,
    ctx: *mut ffi::JSContext,
    weak: Weak<QjsContext>,
    class_finalizers: HashMap<u32, Option<ClassFinalizer>>,
    class_defs: HashMap<u32, Option<ClassDefinition>>,
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
    pub fn new() -> Rc<QjsContext> {
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

            let ctx = Rc::new(QjsContext {
                rt,
                ctx,
                class_finalizers: HashMap::new(),
                class_defs: HashMap::new(),
                weak: Default::default()
            });
            // ctx.weak = Rc::downgrade(&ctx);
            ctx
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
                ctx: self.weak.clone(),
                data: qjs_str,
            },
        }
    }

    fn string_to_utf8(&self, jsi_str: &JsString) -> String {
        string_to_utf8(jsi_str).to_owned()
    }

    // Value

    fn value_make_null(&self) -> Value {
        value_from_qjs(self, qjs_null())
    }

    fn value_make_undefined(&self) -> Value {
        value_from_qjs(self, qjs_undefined())
    }

    fn value_make_bool(&self, val: bool) -> Value {
        unsafe {
            let qjs_val = ffi::JS_NewBool_noinl(self.ctx, val as i32);
            value_from_qjs(self, qjs_val)
        }
    }

    fn value_make_number(&self, val: f64) -> Value {
        unsafe {
            let qjs_val = ffi::JS_NewFloat64_noinl(self.ctx, val);
            value_from_qjs(self, qjs_val)
        }
    }

    fn value_make_string(&self, str: &JsString) -> Value {
        let c_str = CString::new(string_to_utf8(str)).unwrap();
        unsafe {
            let val = ffi::JS_NewString(self.ctx, c_str.as_ptr());
            value_from_qjs(self, val)
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

    fn value_to_string(&self, val: &Value) -> Result<JsString, Error> {
        let res;
        unsafe {
            let c_str = ffi::JS_ToCString_noinl(self.ctx, value_to_qjs(val));
            // TODO check error
            res = CStr::from_ptr(c_str).to_str().unwrap().to_owned();
        }
        Ok(JsString {
            ptr: Pointer {
                ctx: self.weak.clone(),
                data: Box::new(QjsString::new(res)),
            },
        })
    }

    fn value_to_object(
        &self,
        val: &Value,
    ) -> Result<Object, Error> {
        Ok(Object {
            ptr: val.ptr.clone(),
        })
    }

    // Class

    fn class_make( &mut self, def: &ClassDefinition,) -> Class {
        let mut class_id = 0;
        unsafe {
            ffi::JS_NewClassID(&mut class_id);
        }
        let c_name = CString::new(def.name.clone()).unwrap();
        let class_def = ffi::JSClassDef {
            class_name: c_name.as_ptr(),
            finalizer: Some(class_finalizer),
            call: None,
            gc_mark: None,
            exotic: std::ptr::null_mut(),
        };
        unsafe {
            ffi::JS_NewClass(self.rt, class_id, &class_def);
        }

        self.class_finalizers
            .insert(class_id, def.finalizer.clone());

        let proto;
        unsafe {
            proto = ffi::JS_NewObject(self.ctx);
        }
        for (name, prop) in &def.props {
            let c_name = CString::new(name.clone()).unwrap();
            let atom;
            unsafe {
                atom =
                    ffi::JS_NewAtomLen(self.ctx, c_name.as_ptr(), name.len());
            }
            let get_val = match prop.get.as_ref() {
                Some(get) => {
                    let get_clone = get.clone();
                    let func = self.object_make_func(Rc::new(
                        move |this: Value, _args: Vec<Value>| {
                            let obj = this.to_object().unwrap();
                            get_clone(obj)
                        },
                    ));
                    let val = object_to_qjs(&func);
                    unsafe {
                        ffi::JS_DupValue_noinl(self.ctx, val);
                    }
                    val
                }
                None => qjs_null(),
            };
            /*
            let set_val = match prop.set.as_ref() {
                Some(set) => {
                    let set_clone = set.clone();
                    let func = self.object_make_func(Rc::new(
                        move |this: Value, args: Vec<Value>| {
                            let obj = this.to_object().unwrap();
                            set_clone(obj, args[0]);
                            Ok(self.value_make_undefined())
                        },
                    ));
                    let val = object_to_qjs(&func);
                    unsafe {
                        ffi::JS_DupValue_noinl(self.ctx, val);
                    }
                    val
                }
                None => qjs_null(),
            };
            */
            unsafe {
                ffi::JS_DefinePropertyGetSet(
                    self.ctx, proto, atom, get_val, get_val, 0,
                );
                ffi::JS_FreeAtom(self.ctx, atom);
            }
        }

        for (name, method) in &def.methods {
            let val = self.object_make_func(method.clone());
            let c_name = CString::new(name.clone()).unwrap();
            unsafe {
                ffi::JS_DupValue_noinl(self.ctx, object_to_qjs(&val));
                ffi::JS_DefinePropertyValueStr(
                    self.ctx,
                    proto,
                    c_name.as_ptr(),
                    object_to_qjs(&val),
                    ffi::JS_PROP_ENUMERABLE as i32,
                );
            }
        }
        unsafe {
            ffi::JS_SetClassProto(self.ctx, class_id, proto);
        }

        return Class {
            ptr: Pointer {
                ctx: self.weak.clone(),
                data: Box::new(QjsClass::new(class_id)),
            },
        };
    }

    // Object

    fn object_make_func(&self, func: Function) -> Object {
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
    ) -> Result<Value, Error> {
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

    fn object_call_as_function(
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
