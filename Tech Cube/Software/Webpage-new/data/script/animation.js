class anim {
    jsondata = {
        playing:false,
        current:0,
        frames:[],
        fps:3
    }
    constructor(size){
        let frame = [];
        for (let z = 0; z < size.z; z++) {
            let a = []
                for (let y = 0; y < size.y; y++) {
                    let b = []
                    for (let x = 0; x < size.x; x++) {
                        b.push("#000000");
                    }
                a.push(b);
            }
        frame.push(a);
        }
        this.jsondata.frames=[frame]
    }
    get fps() {
        return this.jsondata.fps
    }
    get frameamount() {
        return this.jsondata.frames.length
    }
    get frames() {
        return this.jsondata.frames
    }
    get current() {
        if (this.jsondata.current > this.frameamount-1){
            this.jsondata.current = this.frameamount-1
        }
        return this.jsondata.current + 1
    }
    get frame() {
        return this.frames[this.current-1]
    }
    setfps(fpstoset = 3,send = true) {
        this.jsondata.fps = fpstoset
        if (send) {
        this.send("fps")
        }
    }
    select(frameindex,send = true) {
        if (frameindex < 1 || frameindex > this.frameamount){
            return
        }
        this.jsondata.current = frameindex - 1
        loadcube(cube, this.frame);
        frameselect.selected(this.current)
        if (send) {
            this.send("current")
        }
        return this.current
    }
    setframe(frame){
        this.jsondata.frames[this.current-1] = frame
        loadcube(cube, animation.frame);
        this.send()
        return this.current
    }
    setframes(frames){
        this.jsondata.frames = frames
        loadcube(cube, animation.frame);
        addframeselect()
        this.send()
        return this.current
    }
    addframe(color = "#000000",copy = false) {
        let frame = [];
        for (let z = 0; z < size.z; z++) {
            let a = []
                for (let y = 0; y < size.y; y++) {
                    let b = []
                    for (let x = 0; x < size.x; x++) {
                        b.push(color);
                    }
                a.push(b);
            }
        frame.push(a);
        }
        this.jsondata.frames = this.jsondata.frames.concat(this.jsondata.frames.splice(this.current,-1,(!copy ? frame : this.frame)))
        this.select(this.current+1,false)
        this.send()
    }
    copyframe() {
        return this.addframe("#123456",true)
    }
    removeframe() {
        this.jsondata.frames.splice(this.current - 1, 1);
        if (this.frameamount < 1) {
            this.addframe()
        }
        this.select(this.current,false)
        this.send()
    }
    play() {
        this.jsondata.playing = true
        this.send("playing")
    }
    stop() { 
        this.jsondata.playing = false
        this.send("playing")
    }
    send(atr = "all") {
        console.log("sending: "+atr+" = ",this.jsondata[atr])
        try {
            if (atr == "all"){
                send()
            } else {
                socket.send("{\""+atr+"\":"+JSON.stringify(this.jsondata[atr])+"}")
            }
        } catch (e) {
            if (!e instanceof DOMException){
              throw(e)
            }
        }
    }
}