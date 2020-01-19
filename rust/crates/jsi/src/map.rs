use crate::check::*;
use crate::jsi::*;
use lazy_static::*;
use std::sync::Arc;

pub trait Mapper<T> {
    fn from_js(&self, ctx: &dyn Context, val: &Value) -> T;

    fn try_from_js(
        &self, ctx: &dyn Context, val: &Value, err_params: &CheckErrorParams,
    ) -> Result<T, String>;

    fn to_js(&self, ctx: &dyn Context, val: &T) -> Value;
}

struct FnMapper<T> {
    checker: Arc<dyn Checker>,
    from_js_fn: Box<dyn Fn(&dyn Context, &Value) -> T>,
    to_js_fn: Box<dyn Fn(&dyn Context, &T) -> Value>,
}

unsafe impl<T> Sync for FnMapper<T> {}
unsafe impl<T> Send for FnMapper<T> {}

impl<T> Mapper<T> for FnMapper<T> {
    fn from_js(&self, ctx: &dyn Context, val: &Value) -> T {
        (self.from_js_fn)(ctx, val)
    }

    fn try_from_js(
        &self, ctx: &dyn Context, val: &Value, err_params: &CheckErrorParams,
    ) -> Result<T, String> {
        self.checker
            .check(ctx, &val, err_params)
            .map(|_| (self.from_js_fn)(ctx, val))
    }

    fn to_js(&self, ctx: &dyn Context, val: &T) -> Value {
        (self.to_js_fn)(ctx, val)
    }
}

fn f64_from_js(_ctx: &dyn Context, val: &Value) -> f64 {
    val.to_number().unwrap()
}

fn f64_to_js(ctx: &dyn Context, val: &f64) -> Value {
    ctx.value_make_number(*val)
}

fn i32_from_js(_ctx: &dyn Context, val: &Value) -> i32 {
    val.to_number().unwrap() as i32
}

fn i32_to_js(ctx: &dyn Context, val: &i32) -> Value {
    ctx.value_make_number(*val as f64)
}

fn str_from_js(_ctx: &dyn Context, val: &Value) -> String {
    val.to_string().unwrap().to_utf8()
}

fn str_to_js(ctx: &dyn Context, val: &String) -> Value {
    ctx.value_make_string(&ctx.string_make_from_utf8(val))
}

lazy_static! {
    pub static ref NUMBER_MAPPER: Arc<dyn Mapper<f64> + Send + Sync> =
        Arc::new(FnMapper {
            checker: NUMBER_CHECKER.clone(),
            from_js_fn: Box::new(f64_from_js),
            to_js_fn: Box::new(f64_to_js),
        });
    pub static ref INT_MAPPER: Arc<dyn Mapper<i32> + Send + Sync> =
        Arc::new(FnMapper {
            checker: NUMBER_CHECKER.clone(),
            from_js_fn: Box::new(i32_from_js),
            to_js_fn: Box::new(i32_to_js),
        });
    pub static ref STRING_MAPPER: Arc<dyn Mapper<String> + Send + Sync> =
        Arc::new(FnMapper {
            checker: STRING_CHECKER.clone(),
            from_js_fn: Box::new(str_from_js),
            to_js_fn: Box::new(str_to_js),
        });
}

pub struct StructMapper<T> {
    pub from_js_fn: Box<
        dyn Fn(
            &dyn Context,
            &Value,
            Option<&CheckErrorParams>,
        ) -> Result<T, String>,
    >,
    pub to_js_fn: Box<dyn Fn(&dyn Context, &T) -> Value>,
}

impl<T> Mapper<T> for StructMapper<T> {
    fn from_js(&self, ctx: &dyn Context, val: &Value) -> T {
        (self.from_js_fn)(ctx, val, None).unwrap()
    }

    fn try_from_js(
        &self, ctx: &dyn Context, val: &Value, err_params: &CheckErrorParams,
    ) -> Result<T, String> {
        (self.from_js_fn)(ctx, val, Some(err_params))
    }

    fn to_js(&self, ctx: &dyn Context, val: &T) -> Value {
        (self.to_js_fn)(ctx, val)
    }
}

#[macro_export]
macro_rules! struct_mapper {
    (
        $STRUCT_TYPE:ident {
            $($NAME:ident: $TYPE:ty => $MAPPER:expr,)+
        }
    ) => {
        paste::expr! {
            $( let [<$NAME _MAPPER_CLONE>] = $MAPPER.clone(); )+
            let to_js = move |ctx: &dyn aardvark_jsi::Context,
                              val: &$STRUCT_TYPE| {
                let res = ctx.object_make(None);
                $({
                    let prop_val =
                        [<$NAME _MAPPER_CLONE>].to_js(ctx, &val.$NAME);
                    let _ = res.set_prop(stringify!($NAME), &prop_val);
                })*;
                res.to_value()
            };

            $( let [<$NAME _MAPPER_CLONE>] = $MAPPER.clone(); )+
            let from_js = move |
                ctx: &dyn aardvark_jsi::Context,
                val: &aardvark_jsi::Value,
                err_params: Option<&aardvark_jsi::CheckErrorParams>
            | {
                if let Some(err_params) = err_params {
                    if let Err(check_err) =
                        aardvark_jsi::OBJECT_CHECKER.check(ctx, val, err_params)
                    {
                        return Err(check_err);
                    }
                }
                let obj = val.to_object().unwrap();
                let mut res = $STRUCT_TYPE{
                    ..Default::default()
                };
                if let Some(err_params) = err_params {
                    $(
                        let prop_name_str = stringify!($NAME);
                        let prop_val = if obj.has_prop(prop_name_str) {
                            obj.get_prop(prop_name_str).unwrap()
                        } else {
                            ctx.value_make_undefined()
                        };
                        let prop_err_params = aardvark_jsi::CheckErrorParams{
                            kind: err_params.kind.clone(),
                            name: err_params.name.clone() + "." + prop_name_str,
                            target: err_params.target.clone(),
                        };
                        match [<$NAME _MAPPER_CLONE>].try_from_js(
                            ctx, &prop_val, &prop_err_params)
                        {
                            Ok(val) => res.$NAME = val,
                            Err(err) => return Err(err),
                        };
                    )*
                } else {
                    $(
                        let prop_val = obj.get_prop(stringify!($NAME)).unwrap();
                        res.$NAME =
                            [<$NAME _MAPPER_CLONE>].from_js(ctx, &prop_val);
                    )*
                }
                Ok(res)
            };

            aardvark_jsi::StructMapper{
                from_js_fn: Box::new(from_js),
                to_js_fn: Box::new(to_js),
            }
        }
    };
}
