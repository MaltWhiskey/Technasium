var cube = [];
var animation
var selected = 0;
var play;
var playing = false
var socket;
var frameselect;
var updateneeded = false;
var size = {
  x:4,
  y:4,
  z:4
}
var animation = new anim(size)
var aftercolorpos;
function addframeselect() {
  if (frameselect != null) {
    frameselect.remove()
  }
  frameselect = createSelect();
  frameselect.position(aftercolorpos, 530);
  // load the correct frame when frame changed
  frameselect.changed(framechanged);
  for (let i = 0; i < animation.frameamount;i++){
    frameselect.option("frame " + (i + 1), i +1+ "");
  }
  frameselect.selected(animation.current + "")
}
function update(){
  animation.setframe(storecube(cube))
  updateneeded = false
}
function load(){
  addframeselect()
  fps.value(animation.fps)
  loadcube(cube,animation.frame)
}
function setup() {
  // size = {
  //   x:round(random(1,10)),
  //   y:round(random(1,10)),
  //   z:round(random(1,10))
  // }
  console.log(size)
  httpGet("/hostname",(msg)=>{createElement('h3', msg).position(aftercolorpos,485).mouseClicked(()=>{if (location.host != msg) {window.location.href = window.location.href.replace(location.host,msg)}});},()=>{});
  connect();
  aftercolorpos = 155 + max(21, 175 / max(size.z, size.y, size.z) + 1) * size.x
  createCanvas(500, 500, WEBGL).position(aftercolorpos, 0);
  colorpicker = createColorPicker(color(random(255), random(255), random(255)).toString("#rrggbb"))
  colorpicker.size(100,55)
  colorpicker.position(aftercolorpos+105,590)
  for (let z = 0; z < size.z; z++) {
    let a = []
    for (let y = 0; y < size.y; y++) {
      let b = []
      for (let x = 0; x < size.x; x++) {
        pixel = createButton("");
        pixel.size(max(20,175/max(size.z,size.y,size.z)),max(20,175/max(size.z,size.y,size.z)))
        pixel.style('background-color', '#000000');
        pixel.position(140 + x * max(21,175/max(size.z,size.y,size.z)+1), 10 + y * max(21,175/max(size.z,size.y,size.z)+1) + z*10+z * max(21,175/max(size.z,size.y,size.z)+1)*size.y);
        function callback() {
          if (mouseIsPressed && mouseButton == LEFT){
            if (keyIsPressed && key == 'Control'){
              colorpicker.value(color(cube[z][y][x].style('background-color')).toString("#rrggbb"))
            } else {
            cube[z][y][x].style('background-color', colorpicker.value());
            updateneeded = true
            }
          } 
        };

        
        pixel.mousePressed(callback)
        pixel.mouseOver(callback)
        b.push(pixel)
      }
      a.push(b);
    }
    cube.push(a);
  }

  frameRate(60);
  camera(50*size.x,50*size.y,50*size.z) 
  // show layers input
  rowchecks = []
  for (row = 0;row < size.z;row++){
    rowchecks[row] = createCheckbox('show row '+(row + 1), true);
    rowchecks[row].position(10, 10 + row*10+row * max(21,175/max(size.z,size.y,size.z)+1)*size.y);
  }
  // add frame select input
  addframeselect();
  selected = frameselect.value() - 0
  // add frame input
  addframe = createButton("add frame");
  addframe.position(aftercolorpos, 560);
  removeframe = createButton("remove frame");
  removeframe.position(aftercolorpos, 620);
  copyframe = createButton("copy frame");
  copyframe.position(aftercolorpos, 590);
  // add clear button
  clear = createButton("clear frame");
  clear.position(aftercolorpos+90, 530); randomcube
  clear.mousePressed(clearcube)
  // add randomize frame button
  clear = createButton("randomize frame");
  clear.position(aftercolorpos+90, 560);
  clear.mousePressed(randomcube)
  // add frame when the add frame button is presed
  addframe.mousePressed(() => {
    console.log("add frame " + (animation.frameamount +1));
    animation.addframe()
    addframeselect()
  });
  copyframe.mousePressed(() => {
    console.log("copy frame " + (animation.frameamount +1));
    animation.copyframe()
    addframeselect()
  });
  // remove frame when the remove frame button is presed
  removeframe.mousePressed(() => {
    animation.removeframe()
  })
  // add play button
  playbtn = createButton("play");
  playbtn.position(aftercolorpos+220, 530);
  playbtn.mousePressed(() => {animation.play()})
  // add stop button
  clear = createButton("stop");
  clear.position(aftercolorpos+220, 560);
  clear.mousePressed(() => {animation.stop()})
  fps = createInput(3, "Number")
  fps.position(aftercolorpos+270, 560)
  fps.attribute("placeholder", "FPS")
  fps.size(40)
  fps.input(updatefps)
  fpslabel = createP('FPS')
  fpslabel.position(aftercolorpos+270, 516)
  fpslabel.style('font-size', '16px');
  exportbtn = createButton("export")
  exportbtn.position(aftercolorpos+330, 530)
  exportbtn.mousePressed(() => {
    let file = createWriter('Anim1.h');
    file.write([exportdata(animation.frames)]);
    file.close();
  })
  importbtn = createFileInput(importdata)
  importbtn.hide()
  importbtn.id("importbtn")
  importbtn2 = createButton("import")
  importbtn2.attribute("onclick", "document.getElementById('importbtn').click()")
  importbtn2.position(aftercolorpos+330, 560)
}
function framechanged() {
  animation.select(frameselect.value())
}
function updatefps() {
  if (fps.value() == ""){
    return
  }
  let framerate = fps.value()-0
  animation.setfps(framerate)
  if (framerate < 1) {
    animation.setfps(-1/framerate)
  } else{
    animation.setfps(framerate)
  }
}
function randomcube() {
  cube.forEach((square) => {
    square.forEach((row) => {
      row.forEach((pixel) => {
        pixel.style('background-color',color(random(255), random(255), random(255)).toString("#rrggbb"))
      });
    });
  });
  update()
}
function clearcube() {
  cube.forEach((square) => {
    square.forEach((row) => {
      row.forEach((pixel) => {
        pixel.style('background-color',"#000000")
      });
    });
  });
  update()

}
function storecube(cube) {
  let a = []
  cube.forEach((square) => {
    let b = []
    square.forEach((row) => {
      let c = []
      row.forEach((pixel) => {
        c.push(color(pixel.style('background-color')).toString("#rrggbb"))
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
        cube[z][y][x].style('background-color',pixel)
        x++
      });
      y++
    });
    z++
  });
  return cube
}