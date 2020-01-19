use crate::jsi::*;
use lazy_static::*;
use std::sync::Arc;

pub struct CheckErrorParams {
    pub kind: String,
    pub name: String,
    pub target: String,
}

type CheckResult = Result<(), String>;

pub trait Checker {
    fn check(
        &self, ctx: &dyn Context, val: &Value, err_params: &CheckErrorParams,
    ) -> CheckResult;
}

fn get_type_name(value: &Value) -> String {
    match value.get_type() {
        ValueType::Null => "null".to_owned(),
        ValueType::Undefined => "undefined".to_owned(),
        ValueType::Number => "number".to_owned(),
        ValueType::Bool => "boolean".to_owned(),
        ValueType::String => "string".to_owned(),
        // ValueType::Symbol => "symbol",
        ValueType::Object => {
            let object = value.to_object().unwrap();
            if object.is_array() {
                return "array".to_owned();
            }
            if object.is_function() {
                return "function".to_owned();
            }
            "object".to_owned()
        }
        _ => "unknown".to_owned(),
    }
}

struct FnChecker {
    check_fn:
        Box<dyn Fn(&dyn Context, &Value, &CheckErrorParams) -> CheckResult>,
}

impl Checker for FnChecker {
    fn check(
        &self, ctx: &dyn Context, val: &Value, err_params: &CheckErrorParams,
    ) -> CheckResult {
        self.check_fn.as_ref()(ctx, val, err_params)
    }
}

unsafe impl Sync for FnChecker {}
unsafe impl Send for FnChecker {}

fn make_primitive_checker(expected_type: &str) -> Arc<FnChecker> {
    let expected_type = expected_type.to_string();
    let check = move |_ctx: &dyn Context,
                      val: &Value,
                      err_params: &CheckErrorParams| {
        let val_type = get_type_name(val);
        if val_type == expected_type {
            return Ok(());
        }
        if expected_type == "object"
            && (val_type == "function" || val_type == "array")
        {
            return Ok(());
        }
        Err(format!(
            "Invalid {} `{}` of type `{}` supplied to `{}`, expected `{}`.",
            err_params.kind,
            err_params.name,
            val_type,
            err_params.target,
            expected_type
        ))
    };
    Arc::new(FnChecker {
        check_fn: Box::new(check),
    })
}

lazy_static! {
    pub static ref BOOL_CHECKER: Arc<dyn Checker + Send + Sync> =
        make_primitive_checker("boolean");
    pub static ref NUMBER_CHECKER: Arc<dyn Checker + Send + Sync> =
        make_primitive_checker("number");
    pub static ref STRING_CHECKER: Arc<dyn Checker + Send + Sync> =
        make_primitive_checker("string");
    pub static ref OBJECT_CHECKER: Arc<dyn Checker + Send + Sync> =
        make_primitive_checker("object");
    pub static ref ARRAY_CHECKER: Arc<dyn Checker + Send + Sync> =
        make_primitive_checker("array");
}
