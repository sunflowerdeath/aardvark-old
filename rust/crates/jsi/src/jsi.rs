use downcast_rs;
use std::fmt;

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

pub struct Error<'a> {
    pub val: Box<Value<'a>>,
}

impl<'a> fmt::Debug for Error<'a> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Unhadled JavaScript exception")
    }
}

#[derive(Clone)]
pub struct Pointer<'a> {
    pub(crate) ctx: &'a dyn Context,
    pub(crate) data: Box<dyn PointerData>,
}

#[derive(Clone)]
pub struct Value<'a> {
    pub(crate) ptr: Pointer<'a>,
}

impl<'a> Value<'a> {
    pub fn get_type(&self) -> ValueType {
        return self.ptr.ctx.value_get_type(self);
    }

    pub fn to_bool(&self) -> Result<bool, Error> {
        return self.ptr.ctx.value_to_bool(self);
    }

    pub fn to_number(&self) -> Result<f64, Error> {
        return self.ptr.ctx.value_to_number(self);
    }

    pub fn to_object(&'a self) -> Result<Object, Error> {
        return self.ptr.ctx.value_to_object(self);
    }
}

pub type Hz<'a> = dyn Fn(Value<'a>, Vec<Value<'a>>) -> Result<Value<'a>, Error<'a>>;

pub type Function<'a> = Box<dyn Fn(Value<'a>, Vec<Value<'a>>) -> Result<Value<'a>, Error<'a>> + 'a>;

#[derive(Clone)]
pub struct Object<'a> {
    pub(crate) ptr: Pointer<'a>,
}

impl<'a> Object<'a> {
    pub fn get_prop(&self, prop: &str) -> Result<Value, Error> {
        self.ptr.ctx.object_get_prop(self, prop)
    }

    pub fn call_as_function(&self, this: Option<&Value>, args: &Vec<Value>) 
    -> Result<Value, Error> {
        self.ptr.ctx.object_call_as_function(self, this, args)
    }
}

pub trait Context {
    fn eval(&self, source: &str) -> Value;

    // Value
    // fn value_make_null(&self) -> Value;
    // fn value_make_undefined(&self) -> Value;
    fn value_make_bool(&self, val: bool) -> Value;
    fn value_make_number(&self, val: f64) -> Value;
    // fn value_make_string(&self, str: &String) -> Value;
    // fn value_make_object(&self, obj: &Object) -> Value;

    fn value_get_type(&self, val: &Value) -> ValueType;

    fn value_to_bool(&self, val: &Value) -> Result<bool, Error>;
    fn value_to_number(&self, val: &Value) -> Result<f64, Error>;
    fn value_to_object<'a>(&self, val: &Value<'a>)
        -> Result<Object<'a>, Error>;

    // Object
    // object_make(const Class* js_class) -> Object;
    fn object_make_func<'a>(&self, func: Function<'a>) -> Object;
    // object_make_constructor(const Class& js_class) -> Object;
    // object_make_array() -> Object;

    // fn object_to_value(&self, &Object obj) -> Result<Value, Error>;

    fn object_get_prop(&self, obj: &Object, prop: &str)
        -> Result<Value, Error>;

    fn object_call_as_function<'a>(
        &self,
        func: &Object,
        this: Option<&Value>,
        args: &Vec<Value>,
    ) -> Result<Value, Error>;
}
