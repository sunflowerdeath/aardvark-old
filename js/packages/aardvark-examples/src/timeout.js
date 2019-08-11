
let t2
let t1 = setTimeout(() => {
    log('timeout1')
    clearTimeout(t2)
}, 2000)
t2 = setTimeout(() => log('timeout1'), 2000)

let app = new DesktopApp()
app.createWindow(640, 480)
app.run()