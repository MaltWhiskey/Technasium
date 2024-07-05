var reconnect;
function isJson(item) {
    let value = typeof item !== "string" ? JSON.stringify(item) : item;
    try {
      value = JSON.parse(value);
    } catch (e) {
      return false;
    }
  
    return typeof value === "object" && value !== null;
  }
function connect(){
    playing = false;
    // socket = new WebSocket("ws://10.1.68.48:1337")
    if (location.host == "127.0.0.1:5500"){
        socket = new WebSocket("ws://esp32-led-cube-4827e25725a4.local:1337")
    } else {
        socket = new WebSocket("ws://"+location.host.replace(/:.*$/,"")+":1337")
    }
    socket.onopen = (event) => {
        clearTimeout(reconnect)
        console.log("connected");
    };
    socket.onclose = () => {
        console.log("trying to reconnect")
        reconnect = setTimeout(connect,5000);
    }
    socket.onmessage = (event) => {
        if (!isJson(event.data)) {
            console.log("Non JSON message from server ", event.data);
            return
        }
        console.log("JSON message from server ", JSON.parse(event.data));
        if (playing){
            console.log("playing locally ignoring")
            return
        }
        animation.jsondata = {
            ...animation.jsondata,
            ...JSON.parse(event.data)
        }
        // data2 = [];
        // a = 0
        // b = 0
        // c = 0
        // event.data.replace(']', '').replace('[', '').replace('"', '').split(/,square,?/).slice(0,4).forEach((square) => {
        //     square.split(/,row,?/).slice(0,4).forEach((row) => {
        //         row.split(/,/).slice(0,4).forEach((pixel) => {
        //             cube[a][b][c].value(pixel)
        //             c++
        //         })
        //         b++
        //         c = 0
        //     })
        //     a++
        //     b = 0
        // })
        load()
    };
}
function send() {
    socket.send(JSON.stringify(animation.jsondata))
}