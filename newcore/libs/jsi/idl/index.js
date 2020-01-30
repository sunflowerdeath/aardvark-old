let fs = require('fs')
let path = require('path')
let yaml = require('js-yaml')
let Handlebars = require('handlebars')
let { snakeCase } = require('change-case')
let { groupBy, partition } = require('lodash')
let mkdirp = require('mkdirp')

let compileTmpl = tmpl => Handlebars.compile(tmpl, { noEscape: true })

Handlebars.registerHelper("snakeCase", snakeCase)

const structDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{originalName}}>> {{name}}_mapper;
`)

const structInitTmpl = compileTmpl(`
    auto to_js = [](Context& ctx, const {{name}}& val) {
        auto res = ctx.object_make(nullptr);
        {{#each props}}
        res.set_property("{{name}}", {{type}}_mapper->to_js(
            ctx, val.{{snakeCase name}}));
        {{/each}}
        return res.to_value();
    };

    auto try_from_js = [](
        Context& ctx, const Value& val, const CheckErrorParams& err_params
    ) -> tl::expected<{{name}}, std::string> {
        auto err = check_type(ctx, val, "object", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        auto obj = val.to_object().value();
        auto mapped_struct = {{name}}();
        {{#each props}}
        {
            auto prop_val = std::optional<Value>();
            if (obj.has_property("{{name}}")) {
                auto res = obj.get_property("{{name}}");
                if (res.has_value()) prop_val = res.value();
            }
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
        {{/each}}
        return mapped_struct;
    };

    {{name}}_mapper = SimpleMapper<{{typename}}>(to_js, try_from_js);
`)

const enumDefTmpl = compileTmpl(`
    std::optional<EnumMapper<{{typename}}>> {{name}}_mapper;
`)

const enumInitTmpl = compileTmpl(`
    {{name}}_mapper = EnumMapper<{{typename}}>(int_mapper);
    auto obj = ctx->object_make(nullptr);
    {{#each values}}
    obj.set_property("{{.}}", ctx->value_make_number({{@index}}));
    {{/each}}
    ctx->get_global_object().set_property("{{name}}", obj.to_value());
`)

const classDefTmpl = compileTmpl(`
    std::optional<ObjectsMapper<{{originalName}}>> {{name}}_mapper;
    std::optional<Class> {{name}}_js_class;
`)

const classInitTmpl = compileTmpl(`
    auto def = ClassDefinition();
    def.name = "{{name}}";

    {{#each props}}
    auto {{name}}_getter = [this](Object& this_obj) -> Result<Value> {
        auto mapped_this = {{../name}}_mapper->from_js(*ctx, this_obj.to_value());
        return {{type}}_mapper->to_js(*ctx, mapped_this->{{snakeCase name}});
    };
    {{#unless readonly}}
    auto {{name}}_setter = [this](Object& this_obj, Value& val) -> Result<bool> {
        auto mapped_this = {{../name}}_mapper->from_js(*ctx, this_obj.to_value());
        auto err_params = CheckErrorParams{"property", "{{name}}", "{{../name}}"};
        auto mapped_val = {{type}}_mapper->try_from_js(*ctx, val, err_params);
        if (!mapped_val.has_value()) {
            return make_error_result(*ctx, mapped_val.error());
        }
        mapped_this->{{snakeCase name}} = mapped_val.value();
        return true;
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
        auto res = mapped_this->{{snakeCase name}}(
            {{#each args}}{{name}}_arg.value(){{#unless @last}}, {{/unless}}{{/each}}
        );
        {{#if return}}
        return {{return}}_mapper->to_js(*ctx, res);
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
    
    def.finalizer = [](const Object& object) {
        ObjectsMapper<{{name}}>::finalize(object.to_value());
    };

    {{name}}_js_class = ctx->class_make(def);
    {{name}}_mapper = ObjectsMapper<{{name}}>("{{name}}", {{name}}_js_class.value());
    
    auto ctor = [this](Value& this_val, std::vector<Value>& args) -> Result<Value> {
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
        auto res = std::make_shared<{{name}}>(
            {{#each constructor.args}}{{name}}_arg.value(){{#unless @last}},{{/unless}}{{/each}}
        );
        return {{name}}_mapper->to_js(*ctx, res);
    {{else}}
        return make_error_result("Class '{{name}}' doesn't provide a constructor.")
    {{/if}}
    };
    auto ctor_obj = ctx->object_make_constructor2({{name}}_js_class.value(), ctor);
    ctx->get_global_object().set_property("{{name}}", ctor_obj.to_value());
`)

const callbackDefTmpl = compileTmpl(`
    std::optional<SimpleMapper<{{typename}}>> {{name}}_mapper;
`)

const callbackInitTmpl = compileTmpl(`
    auto from_js = [](Context* ctx, Value& val) {
        auto fn = val.to_object().value();
        return [fn](
            {{#each args}}{{getTypename type}} {{name}}{{#unless @last}},{{/unless}}{{/each}}
        ){
            auto js_args = std::vector<Value>();
            {{#each args}}
            js_args.push_back({{type}}_mapper->to_js(ctx, {{name}}));
            {{/each}}
            auto res = fn.call_as_function(nullptr, js_args);
            {{#if return}}
            auto js_res = {{return}}_mapper->try_from_js(ctx, res);
            if (!js_res.has_value()) {
                return make_error_result(*ctx, js_res.error());
            }
            return js_res.value();
            {{/if}}
        }
    };
    
    auto to_js = [](Context* ctx, const {{name}}& val) {
        return ctx->value_make_null();
    };
    
    {{name}}_mapper = SimpleMapper(from_js, to_js);
`)

const functionDefTmpl = compileTmpl(``)

const functionInitTmpl = compileTmpl(`
    auto func = [this](Value& this_val, std::vector<Value>&) {
        {{#each args}}
        auto err_params = CheckErrorParams{"argument", "{{name}}", "{{../name}}"};
        auto mapped_arg_{{name}} = {{type}}_mapper->try_from_js(
            ctx, args[{{@index}}], err_params);
        if (!mapped_arg_{{name}}.has_value()) {
            return make_error_result(*ctx, mapped_arg_{{name}}.value());
        }
        {{/each}}
        {{originalName}}(
            {{#each args}}mapped_arg_{{name}}.value(){{#unless @last}}, {{/unless}}{{/each}}
        );
        {{#if return}}
        return {{return}}_mapper->to_js(ctx, res);
        {{/if}}
    };
    auto obj = ctx->object_make_function(func);
    ctx->get_global_object().set_property("{{name}}", obj.to_value());
`)

const templates = {
    struct: { def: structDefTmpl, init: structInitTmpl },
    'enum': { def: enumDefTmpl, init: enumInitTmpl },
    'class': { def: classDefTmpl, init: classInitTmpl },
    'callback': { def: callbackDefTmpl, init: callbackInitTmpl },
    'function': { def: functionDefTmpl, init: functionInitTmpl }
}     

const headerTmpl = compileTmpl(
`#pragma once

#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>

{{#each includes}}
#include "{{.}}"
{{/each}}

namespace {{namespace}} {

using namespace aardvark::jsi;

class {{classname}} {
  public:
    {{classname}}(Context* ctx);
  // private:
    Context* ctx;
    {{#each types}}
    {{def}}
    {{/each}}
};

} // namespace {{namespace}}
`)

const implTmpl = compileTmpl(
`#include "{{filename}}.hpp"

namespace {{namespace}} {

{{classname}}::{{classname}}(Context* ctx_arg) : ctx(ctx_arg) {
    {{#each types}}
    
    // {{kind}} {{name}}
    {
    {{init}}
    }
    
    {{/each}}
}

} // namespace {{namespace}}
`)

const defaultTypes = {
    bool: 'bool',
    int: 'int',
    float: 'float',
    string: 'std::string'
}

let getTypename = (name, defs) => {
    if (name in defaultTypes) return defaultTypes[name]
    if (name in defs) return defs[name].typename
    throw new Error(`Unknown type "${name}"`)
}

let setTypenames = defs => {
    let [callbacks, rest] = partition(defs, def => def.kind === 'callback') 
    rest.map(def => {
        if (!('originalName' in def)) {
            def.originalName = def.kind === 'function'
                ? snakeCase(def.name)
                : def.name
        }
        def.typename = def.kind === 'class' 
            ? `std::shared_ptr<${def.originalName}>`
            : def.originalName
    })
    callbacks.map(def => {
        let returnTypename = 'return' in def
            ? getTypename(def['return'], defs)
            : 'void'
        let argTypenames = 'args' in def
            ? def.args.map(arg => getTypename(arg.type, defs)).join(', ')
            : ''
        def.typename = `std::function<${returnTypename}(${argTypenames})>`
    })
}

let genCode = (options, defs) => {
    let tmplOptions = {
        helpers: {
            getTypename: name => getTypename(name, defs)
        }
    }
    let chunks = []
    let includes = []
    for (let name in defs) {
        let def = defs[name]
        if ('include' in def) includes.push(def.include)
        let t = templates[def.kind];
        chunks.push({
            name: def.name,
            kind: def.kind,
            def: t.def(def, tmplOptions),
            init: t.init(def, tmplOptions)
        })
    }
    let ctx = { ...options, types: chunks, includes }
    return {
        header: headerTmpl(ctx, tmplOptions),
        impl: implTmpl(ctx, tmplOptions)
    }
}

let output = (options, code) => {
    let { header, impl } = code    
    let { filename, outputDir } = options
    mkdirp.sync(outputDir)
    fs.writeFileSync(path.join(outputDir, `${filename}.hpp`), header)
    fs.writeFileSync(path.join(outputDir, `${filename}.cpp`), impl)
}

let gen = options => {
    let input = fs.readFileSync(options.src, 'utf8')
    let defs = []
    yaml.loadAll(input, def => { if (def) defs.push(def) })
    setTypenames(defs)
    let defsByName = {}
    for (let def of defs) defsByName[def.name] = def
    let code = genCode(options, defsByName)
    output(options, code)
}

module.exports = gen
