
let ws = new WebSocket();
ws.onstart = () => log('START')
ws.onclose = () => log('CLOSE')
ws.onerror = () => log('ERROR')
