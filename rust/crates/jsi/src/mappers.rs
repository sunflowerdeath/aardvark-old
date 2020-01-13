use crate::jsi::*;
use std::sync::Arc;

pub trait Mapper<T> {
    fn from_js(&self, ctx: &dyn Context, val: Value) -> T;

    /*
    fn try_from_js(
        &self,
        ctx: &dyn Context,
        val: Value,
        err_params: &CheckErrorParams,
    ) -> Result<T, String>;
    */

    fn to_js(&self, ctx: &dyn Context, val: &T) -> Value;
}

struct FnMapper<T> {
    from_js: Box<dyn Fn(&dyn Context, Value) -> T>,
    to_js: Box<dyn Fn(&dyn Context, &T) -> Value>,
}

unsafe impl<T> Sync for FnMapper<T> {}
unsafe impl<T> Send for FnMapper<T> {}

impl<T> Mapper<T> for FnMapper<T> {
    fn from_js(&self, ctx: &dyn Context, val: Value) -> T {
        self.from_js(ctx, val)
    }

    fn to_js(&self, ctx: &dyn Context, val: &T) -> Value {
        self.to_js(ctx, val)
    }
}

fn f64_from_js(ctx: &dyn Context, val: Value) -> f64 {
    val.to_number().unwrap()
}

fn f64_to_js(ctx: &dyn Context, val: &f64) -> Value {
    ctx.value_make_number(*val)
}

fn i64_to_js(ctx: &dyn Context, val: &i64) -> Value {
    ctx.value_make_number(*val as f64)
}

fn i64_from_js(_ctx: &dyn Context, val: Value) -> i64 {
    val.to_number().unwrap() as i64
}

fn str_from_js(_ctx: &dyn Context, val: Value) -> String {
    val.to_string().unwrap().to_utf8()
}

fn str_to_js(ctx: &dyn Context, val: &String) -> Value {
    ctx.value_make_string(&ctx.string_make_from_utf8(val))
}

lazy_static! {
    pub static ref I64_MAPPER: Arc<dyn Mapper<i64> + Send + Sync> =
        Arc::new(FnMapper {
            from_js: Box::new(i64_from_js),
            to_js: Box::new(i64_to_js),
        });
    pub static ref STR_MAPPER: Arc<dyn Mapper<String> + Send + Sync> =
        Arc::new(FnMapper {
            from_js: Box::new(str_from_js),
            to_js: Box::new(str_to_js),
        });
}

struct StructMapper<T> {
    from_js: Box<dyn Fn(&dyn Context, Value) -> Result<T, Error>>,
    // to_js: Rc<dyn Fn(&dyn Context, &T) -> Value>,
}

macro_rules! struct_mapper {
    (
        $STRUCT_TYPE:ident {
            $(
                $NAME:ident: $TYPE:ty => $MAPPER:expr,
            )+
        }
    ) => {
        paste::expr! {
            $( let [<$NAME _MAPPER_CLONE>] = $MAPPER.clone(); )+
            let from_js = move |ctx: &dyn Context, val: Value| {
                let obj = val.to_object().unwrap();
                let mut res = $STRUCT_TYPE{
                    ..Default::default()
                };
                $(
                    let prop_val = obj.get_prop(stringify!($NAME)).unwrap();
                    res.$NAME = [<$NAME _MAPPER_CLONE>].from_js(ctx, prop_val);
                )*
                Ok(res)
            };
            StructMapper{
                from_js: Box::new(from_js)
            }
        }
    };
}

#[derive(Default)]
struct Test {
    num: i64,
    str: String,
}

pub fn test() {
    // let f64_mapper = Rc::new(FnMapper {
    // from_js: Box::new(f64_from_js),
    // to_js: Box::new(f64_to_js),
    // });
    let a = struct_mapper!(Test {
        num: i64 => I64_MAPPER,
        str: String => STR_MAPPER,
    });
    let b = struct_mapper!(Test {
        num: i64 => I64_MAPPER,
        str: String => STR_MAPPER,
    });
}
