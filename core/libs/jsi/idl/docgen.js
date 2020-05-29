let fs = require('fs')
let path = require('path')
let yaml = require('js-yaml')
let Handlebars = require('handlebars')
let mkdirp = require('mkdirp')

let compileTmpl = tmpl => Handlebars.compile(tmpl, { noEscape: true })

let enumTmpl = `
# \`enum {{name}}\`

{{doc}}

## Values

{{#each values}}
- \`{{.}}\`

{{/each}}
`

let structTmpl = `
# \`struct {{name}}\`

{{doc}}

## Props

{{#each props}}
### \`{{name}}\`

Type: \`{{type}}\`

{{doc}}

{{#unless @last}}---{{/unless}}
{{/each}}
`

let classTmpl = `
# \`class {{name}}\`

{{#if extends}}**Extends:** \`{{extends}}\`{{/if}}

{{doc}}

## Members

\`\`\`@toc
levels: [3]
\`\`\`

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
    struct: compileTmpl(structTmpl),
    enum: compileTmpl(enumTmpl)
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
        if (def.kind === 'class' || def.kind === 'struct' || def.kind === 'enum') {
            res[def.name] = templates[def.kind](def)
        }
    })
    mkdirp.sync(options.output.dir)
    Object.entries(res).forEach(([name, doc]) => {
        fs.writeFileSync(path.join(options.output.dir, `${name}.md`), doc)
    })
}

module.exports = docgen


