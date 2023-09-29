var cube = [];
var animatieframes = [];
var selected = 0;
var play;
var fps = 3;
var playing = false
var socket;
var send
var frameselect
function addframeselect() {
  if (frameselect != null) {
    frameselect.remove()
  }
  frameselect = createSelect();
  frameselect.position(10, 530);
  // load the correct frame when frame changed
  frameselect.changed(framechanged);
  animatieframes.forEach((_frame, index) => {
    frameselect.option("frame " + index, index +1+ "");
  })
}
function setup() {
  connect();
  createCanvas(500, 500, WEBGL).position(500, 0);
  // create cube color pickers
  for (let z = 0; z < 4; z++) {
    let a = []
    for (let y = 0; y < 4; y++) {
      let b = []
      for (let x = 0; x < 4; x++) {
        b.push(createColorPicker("#000000"));
      }
      a.push(b);
    }
    cube.push(a);
  }
  // set to position
  let z = 0;
  cube.forEach((square) => {
    let y = 0;
    square.forEach((row) => {
      let x = 0;
      row.forEach((pixel) => {
        pixel.position(140 + x * 60, 10 + y * 30 + z * 130);
        pixel.input(send)
        x++;
      });
      y++;
    });
    z++;
  });
  frameRate(60);
  camera(200, 200, 200);
  // show layers input
  r1 = createCheckbox('show row 1', true);
  r2 = createCheckbox('show row 2', true);
  r3 = createCheckbox('show row 3', true);
  r4 = createCheckbox('show row 4', true);
  r1.position(10, 10);
  r2.position(10, 130);
  r3.position(10, 270);
  r4.position(10, 400);
  // add frame select input
  addframeselect();
  frameselect.option("frame " + animatieframes.length + "", animatieframes.length +1+ "");
  selected = frameselect.value() - 0
  // add frame input
  addframe = createButton("add frame");
  addframe.position(10, 560);
  removeframe = createButton("remove frame");
  removeframe.position(10, 620);
  copyframe = createButton("copy frame");
  copyframe.position(10, 590);
  // add clear button
  clear = createButton("clear frame");
  clear.position(100, 530); randomcube
  clear.mousePressed(clearcube)
  // add randomize frame button
  clear = createButton("randomize frame");
  clear.position(100, 560);
  clear.mousePressed(randomcube)
  // add frame when the add frame button is presed
  addframe.mousePressed(() => {
    console.log("add frame " + (animatieframes.length + 1));
    animatieframes[frameselect.value() - 0] = storecube(cube);
    clearcube();
    animatieframes.push(storecube(cube))
    addframeselect()
    frameselect.selected(animatieframes.length -1 + "");
    selected = frameselect.value() - 0
  });
  copyframe.mousePressed(() => {
    console.log("add frame " + (animatieframes.length + 1));
    animatieframes[frameselect.value() - 0] = storecube(cube);
    console.log(animatieframes)
    animatieframes = animatieframes.concat(animatieframes.splice(frameselect.value() - 0,-1,storecube(cube))) 
    console.log(animatieframes)
    selected = frameselect.value() - 0 +1
    addframeselect()
    frameselect.selected(selected)
    loadcube(cube, animatieframes[selected]);
  });
  // remove frame when the remove frame button is presed
  removeframe.mousePressed(() => {
    if (animatieframes.length > 1) {
      selected--
      animatieframes.splice(frameselect.value() - 0, 1);
      addframeselect()
      loadcube(cube, animatieframes[selected]);
    } else {
      clearcube()
    }
  })
  // add play button
  clear = createButton("play");
  clear.position(230, 530);
  clear.mousePressed(() => { if (!playing) { frame() } })
  // add stop button
  clear = createButton("stop");
  clear.position(230, 560);
  clear.mousePressed(() => {
    clearTimeout(play);
    playing = false
  })
  fps = createInput(3, "Number")
  fps.position(280, 560)
  fps.attribute("placeholder", "FPS")
  fps.size(40)
  fpslabel = createP('FPS')
  fpslabel.position(280, 516)
  fpslabel.style('font-size', '16px');
  exportbtn = createButton("export")
  exportbtn.position(340, 530)
  exportbtn.mousePressed(() => {
    let file = createWriter('Anim1.h');
    file.write([exportdata(animatieframes)]);
    file.close();
  })
  importbtn = createFileInput(importdata)
  importbtn.hide()
  importbtn.id("importbtn")
  importbtn2 = createButton("import")
  importbtn2.attribute("onclick", "document.getElementById('importbtn').click()")
  importbtn2.position(340, 560)
}
function framechanged() {
  animatieframes[selected] = storecube(cube);
  selected = frameselect.value() - 0
  loadcube(cube, animatieframes[selected]);
}
function frame() {
  playing = true
  let framerate = fps.value() - 0
  let frametime = 1000 / framerate
  if (framerate < 1) {
    frametime = -1000 * framerate
  }
  if (frametime != 0) {
    frameselect.value((((frameselect.value() - 0) + 1) % animatieframes.length) + "")
    framechanged()
  }
  play = setTimeout(frame, frametime);
}
function randomcube() {
  cube.forEach((square) => {
    square.forEach((row) => {
      row.forEach((pixel) => {
        pixel.value(color(random(255), random(255), random(255)).toString("#rrggbb"))
      });
    });
  });
  send()
}
function clearcube() {
  cube.forEach((square) => {
    square.forEach((row) => {
      row.forEach((pixel) => {
        pixel.value("#00000")
      });
    });
  });
  send()
}
function storecube(cube) {
  let a = []
  cube.forEach((square) => {
    let b = []
    square.forEach((row) => {
      let c = []
      row.forEach((pixel) => {
        c.push(pixel.value())
      });
      b.push(c)
    });
    a.push(b)
  });
  return a
}
function loadcube(cube, load) {
  let z = 0
  load.forEach((square) => {
    let y = 0
    square.forEach((row) => {
      let x = 0
      row.forEach((pixel) => {
        cube[z][y][x].value(pixel)
        x++
      });
      y++
    });
    z++
  });
  send()
  return cube
}