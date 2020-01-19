pub mod ffi;

use crate::jsi::*;
use std::cell::RefCell;
use std::collections::HashMap;
use std::ffi::{CStr, CString};
use std::mem::MaybeUninit;
use std::rc::Rc;
use std::sync::Once;

// QjsString
#[derive(Clone)]
struct QjsString {
    str: String,
}

impl PointerData for QjsString {
    fn box_clone(&self) -> Box<dyn PointerData> {
        Box::new(self.clone())
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
        Box::new(self.clone())
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
    is_finalizing: bool,
}

impl Clone for QjsValue {
    fn clone(&self) -> QjsValue {
        unsafe { ffi::JS_DupValue_NOINL(self.ctx, self.val) };
        QjsValue {
            ctx: self.ctx,
            val: self.val,
            is_finalizing: self.is_finalizing,
        }
    }
}

impl Drop for QjsValue {
    fn drop(&mut self) {
        if !self.is_finalizing {
            unsafe { ffi::JS_FreeValue_NOINL(self.ctx, self.val) };
        }
    }
}

impl PointerData for QjsValue {
    fn box_clone(&self) -> Box<dyn PointerData> {
        Box::new(self.clone())
    }
}

impl QjsValue {
    fn new(ctx: &QjsContext, qjs_val: ffi::JSValue) -> QjsValue {
        QjsValue::new_full(ctx, qjs_val, false)
    }

    fn new_full(
        ctx: &QjsContext, qjs_val: ffi::JSValue, is_finalizing: bool,
    ) -> QjsValue {
        QjsValue {
            ctx: ctx.ctx,
            val: qjs_val,
            is_finalizing,
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

fn class_to_qjs(cls: &Class) -> u32 {
    cls.ptr.data.downcast_ref::<QjsClass>().unwrap().id
}

fn string_to_utf8(jsi_str: &JsString) -> &str {
    let qjs_str = jsi_str.ptr.data.downcast_ref::<QjsString>().unwrap();
    &qjs_str.str
}

// NativeFunction
static mut FUNCTION_CLASS_ID: u32 = 0;

extern "C" fn native_function_finalizer(
    _: *mut ffi::JSRuntime, val: ffi::JSValue,
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
    ctx: *mut ffi::JSContext, func_obj: ffi::JSValue, this_val: ffi::JSValue,
    argc: i32, argv: *mut ffi::JSValue, _flags: i32,
) -> ffi::JSValue {
    unsafe {
        let func =
            ffi::JS_GetOpaque(func_obj, FUNCTION_CLASS_ID) as *mut Function;
        let jsi_ctx = QjsContext::get(ctx);
        ffi::JS_DupValue_NOINL(ctx, this_val);
        let jsi_this = value_from_qjs(jsi_ctx, this_val);
        let mut jsi_args = Vec::new();
        for i in 0..argc {
            let qjs_arg = *argv.offset(i as isize);
            ffi::JS_DupValue_NOINL(ctx, qjs_arg);
            jsi_args.push(value_from_qjs(jsi_ctx, qjs_arg));
        }
        let res = (*func)(jsi_ctx, jsi_this, jsi_args);
        match res {
            Ok(val) => {
                let qjs_res = value_to_qjs(&val);
                ffi::JS_DupValue_NOINL(ctx, qjs_res);
                return qjs_res;
            }
            Err(err) => {
                let qjs_ex = value_to_qjs(err.val.as_ref());
                ffi::JS_DupValue_NOINL(ctx, qjs_ex);
                return ffi::JS_Throw(ctx, qjs_ex);
            }
        }
    }
}

// Class

// This map stores pointers to all contexts. It is needed because class
// finalizers do not get the ctx argument.
static mut RUNTIMES_MAP: MaybeUninit<
    HashMap<*mut ffi::JSRuntime, *mut QjsContext>,
> = MaybeUninit::uninit();

// This is how to get simple mutable global variable in rust...
fn init_runtimes_map() {
    static mut ONCE: Once = Once::new();
    unsafe {
        ONCE.call_once(|| RUNTIMES_MAP.as_mut_ptr().write(HashMap::new()));
    }
}

extern "C" fn class_finalizer(rt: *mut ffi::JSRuntime, val: ffi::JSValue) {
    // Yeah, rust is readable and safe
    let ctx = unsafe { &**(*RUNTIMES_MAP.as_ptr()).get(&rt).unwrap() };
    let ptr = unsafe { ffi::JS_ValueGetPtr(val) };
    if let Some(cls_id) = ctx.class_instances.borrow_mut().remove(&ptr) {
        if let Some(finalizer) = ctx.class_finalizers.borrow().get(&cls_id) {
            let obj = Object {
                ptr: Pointer {
                    ctx,
                    data: Box::new(QjsValue::new_full(ctx, val, true)),
                },
            };
            finalizer(obj);
        }
    }
}

// QjsContext
pub struct QjsContext {
    rt: *mut ffi::JSRuntime,
    ctx: *mut ffi::JSContext,
    class_finalizers: RefCell<HashMap<ffi::JSClassID, ClassFinalizer>>,

    // This map stores all class instances. That is needed to implement
    // `get_private_data` without passing class argument.
    // Map key is actual value pointer instead of JSValue.
    class_instances: RefCell<HashMap<*mut std::ffi::c_void, ffi::JSClassID>>,
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
        let ctx = QjsContext::create();
        QjsContext::init(&ctx);
        ctx
    }

    fn create() -> Rc<QjsContext> {
        init_runtimes_map();
        let qjs_rt = unsafe { ffi::JS_NewRuntime() };
        let qjs_ctx = unsafe { ffi::JS_NewContext(qjs_rt) };
        let mut ctx = Rc::new(QjsContext {
            rt: qjs_rt,
            ctx: qjs_ctx,
            class_finalizers: RefCell::new(HashMap::new()),
            class_instances: RefCell::new(HashMap::new()),
        });
        unsafe {
            let ctx_ptr = Rc::get_mut(&mut ctx).unwrap() as *mut _;
            ffi::JS_SetContextOpaque(qjs_ctx, ctx_ptr as *mut std::ffi::c_void);
            (&mut *RUNTIMES_MAP.as_mut_ptr()).insert(ctx.rt, ctx_ptr);
        }
        ctx
    }

    pub fn init(ctx: &Rc<QjsContext>) {
        unsafe {
            ffi::JS_NewClassID(&mut FUNCTION_CLASS_ID); // TODO once?
            let class_name = CString::new("NativeFunction").unwrap();
            let function_class_def = ffi::JSClassDef {
                class_name: class_name.as_ptr(),
                finalizer: Some(native_function_finalizer),
                gc_mark: None,
                call: Some(native_function_call),
                exotic: std::ptr::null_mut(),
            };
            ffi::JS_NewClass(ctx.rt, FUNCTION_CLASS_ID, &function_class_def);
        }
    }

    fn get(ctx: *mut ffi::JSContext) -> &'static QjsContext {
        unsafe {
            let ptr = ffi::JS_GetContextOpaque(ctx) as *mut QjsContext;
            &*ptr
        }
    }

    fn get_error(&self) -> Error {
        let val = unsafe { ffi::JS_GetException(self.ctx) };
        Error {
            val: Box::new(value_from_qjs(self, val)),
        }
    }
}

impl Context for QjsContext {
    fn eval(&self, source: &str, source_url: &str) -> Result<Value, Error> {
        let c_source = CString::new(source).unwrap();
        let c_source_url = CString::new(source_url).unwrap();
        let res = unsafe {
            ffi::JS_Eval(
                self.ctx,
                c_source.as_ptr(),
                source.len(),
                c_source_url.as_ptr(),
                ffi::JS_EVAL_TYPE_GLOBAL as i32,
            )
        };
        if unsafe { ffi::JS_IsException_NOINL(res) } == 1 {
            return Err(self.get_error());
        }
        Ok(value_from_qjs(&self, res))
    }

    fn get_global_object(&self) -> Object {
        object_from_qjs(self, unsafe { ffi::JS_GetGlobalObject(self.ctx) })
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
        let qjs_val = unsafe { ffi::JS_NewBool_NOINL(self.ctx, val as i32) };
        value_from_qjs(self, qjs_val)
    }

    fn value_make_number(&self, val: f64) -> Value {
        let qjs_val = unsafe { ffi::JS_NewFloat64_NOINL(self.ctx, val) };
        value_from_qjs(self, qjs_val)
    }

    fn value_make_string(&self, str: &JsString) -> Value {
        let c_str = CString::new(string_to_utf8(str)).unwrap();
        let val = unsafe { ffi::JS_NewString(self.ctx, c_str.as_ptr()) };
        value_from_qjs(self, val)
    }

    fn value_get_type(&self, val: &Value) -> ValueType {
        let qjs_val = value_to_qjs(val);
        unsafe {
            if ffi::JS_IsBool_NOINL(qjs_val) == 1 {
                return ValueType::Bool;
            }
            if ffi::JS_IsNumber(qjs_val) == 1 {
                return ValueType::Number;
            }
            if (ffi::JS_IsNull_NOINL(qjs_val)) == 1 {
                return ValueType::Null;
            }
            if (ffi::JS_IsUndefined_NOINL(qjs_val)) == 1 {
                return ValueType::Undefined;
            }
            if (ffi::JS_IsString_NOINL(qjs_val)) == 1 {
                return ValueType::String;
            }
            if (ffi::JS_IsObject_NOINL(qjs_val)) == 1 {
                return ValueType::Object;
            }
        }
        ValueType::Unknown
    }

    fn value_to_bool(&self, val: &Value) -> Result<bool, Error> {
        let res = unsafe { ffi::JS_ToBool(self.ctx, value_to_qjs(val)) };
        Ok(res == 1)
    }

    fn value_to_number(&self, val: &Value) -> Result<f64, Error> {
        let mut num = 0.0_f64;
        let res =
            unsafe { ffi::JS_ToFloat64(self.ctx, &mut num, value_to_qjs(val)) };
        if res == -1 {
            return Err(self.get_error());
        }
        Ok(num)
    }

    fn value_to_string(&self, val: &Value) -> Result<JsString, Error> {
        let res;
        unsafe {
            let c_str = ffi::JS_ToCString_NOINL(self.ctx, value_to_qjs(val));
            // TODO check error
            res = CStr::from_ptr(c_str).to_str().unwrap().to_owned();
        }
        Ok(JsString {
            ptr: Pointer {
                ctx: self,
                data: Box::new(QjsString::new(res)),
            },
        })
    }

    fn value_to_object(&self, val: &Value) -> Result<Object, Error> {
        Ok(Object {
            ptr: val.ptr.clone(),
        })
    }

    // Class
    fn class_make(&self, def: &ClassDefinition) -> Class {
        let mut class_id = 0;
        unsafe { ffi::JS_NewClassID(&mut class_id) };
        let c_name = CString::new(def.name.clone()).unwrap();
        let class_def = ffi::JSClassDef {
            class_name: c_name.as_ptr(),
            finalizer: Some(class_finalizer),
            call: None,
            gc_mark: None,
            exotic: std::ptr::null_mut(),
        };
        unsafe { ffi::JS_NewClass(self.rt, class_id, &class_def) };

        if let Some(finalizer) = def.finalizer.as_ref() {
            self.class_finalizers
                .borrow_mut()
                .insert(class_id, finalizer.clone());
        }

        let proto = unsafe { ffi::JS_NewObject(self.ctx) };
        for (name, prop) in &def.props {
            let c_name = CString::new(name.clone()).unwrap();
            let atom = unsafe {
                ffi::JS_NewAtomLen(self.ctx, c_name.as_ptr(), name.len())
            };
            let get_val = match prop.get.as_ref() {
                Some(get) => {
                    let get = get.clone();
                    let func = self.object_make_func(Rc::new(
                        move |ctx: &dyn Context,
                              this: Value,
                              _args: Vec<Value>| {
                            let obj = this.to_object().unwrap();
                            get(ctx, obj)
                        },
                    ));
                    let val = object_to_qjs(&func);
                    unsafe { ffi::JS_DupValue_NOINL(self.ctx, val) };
                    val
                }
                None => qjs_null(),
            };
            let set_val = match prop.set.as_ref() {
                Some(set) => {
                    let set = set.clone();
                    let func = self.object_make_func(Rc::new(
                        move |ctx: &dyn Context,
                              this: Value,
                              args: Vec<Value>| {
                            let obj = this.to_object().unwrap();
                            if let Err(error) = set(ctx, obj, args[0].clone()) {
                                return Err(error);
                            }
                            Ok(ctx.value_make_undefined())
                        },
                    ));
                    let val = object_to_qjs(&func);
                    unsafe { ffi::JS_DupValue_NOINL(self.ctx, val) };
                    val
                }
                None => qjs_null(),
            };
            unsafe {
                ffi::JS_DefinePropertyGetSet(
                    self.ctx, proto, atom, get_val, set_val, 0,
                );
                ffi::JS_FreeAtom(self.ctx, atom);
            }
        }

        for (name, method) in &def.methods {
            let val = self.object_make_func(method.clone());
            let c_name = CString::new(name.clone()).unwrap();
            unsafe {
                ffi::JS_DupValue_NOINL(self.ctx, object_to_qjs(&val));
                ffi::JS_DefinePropertyValueStr(
                    self.ctx,
                    proto,
                    c_name.as_ptr(),
                    object_to_qjs(&val),
                    ffi::JS_PROP_ENUMERABLE as i32,
                );
            }
        }
        unsafe { ffi::JS_SetClassProto(self.ctx, class_id, proto) };
        Class {
            ptr: Pointer {
                ctx: self,
                data: Box::new(QjsClass::new(class_id)),
            },
        }
    }

    // Object
    fn object_make(&self, class: Option<&Class>) -> Object {
        let qjs_obj = unsafe {
            match class {
                Some(class) => {
                    ffi::JS_NewObjectClass(self.ctx, class_to_qjs(class) as i32)
                }
                None => ffi::JS_NewObject(self.ctx),
            }
        };
        if let Some(class) = class {
            let ptr = unsafe { ffi::JS_ValueGetPtr(qjs_obj) };
            self.class_instances
                .borrow_mut()
                .insert(ptr, class_to_qjs(class) as u32);
        }
        object_from_qjs(self, qjs_obj)
    }

    fn object_make_func(&self, func: Function) -> Object {
        let func_ptr = Box::into_raw(Box::new(func));
        let qjs_obj;
        unsafe {
            qjs_obj =
                ffi::JS_NewObjectClass(self.ctx, FUNCTION_CLASS_ID as i32);
            ffi::JS_SetOpaque(qjs_obj, func_ptr as *mut std::ffi::c_void);
        }
        object_from_qjs(self, qjs_obj)
    }

    fn object_make_array(&self) -> Object {
        let qjs_arr = unsafe { ffi::JS_NewArray(self.ctx) };
        object_from_qjs(self, qjs_arr)
    }

    fn object_to_value(&self, obj: &Object) -> Value {
        Value {
            ptr: obj.ptr.clone(),
        }
    }

    fn object_set_private_data(
        &self, obj: &Object, data: *mut std::ffi::c_void,
    ) {
        unsafe { ffi::JS_SetOpaque(object_to_qjs(obj), data) };
    }

    fn object_get_private_data(&self, obj: &Object) -> *mut std::ffi::c_void {
        let qjs_value = object_to_qjs(obj);
        let ptr = unsafe { ffi::JS_ValueGetPtr(qjs_value) };
        match self.class_instances.borrow().get(&ptr) {
            Some(cls_id) => unsafe { ffi::JS_GetOpaque(qjs_value, *cls_id) },
            None => std::ptr::null_mut(),
        }
    }

    fn object_has_prop(&self, obj: &Object, prop: &str) -> bool {
        let cprop = CString::new(prop).unwrap();
        let res;
        unsafe {
            let atom = ffi::JS_NewAtomLen(self.ctx, cprop.as_ptr(), prop.len());
            res = ffi::JS_HasProperty(self.ctx, object_to_qjs(obj), atom);
            ffi::JS_FreeAtom(self.ctx, atom);
        }
        res == 1
    }

    fn object_get_prop(
        &self, obj: &Object, prop: &str,
    ) -> Result<Value, Error> {
        let cprop = CString::new(prop).unwrap();
        let res = unsafe {
            ffi::JS_GetPropertyStr(self.ctx, object_to_qjs(obj), cprop.as_ptr())
        };
        if unsafe { ffi::JS_IsException_NOINL(res) } == 1 {
            return Err(self.get_error());
        }
        Ok(value_from_qjs(&self, res))
    }

    fn object_set_prop(
        &self, obj: &Object, prop: &str, val: &Value,
    ) -> Result<(), Error> {
        let qjs_val = value_to_qjs(val);
        let cprop = CString::new(prop).unwrap();
        let res = unsafe {
            ffi::JS_SetPropertyStr(
                self.ctx,
                object_to_qjs(obj),
                cprop.as_ptr(),
                qjs_val,
            )
        };
        if res == -1 {
            return Err(self.get_error());
        }
        unsafe { ffi::JS_DupValue_NOINL(self.ctx, qjs_val) };
        Ok(())
    }

    fn object_delete_prop(
        &self, obj: &Object, prop: &str,
    ) -> Result<(), Error> {
        let cprop = CString::new(prop).unwrap();
        let res;
        unsafe {
            let atom = ffi::JS_NewAtomLen(self.ctx, cprop.as_ptr(), prop.len());
            res = ffi::JS_DeleteProperty(
                self.ctx,
                object_to_qjs(obj),
                atom,
                ffi::JS_PROP_THROW as i32,
            );
            ffi::JS_FreeAtom(self.ctx, atom);
        }
        if res == -1 {
            return Err(self.get_error());
        }
        Ok(())
    }

    fn object_is_function(&self, obj: &Object) -> bool {
        unsafe { ffi::JS_IsFunction(self.ctx, object_to_qjs(obj)) == 1 }
    }

    fn object_call_as_function(
        &self, object: &Object, this: Option<&Value>, args: &Vec<Value>,
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
        let qjs_res = unsafe {
            ffi::JS_Call(
                self.ctx,
                qjs_object,
                qjs_this,
                args.len() as i32,
                qjs_args.as_mut_ptr(),
            )
        };
        if unsafe { ffi::JS_IsException_NOINL(qjs_res) } == 1 {
            return Err(self.get_error());
        }
        Ok(value_from_qjs(self, qjs_res))
    }

    fn object_is_constructor(&self, obj: &Object) -> bool {
        unsafe { ffi::JS_IsConstructor(self.ctx, object_to_qjs(obj)) == 1 }
    }

    fn object_is_array(&self, obj: &Object) -> bool {
        unsafe {
            ffi::JS_IsArray(self.ctx, object_to_qjs(obj)) == 1
        }
    }
}
