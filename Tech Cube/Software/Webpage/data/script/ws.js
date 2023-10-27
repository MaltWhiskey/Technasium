var reconnect;
function connect(){
    clearTimeout(play)
    playing = false;
    socket = new WebSocket("ws://"+location.host.replace(/:.*$/,"")+":1337")
    // socket = new WebSocket("ws://10.1.68.48:1337")
    // socket = new WebSocket("ws://127.0.0.1:1337")
    // socket = new WebSocket("ws://10.0.0.33:1337")
    socket.onopen = (event) => {
        clearTimeout(reconnect)
        console.log("connected");
    };
    socket.onmessage = (event) => {
        console.log("Message from server ", (event.data));
        if (playing){
            console.log("playing locally ignoring")
            return
        }
        data2 = [];
        a = 0
        b = 0
        c = 0
        event.data.replace(']', '').replace('[', '').replace('"', '').split(/,square,?/).slice(0,4).forEach((square) => {
            square.split(/,row,?/).slice(0,4).forEach((row) => {
                row.split(/,/).slice(0,4).forEach((pixel) => {
                    cube[a][b][c].value(pixel)
                    c++
                })
                b++
                c = 0
            })
            a++
            b = 0
        })

        console.log(cube);
    };
    socket.onclose = () => {
        console.log("trying to reconnect")
        reconnect = setTimeout(socket.onclose,5000);
        connect()
    }
}
function send() {
    let x = 0;
    let y = 0;
    let z = 0;
    blocks = [];
    cube.forEach((square) => {
        square.forEach((row) => {
            row.forEach((pixel) => {
                blocks.push(pixel.value())
                x++;
            });
            blocks.push("row");
            x = 0;
            y++;
        });
        blocks.push("square");
        y = 0;
        z++;
    });
    socket.send(blocks);
    console.log("update")
}