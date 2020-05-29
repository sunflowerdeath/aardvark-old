let fs = require('fs')
let path = require('path')
let yaml = require('js-yaml')
let Handlebars = require('handlebars')
let mkdirp = require('mkdirp')

let compileTmpl = tmpl => Handlebars.compile(tmpl, { noEscape: true })

let structTmpl = `
# \`struct {{name}}\`

{{doc}}

{{#each props}}
---
### \`{{name}}\`

Type: \`{{type}}\`

{{doc}}
{{/each}}
`

let classTmpl = `
# \`class {{name}}\`

{{doc}}

{{#if extends}}**Extends:** \`{{extends}}\`{{/if}}

## Members

{{#each props}}
---
### \`{{name}}\`

Type: \`{{type}}\`

{{doc}}

{{/each}}

{{#each methods}}
---
### \`{{name}}({{#each args}}{{name}}{{#unless @last}}, {{/unless}}{{/each}})\`

{{doc}}

{{#if args}}
**Arguments:**

{{#each args}}
- **{{name}}**
  <br>
  Type: \`{{type}}\`
  <br>
  {{doc}}
{{/each}}
{{/if}}

{{#if return}}**Returns:** \`{{return}}\`{{/if}}

{{/each}}
`

let templates = {
    class: compileTmpl(classTmpl),
    struct: compileTmpl(structTmpl)
}

let docgen = options => {
    let src = Array.isArray(options.src) ? options.src : [options.src]
    let defs = []
    src.forEach(src => {
        let input = fs.readFileSync(src, 'utf8')
        yaml.loadAll(input, def => { if (def) defs.push(def) })
    })
    let res = {}
    defs.forEach(def => {
        if (def.kind === 'class' || def.kind === 'struct') {
            res[def.name] = templates[def.kind](def)
        }
    })
    mkdirp.sync(options.output.dir)
    Object.entries(res).forEach(([name, doc]) => {
        fs.writeFileSync(path.join(options.output.dir, `${name}.md`), doc)
    })
}

module.exports = docgen


