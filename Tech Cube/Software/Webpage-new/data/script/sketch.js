var lastmousestate;
function draw() {
  orbitControl(1,1,0);
  background(100,100,100);
  ambientLight(200, 200, 200);
  let x = 0;
  let y = 0;
  let z = 0;
  cube.forEach((square) => {
    z++;
    if ((rowchecks[z-1].checked())){
      square.forEach((row) => {
        y++;
          row.forEach((pixel) => {
            x++;
            push();
            translate((-50-50*size.z)/2 + z *50,(-50-50*size.y)/2 + y *50,(-50-50*size.x)/2 + x *50);
            fill(pixel.style("background-color"));
            box(35,35,35);
            pop();  
          });
          x = 0;
        })
        y = 0;
      };
    });
    if (lastmousestate && !mouseIsPressed && updateneeded){
      try {
        update()
      } catch (e) {
        if (!e instanceof DOMException){
          throw(e)
        }
      }
    }
    lastmousestate = mouseIsPressed

  }
  