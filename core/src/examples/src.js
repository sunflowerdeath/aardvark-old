let app = new DesktopApp();
log('create app');
let window = app.createWindow(640, 480);
log('create window');
log('app.windows.size =', app.windows.size);
log('app.windows[0].width =', app.windows[0].width);
let doc = app.getDocument(window);
log('get doc');

let stack = new Stack();
let align = new Align();
align.align = {
    left: {type: 'abs', value: 50},
    top: {type: 'abs', value: 100}
};
let red = new Background();
align.appendChild(red)
stack.appendChild(align);

doc.root = stack;
log('set root');

app.run();
