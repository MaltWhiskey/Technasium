function exportdata(data){
    animatieframes[frameselect.value()-0] = storecube(cube);
    framerate = fps.value()-0
    if (framerate < 0) {
        framerate = -1/framerate
    }
    frames = animatieframes.length
    base = "#include <stdint.h>\n#define ANIM1_DATA_FRAME_RATE "+framerate+"\n#define ANIM1_DATA_FRAME_COUNT " + frames +"\n#define ANIM1_DATA_FRAME_WIDTH 16"+"\n#define ANIM1_DATA_FRAME_HEIGHT 4 \n\n/*autogen anim data */\n\nstatic const uint32_t anim1_data_data["+frames+"][64] = {\n"
    data.forEach(frame => {
        base += "{"
        for (let x = 0; x < frame.length; x++){
            base += "\n"
            for (let y = 0; y < frame[0][0].length; y++){
                for (let z = 0; z < frame[0].length; z++){
                    const color = frame[y][x][z].replace("#", "");
                    base += "0x"+"ff" +color.slice(4, 8)+color.slice(2, 4)+color.slice(0, 2) + ","
                }
            }
        }
        base = base.replace(/,\n?$/, "") + "\n},\n"
    });
    result = base.replace(/,\n?$/, "") +"\n};"
    return result
}
function importdata(file) {
    data = file.data
    // if (file.type != "text"){alert(file.name+" is not a text file")}
    console.log(data)
    console.log(file.name)
    if (!data.includes("ANIM1")){
        data = atob(data.replace(/^.*base64,/,""))
    }
    console.log(data)
    framerate = data.replace(/^[\s\S]*ANIM1_DATA_FRAME_RATE /,"").replace(/\n[\s\S]*$/,"")-0
    leds = data.replace(/^[\s\S]*\[64\] = /,"").replaceAll(";","").replaceAll("{","[").replaceAll("}","]").replaceAll("\n","").replaceAll("0xff","").replace(/\[|\]/g,'').split(',')
    frames=[]
    let i = 0
    let ii = 0
    let iii = 0
    let frame = []
    let clearframe = () =>{
        frames.push(frame)
        frame=[]
        for (z = 0;z<4;z++){
            let square = []
            for (y = 0;y<4;y++){
                let row = []
                for (x = 0;x<4;x++){
                    row.push("")
                }
                square.push(row)
            }
        frame.push(square)
        }
    }
    clearframe()
    leds.forEach(led => {
        frame[ii][iii][i] = "#"+led.slice(4,6) +led.slice(2,4)+led.slice(0,2)
        i++
        if (i > 3) {
            i = 0
            ii++
            if (ii > 3) {
                ii=0
                iii++
                if (iii > 3){
                    iii=0
                    clearframe()
                }
            }
            
        }
    });
    frames.shift()
    if (framerate < 1 && framerate != 0){
        fps.value(Math.round(-1/framerate))
    } else {
        fps.value(framerate)
    }
    console.log(framerate)
    console.log(frames , leds.length)
    animatieframes = [...frames]
    loadcube(cube,animatieframes[selected])
    animatieframes.forEach((_frame,index)=>{
        frameselect.option("frame " + index,index+"");
    })
}