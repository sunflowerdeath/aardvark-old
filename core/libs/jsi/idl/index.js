let fs = require('fs')
let path = require('path')
let yaml = require('js-yaml')
let Handlebars = require('handlebars')
let { snakeCase } = require('change-case')
let { groupBy, partition, uniq, flatten } = require('lodash')
let mkdirp = require('mkdirp')

let compileTmpl = tmpl => Handlebars.compile(tmpl, { noEscape: true })

Handlebars.registerHelper("snakeCase", snakeCase)

const structDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{typeName}}>> {{name}}_mapper;
`)

const structInitTmpl = compileTmpl(`
    auto to_js = [this](Context& ctx, const {{typeName}}& val) {
        auto res = ctx.object_make(nullptr);
        {{#each props}}
        res.set_property("{{name}}",
            {{type}}_mapper->to_js(ctx, val.{{snakeCase name}}));
        {{/each}}
        return res.to_value();
    };

    auto try_from_js = [this](
        Context& ctx, const Value& val, const CheckErrorParams& err_params
    ) -> tl::expected<{{typeName}}, std::string> {
        auto err = check_type(ctx, val, "object", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        auto obj = val.to_object().value();
        auto mapped_struct = {{typeName}}();
        {{#each props}}
        {
            auto prop_val = std::optional<Value>();
            if (obj.has_property("{{name}}")) {
                auto res = obj.get_property("{{name}}");
                if (res.has_value()) prop_val = res.value();
            }
            {{#if hasDefault}}
            if (!prop_val.has_value() || prop_val.value().get_type() == ValueType::undefined) {
                goto end_{{name}};
            }
            {{/if}}
            if (!prop_val.has_value()) prop_val = ctx.value_make_undefined();
            auto prop_err_params = CheckErrorParams{
                err_params.kind,
                err_params.name + ".{{name}}",
                err_params.target};
            auto mapped_prop_val = {{type}}_mapper->try_from_js(
                ctx, prop_val.value(), prop_err_params);
            if (mapped_prop_val.has_value()) {
                mapped_struct.{{snakeCase name}} = mapped_prop_val.value();
            } else {
                return tl::make_unexpected(mapped_prop_val.error());
            }
        }
        end_{{name}}:
        {{/each}}
        return mapped_struct;
    };

    {{name}}_mapper = SimpleMapper<{{typeName}}>(to_js, try_from_js);
`)

const unionDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{typeName}}>> {{name}}_mapper;
`)

const unionInitTmpl = compileTmpl(`
    auto to_js = [this](Context& ctx, const {{typeName}}& val) {
        auto tag = "";
        std::optional<Value> js_val = std::nullopt;
        {{#each items}}if (auto a = std::get_if<{{getTypeName type}}>(&val)) {
            tag = "{{tag}}";
            js_val = {{type}}_mapper->to_js(ctx, *a);
        }{{#unless @last}} else {{/unless}}{{/each}}
        js_val.value().to_object().value().set_property(
            "{{tag}}",
            ctx.value_make_string(ctx.string_make_from_utf8(tag)));
        return js_val.value();
    };

    auto try_from_js = [this](
        Context& ctx, const Value& val, const CheckErrorParams& err_params
    ) -> tl::expected<{{typeName}}, std::string> {
        auto tag = val
            .to_object()
            .and_then([](auto obj) { return obj.get_property("tag"); })
            .and_then([](auto val) { return val.to_string(); })
            .map([](auto str) { return str.to_utf8(); });
        if (!tag.has_value()) return tl::make_unexpected("Invalid tag");
        {{#each items}}if (tag.value() == "{{tag}}") {
            return {{type}}_mapper->try_from_js(ctx, val, err_params);
        }{{#unless @last}} else {{/unless}}{{/each}}
        return tl::make_unexpected("Invalid tag");
    };

    {{name}}_mapper = SimpleMapper<{{typeName}}>(to_js, try_from_js);
`)

const enumDefTmpl = compileTmpl(`
    std::optional<EnumMapper<{{typeName}}>> {{name}}_mapper;
`)

const enumInitTmpl = compileTmpl(`
    {{name}}_mapper = EnumMapper<{{typeName}}>(int_mapper);
    auto obj = ctx->object_make(nullptr);
    {{#each values}}
    obj.set_property("{{.}}", ctx->value_make_number({{@index}}));
    {{/each}}
    ctx->get_global_object().set_property("{{name}}", obj.to_value());
`)

const classDefTmpl = compileTmpl(`
    std::optional<ObjectsMapper2<{{className}}, {{rootClassName}}>>
        {{name}}_mapper;
    std::optional<Class> {{name}}_js_class;
`)

const classInitTmpl = compileTmpl(`
    auto def = ClassDefinition();
    def.name = "{{name}}";
    {{#if extends}}def.base_class = {{extends}}_js_class;{{/if}}

    {{#each props}}
    auto {{name}}_getter = [this](Object& this_obj) -> Result<Value> {
        auto mapped_this = {{../name}}_mapper->from_js(
            *ctx, this_obj.to_value());
        {{#if get_proxy}}
        return {{get_proxy}}(*ctx, mapped_this, *{{type}}_mapper);
        {{else}}
        auto prop_val = {{#if getter}}mapped_this->{{getter}}()
            {{else}}mapped_this->{{snakeCase name}}{{/if}};
        return {{type}}_mapper->to_js(*ctx, prop_val);
        {{/if}}
    };
    {{#unless readonly}}
    auto {{name}}_setter = [this](Object& this_obj, Value& val)
        -> Result<bool> {
        auto mapped_this = {{../name}}_mapper->from_js(
            *ctx, this_obj.to_value());
        auto err_params = CheckErrorParams{
            "property", "{{name}}", "{{../name}}"};
        {{#if set_proxy}}
        return {{set_proxy}}(
            *ctx, mapped_this, val, *{{type}}_mapper, err_params);
        {{else}}
        auto mapped_val = {{type}}_mapper->try_from_js(*ctx, val, err_params);
        if (!mapped_val.has_value()) {
            return make_error_result(*ctx, mapped_val.error());
        }
        {{#if setter}}
        mapped_this->{{setter}}(mapped_val.value());
        {{else}}
        mapped_this->{{snakeCase name}} = mapped_val.value();
        {{/if}}
        return true;
        {{/if}}
    };
    {{/unless}}
    {{/each}}

    {{#each methods}}
    auto {{name}}_method = [this](Value& this_val, std::vector<Value>& args) 
        -> Result<Value> {
        auto mapped_this = {{../name}}_mapper->from_js(*ctx, this_val);
        {{#each args}}
        auto {{name}}_err_params = CheckErrorParams{
            "argument", "{{name}}", "{{../../name}}.{{../name}}"};
        auto {{name}}_arg = {{type}}_mapper->try_from_js(
            *ctx, args[{{@index}}], {{name}}_err_params);
        if (!{{name}}_arg.has_value()) {
            return make_error_result(*ctx, {{name}}_arg.error());
        }
        {{/each}}
        {{#if proxy}}
        {{#if return}}return {{/if}}{{proxy}}(
            *ctx, mapped_this
            {{#each args}}, {{name}}_arg.value(){{/each}}
            {{#if return}},*{{return}}_mapper{{/if}}
        );
        {{#unless return}}return ctx->value_make_undefined();{{/unless}}
        {{else}}
        {{#if return}}auto res = {{/if}}mapped_this->{{snakeCase name}}(
            {{#each args}}{{name}}_arg.value(){{#unless @last}}, {{/unless}}{{/each}}
        );
        {{#if return}}
        return {{return}}_mapper->to_js(*ctx, res);
        {{else}}
        return ctx->value_make_undefined();
        {{/if}}
        {{/if}}
    };
    {{/each}}

    def.properties = {
        {{#each props}}
        {
            "{{name}}", 
            ClassPropertyDefinition{
                {{name}}_getter,
                {{#if readonly}}nullptr{{else}}{{name}}_setter{{/if}}
            }
        }{{#unless @last}},{{/unless}}
        {{/each}}
    };
    
    def.methods = {
        {{#each methods}}
        {"{{name}}", {{name}}_method}{{#unless @last}},{{/unless}}
        {{/each}}
    };
    
    {{#unless extends}}
    def.finalizer = [](const Object& object) {
        ObjectsIndex<{{rootClassName}}>::finalize(object.to_value());
    };
    {{/unless}}

    {{name}}_js_class = ctx->class_make(def);
    js_class_map.emplace(
        std::type_index(typeid({{className}})), {{name}}_js_class.value());
    {{name}}_mapper = ObjectsMapper2<{{className}}, {{rootClassName}}>(
        &{{rootClass}}_objects_index.value());
    
    auto ctor = [this](Value& this_val, std::vector<Value>& args)
        -> Result<Value> {
    {{#if constructor}}
        {{#each constructor.args}}
        auto {{name}}_err_params = CheckErrorParams{
            "argument", "{{name}}", "new {{../name}}()"};
        auto {{name}}_arg = {{type}}_mapper->try_from_js(
            *ctx, args[{{@index}}], {{name}}_err_params);
        if (!{{name}}_arg.has_value()) {
            return make_error_result(*ctx, {{name}}_arg.error());
        }
        {{/each}}
        auto res = std::make_shared<{{className}}>(
            {{#each constructor.args}}{{name}}_arg.value(){{#unless @last}},{{/unless}}{{/each}}
        );
        return {{name}}_mapper->to_js(*ctx, res);
    {{else}}
        return make_error_result(
            *ctx, "Class '{{name}}' doesn't provide a constructor.");
    {{/if}}
    };
    auto ctor_obj = ctx->object_make_constructor2(
        {{name}}_js_class.value(), ctor);
    ctx->get_global_object().set_property("{{name}}", ctor_obj.to_value());
`)

const callbackDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{typeName}}>> {{name}}_mapper;
`)

const callbackInitTmpl = compileTmpl(`
    auto to_js = [](Context& ctx, const {{typeName}}& val) {
        return ctx.value_make_null();
    };

    auto try_from_js = [this](
        Context& ctx, const Value& val, const CheckErrorParams& err_params
    )  -> tl::expected<{{typeName}}, std::string> {
        // TODO check type
        auto fn = val.to_object().value();
        return [this, fn, &ctx](
            {{#each args}}{{getTypeName type}} {{name}}{{#unless @last}},{{/unless}}{{/each}}
        ) -> {{#if return}}{{getTypeName return}}{{else}}void{{/if}} {
            auto js_args = std::vector<Value>();
            {{#each args}}
            js_args.push_back({{type}}_mapper->to_js(ctx, {{name}}));
            {{/each}}
            auto res = fn.call_as_function(nullptr, js_args);
            if (!res.has_value()) {
                if (error_handler) error_handler(res.error());
                // TODO fallback
                return {{#if return}}{{getTypeName return}}(){{/if}};
            }
            {{#if return}}
            auto err_params = CheckErrorParams{"return value", "", "{{name}}"};
            auto js_res = {{return}}_mapper->try_from_js(
                ctx, res.value(), err_params);
            if (!js_res.has_value()) {
                if (error_handler) {
                    auto err_val = ctx.value_make_error(js_res.error());
                    auto err = Error(&ctx, &err_val);
                    error_handler(err);
                }
                // TODO fallback
                return {{#if return}}{{getTypeName return}}(){{/if}};
            }
            return js_res.value();
            {{/if}}
        };
    };
    
    {{name}}_mapper = SimpleMapper<{{typeName}}>(to_js, try_from_js);
`)

const functionDefTmpl = compileTmpl(``)

const functionInitTmpl = compileTmpl(`
    auto func = [this](Value& this_val, std::vector<Value>& args)
        -> Result<Value> {
        // TODO check number of arguments
        {{#each args}}
        auto {{name}}_err_params = CheckErrorParams{
            "argument", "{{name}}", "{{../name}}"};
        auto {{name}}_arg = {{type}}_mapper->try_from_js(
            *ctx, args[{{@index}}], {{name}}_err_params);
        if (!{{name}}_arg.has_value()) {
            return make_error_result(*ctx, {{name}}_arg.error());
        }
        {{/each}}
        {{#if return}}auto res = {{/if}}{{functionName}}(
            *ctx,
            {{#each args}}{{name}}_arg.value(){{#unless @last}}, {{/unless}}{{/each}}
        );
        {{#if return}}
        return {{return}}_mapper->to_js(*ctx, res);
        {{else}}
        return ctx->value_make_undefined();
        {{/if}}
    };
    auto obj = ctx->object_make_function(func);
    ctx->get_global_object().set_property("{{name}}", obj.to_value());
`)

const customDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{typeName}}>> {{name}}_mapper;
`)

const customInitTmpl = compileTmpl(`
    {{name}}_mapper = SimpleMapper<{{typeName}}>({{to_js}}, {{try_from_js}});
`)

const optionalDefTmpl = compileTmpl(`
    std::optional<OptionalMapper<{{innerTypeName}}>> {{name}}_mapper;
`)

const optionalInitTmpl = compileTmpl(`
    {{name}}_mapper = OptionalMapper<{{innerTypeName}}>(&{{type}}_mapper.value());
`)

const templates = {
    struct: { def: structDefTmpl, init: structInitTmpl },
    union: { def: unionDefTmpl, init: unionInitTmpl },
    enum: { def: enumDefTmpl, init: enumInitTmpl },
    class: { def: classDefTmpl, init: classInitTmpl },
    callback: { def: callbackDefTmpl, init: callbackInitTmpl },
    function: { def: functionDefTmpl, init: functionInitTmpl },
    custom: { def: customDefTmpl, init: customInitTmpl },
    optional: { def: optionalDefTmpl, init: optionalInitTmpl }
}     

const headerTmpl = compileTmpl(
`#pragma once

#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>

{{#each include}}
#include "{{.}}"
{{/each}}

namespace {{output.namespace}} {

using namespace aardvark::jsi;

class {{output.class}} {
  public:
    {{output.class}}(Context* ctx);
    
    Context* ctx;
    std::function<void(Error&)> error_handler;
    std::unordered_map<std::type_index, Class> js_class_map = {};

    {{#each rootClasses}}
    std::optional<ObjectsIndex<{{className}}>> {{name}}_objects_index;
    {{/each}}

    {{#each types}}
    {{def}}
    {{/each}}
};

} // namespace {{output.namespace}}
`)

const implTmpl = compileTmpl(
`#include "{{output.filename}}.hpp"

namespace {{output.namespace}} {

{{output.class}}::{{output.class}}(Context* ctx_arg) : ctx(ctx_arg) {
    {{#each rootClasses}}
    {{name}}_objects_index = ObjectsIndex<{{className}}>("{{name}}", &js_class_map);
    {{/each}}
    
    {{#each types}}
    // {{kind}} {{name}}
    {
    {{init}}
    }
    {{/each}}
}

} // namespace {{output.namespace}}
`)

const defaultTypes = {
    bool: 'bool',
    int: 'int',
    float: 'float',
    string: 'std::string'
}

let getRootClass = (name, defs) => {
    let def = defs[name]
    if (def['extends'] === undefined) return name
    return getRootClass(defs[def['extends']].name, defs)
}

let setRootClasses = data => {
    data.rootClasses = []
    for (let name in data.defs) {
        let def = data.defs[name]
        if (def.kind != 'class') continue;
        if (def['extends'] === undefined) data.rootClasses.push(def)
        def.rootClass = getRootClass(def.name, data.defs)
        def.rootClassName = data.defs[def.rootClass].className
    }
}

let getTypeName = (name, defs) => {
    if (name in defaultTypes) return defaultTypes[name]
    if (name in defs) return defs[name].typeName
    throw new Error(`Unknown type "${name}"`)
}

let setTypeNames = (data, options) => {
    let [callbacks, rest] = partition(data.defs, def => def.kind === 'callback') 
    rest.forEach(def => {
        if (!('originalName' in def)) {
            def.originalName = def.kind === 'function'
                ? snakeCase(def.name)
                : def.name
        }
        let ns = def.namespace !== undefined 
            ? def.namespace 
            : options.defaultNamespace
        let prefix = ns !== undefined ? `${ns}::` : ''
        let fullName = `${prefix}${def.originalName}`
        if (def.kind === 'class') {
            def.className = fullName
            def.typeName = `std::shared_ptr<${fullName}>`
        } else if (def.kind === 'optional') {
            def.innerTypeName = `${prefix}${def.type}`
            def.typeName = `std::optional<${def.innerTypeName}>`
        } else if (def.kind == 'function') {
            def.functionName = fullName
        } else {
            def.typeName = fullName
        }
    })
    // TODO callbacks that return/take other callbacks or
    // composition of optional/array/other - use recursive typename resolution
    callbacks.forEach(def => {
        let returnTypeName = 'return' in def
            ? getTypeName(def['return'], data.defs)
            : 'void'
        let argTypeNames = 'args' in def
            ? def.args.map(arg => getTypeName(arg.type, data.defs)).join(', ')
            : ''
        def.typeName = `std::function<${returnTypeName}(${argTypeNames})>`
    })
}

let prepareData = (defs, options) => {
    let data = { defs: {} }
    defs.forEach(def => data.defs[def.name] = def)
    setTypeNames(data, options)
    setRootClasses(data)
    return data
}

let genCode = (data, options) => {
    let tmplOptions = {
        helpers: {
            getTypeName: name => getTypeName(name, data.defs)
        }
    }
    let chunks = []
    let include = options.include == undefined ? [] : options.include
    for (let name in data.defs) {
        let def = data.defs[name]
        if ('include' in def) include.push(def.include)
        let t = templates[def.kind]
        chunks.push({
            name: def.name,
            kind: def.kind,
            def: t.def(def, tmplOptions),
            init: t.init(def, tmplOptions)
        })
    }
    include = uniq(flatten(include))
    let tmplData = {
        ...options, types: chunks, include, rootClasses: data.rootClasses
    }
    return {
        header: headerTmpl(tmplData, tmplOptions),
        impl: implTmpl(tmplData, tmplOptions)
    }
}

let output = (code, options) => {
    let { header, impl } = code    
    let { filename, dir } = options.output
    mkdirp.sync(dir)
    fs.writeFileSync(path.join(dir, `${filename}.hpp`), header)
    fs.writeFileSync(path.join(dir, `${filename}.cpp`), impl)
}

let gen = options => {
    let src = Array.isArray(options.src) ? options.src : [options.src]
    let defs = []
    src.forEach(src => {
        let input = fs.readFileSync(src, 'utf8')
        yaml.loadAll(input, def => { if (def) defs.push(def) })
    })
    let data = prepareData(defs, options)
    let code = genCode(data, options)
    output(code, options)
}

module.exports = gen
