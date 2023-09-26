socket.addEventListener("open", (event) => {
    console.log("connected");
});
socket.addEventListener("message", (event) => {
    console.log("Message from server ", (event.data));
    data2 = [];
    a = 0
    b = 0
    c = 0
    // pixel.value("#ffffff")
    event.data.replace(']', '').replace('[', '').replace('"', '').split(/,square,?/).slice(0,4).forEach((square) => {
        // data3 = []
        square.split(/,row,?/).slice(0,4).forEach((row) => {
            row.split(/,/).slice(0,4).forEach((pixel) => {
                cube[a][b][c].value(pixel)
                c++
            })
            b++
            c = 0
            // data3.push()
        })
        a++
        b = 0
        // data2.push(data3)
    })

    console.log(data2);
    // cube = 
});
function send() {
    let x = 0;
    let y = 0;
    let z = 0;
    blocks = [];
    cube.forEach((square) => {
        // translate(0,-square.length*50,50);
        square.forEach((row) => {
            // translate(-row.length*50,50,0);
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