use downcast_rs;
use std::collections::HashMap;
use std::fmt;
use std::rc::Rc;

#[derive(PartialEq, Debug)]
pub enum ValueType {
    Null,
    Undefined,
    Bool,
    Number,
    String,
    Object,
    Unknown,
}

pub trait PointerData: downcast_rs::Downcast {
    fn box_clone(&self) -> Box<dyn PointerData>;
}
downcast_rs::impl_downcast!(PointerData);

impl Clone for Box<dyn PointerData> {
    fn clone(&self) -> Box<dyn PointerData> {
        self.box_clone()
    }
}

pub struct Error {
    pub val: Box<Value>,
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Unhandled JavaScript exception")
    }
}

#[derive(Clone)]
pub struct Pointer {
    pub(crate) ctx: *const dyn Context,
    pub(crate) data: Box<dyn PointerData>,
}

impl Pointer {
    fn get_ctx(&self) -> &dyn Context {
        unsafe { &*self.ctx }
    }
}

#[derive(Clone)]
pub struct JsString {
    pub(crate) ptr: Pointer,
}

impl<'a> JsString {
    pub fn to_utf8(&self) -> String {
        return self.ptr.get_ctx().string_to_utf8(self);
    }
}

#[derive(Clone)]
pub struct Value {
    pub(crate) ptr: Pointer,
}

impl Value {
    pub fn get_type(&self) -> ValueType {
        self.ptr.get_ctx().value_get_type(self)
    }

    pub fn to_bool(&self) -> Result<bool, Error> {
        self.ptr.get_ctx().value_to_bool(self)
    }

    pub fn to_number(&self) -> Result<f64, Error> {
        self.ptr.get_ctx().value_to_number(self)
    }

    pub fn to_string(&self) -> Result<JsString, Error> {
        self.ptr.get_ctx().value_to_string(self)
    }

    pub fn to_object(&self) -> Result<Object, Error> {
        self.ptr.get_ctx().value_to_object(self)
    }
}

pub type Function =
    Rc<dyn Fn(&dyn Context, Value, Vec<Value>) -> Result<Value, Error>>;

pub type ClassFinalizer = Rc<dyn Fn(Object)>;

pub type ClassPropertyGetter =
    Rc<dyn Fn(&dyn Context, Object) -> Result<Value, Error>>;

pub type ClassPropertySetter =
    Rc<dyn Fn(&dyn Context, Object, Value) -> Result<(), Error>>;

pub struct ClassPropertyDefinition {
    pub get: Option<ClassPropertyGetter>,
    pub set: Option<ClassPropertySetter>,
}

#[derive(Default)]
pub struct ClassDefinition {
    pub name: String,
    pub methods: HashMap<String, Function>,
    pub props: HashMap<String, ClassPropertyDefinition>,
    pub finalizer: Option<ClassFinalizer>,
}

#[derive(Clone)]
pub struct Class {
    pub(crate) ptr: Pointer,
}

#[derive(Clone)]
pub struct Object {
    pub(crate) ptr: Pointer,
}

impl Object {
    pub fn to_value(&self) -> Value {
        self.ptr.get_ctx().object_to_value(self)
    }

    pub fn set_private_data(&self, data: *mut std::ffi::c_void) {
        self.ptr.get_ctx().object_set_private_data(self, data)
    }

    pub fn get_private_data(&self) -> *mut std::ffi::c_void {
        self.ptr.get_ctx().object_get_private_data(self)
    }

    /*
    pub fn object_get_prop_names(&self) -> Result<Vec<String>, Error> {
        self.ptr.ctx.object_get_prop_names(self)
    }
    */

    pub fn object_has_prop(&self, prop: &str) -> bool {
        self.ptr.get_ctx().object_has_prop(self, prop)
    }

    pub fn get_prop(&self, prop: &str) -> Result<Value, Error> {
        self.ptr.get_ctx().object_get_prop(self, prop)
    }

    pub fn set_prop(&self, prop: &str, val: &Value) -> Result<(), Error> {
        self.ptr.get_ctx().object_set_prop(self, prop, val)
    }

    pub fn is_function(&self) -> bool {
        self.ptr.get_ctx().object_is_function(self)
    }

    pub fn call_as_function(
        &self, this: Option<&Value>, args: &Vec<Value>,
    ) -> Result<Value, Error> {
        self.ptr.get_ctx().object_call_as_function(self, this, args)
    }
}

pub trait Context {
    fn eval(&self, source: &str, sourceurl: &str) -> Result<Value, Error>;
    // fn garbage_collect(&self);
    fn get_global_object(&self) -> Object;

    fn string_make_from_utf8(&self, rs_str: &str) -> JsString;
    fn string_to_utf8(&self, jsi_str: &JsString) -> String;

    // Value
    fn value_make_null(&self) -> Value;
    fn value_make_undefined(&self) -> Value;
    fn value_make_bool(&self, val: bool) -> Value;
    fn value_make_number(&self, val: f64) -> Value;
    fn value_make_string(&self, str: &JsString) -> Value;
    // fn value_make_object(&self, obj: &Object) -> Value;

    fn value_get_type(&self, val: &Value) -> ValueType;

    fn value_to_bool(&self, val: &Value) -> Result<bool, Error>;
    fn value_to_number(&self, val: &Value) -> Result<f64, Error>;
    fn value_to_string(&self, val: &Value) -> Result<JsString, Error>;
    fn value_to_object(&self, val: &Value) -> Result<Object, Error>;

    // fn value_strict_equal_to(&self, a: &Value, b: &Value) -> bool;

    // Class
    fn class_make(&self, def: &ClassDefinition) -> Class;

    // Object
    fn object_make(&self, class: Option<&Class>) -> Object;
    fn object_make_func(&self, func: Function) -> Object;
    // object_make_constructor(const Class& js_class) -> Object;
    object_make_array() -> Object;

    fn object_to_value(&self, obj: &Object) -> Value;

    fn object_set_private_data(
        &self, obj: &Object, data: *mut std::ffi::c_void,
    );
    fn object_get_private_data(&self, obj: &Object) -> *mut std::ffi::c_void;

    // fn object_get_own_prop_names(&self, obj: &Object, prop: &str)
    //      -> Result<Vec<String>, Error>;
    fn object_has_prop(&self, obj: &Object, prop: &str) -> bool;
    fn object_get_prop(&self, obj: &Object, prop: &str)
        -> Result<Value, Error>;
    fn object_set_prop(
        &self, obj: &Object, prop: &str, val: &Value,
    ) -> Result<(), Error>;
    fn object_delete_prop(&self, obj: &Object, prop: &str)
        -> Result<(), Error>;

    fn object_is_function(&self, obj: &Object) -> bool;
    fn object_call_as_function(
        &self, func: &Object, this: Option<&Value>, args: &Vec<Value>,
    ) -> Result<Value, Error>;

    fn object_is_constructor(&self, obj: &Object) -> bool;
    // fn object_call_as_constructor(
    // &self,
    // func: &Object,
    // args: &Vec<Value>,
    // ) -> Result<Value, Error>;

    // fn object_is_array(&self, obj: &Object) -> bool;
    // fn object_get_prop_at_index(&self, obj: &Object, idx: u32)
    // -> Result<Value, Error>;
    // fn object_set_prop(
    // &self,
    // obj: &Object,
    // idx: u32,
    // val: &Value,
    // ) -> Result<(), Error>;
}
