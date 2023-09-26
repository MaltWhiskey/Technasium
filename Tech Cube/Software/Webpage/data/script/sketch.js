function draw() {
  orbitControl(1,1,0);
  background(100,100,100);
  ambientLight(200, 200, 200);
  let x = 0;
  let y = 0;
  let z = 0;
  cube.forEach((square) => {
    z++;
    // translate(0,-square.length*50,50);
    if ((z == 1 && r1.checked()) | (z == 2 && r2.checked()) | (z == 3 && r3.checked()) | (z == 4 && r4.checked())){
      square.forEach((row) => {
        y++;
        // translate(-row.length*50,50,0);
          row.forEach((pixel) => {
            x++;
            push();
            translate(-125 + z *50,-125 + y *50,-125 + x *50);
            // translate(50,0,0);
            fill(pixel.color());
            box(35,35,35);
            pop();  
          });
          x = 0;
        })
        y = 0;
    };
  });

}
