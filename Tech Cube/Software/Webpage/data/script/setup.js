let cube;
let colorPicker;
const socket = new WebSocket("ws://" + location.host + "/ws");
function setup() {
  createCanvas(500, 500, WEBGL).position(500, 0);
  // sendMessage({hello:"world"});
  cube = [
    [
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ]
    ],
    [
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ]
    ],
    [
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ]
    ],
    [
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ],
      [
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000'),
        createColorPicker('#000000')
      ]
    ],
  ];
  // createCanvas(500, 500, WEBGL).position(0,0);
  frameRate(60);
  camera(200, 200, 200);
  let x = 0;
  let y = 0;
  let z = 0;
  r1 = createCheckbox('show row 1', true);
  r2 = createCheckbox('show row 2', true);
  r3 = createCheckbox('show row 3', true);
  r4 = createCheckbox('show row 4', true);
  r1.position(10, 10);
  r2.position(10, 130);
  r3.position(10, 270);
  r4.position(10, 400);
  cube.forEach((square) => {
    // translate(0,-square.length*50,50);
    square.forEach((row) => {
      // translate(-row.length*50,50,0);
      row.forEach((pixel) => {
        pixel.position(140 + x * 60, 10 + y * 30 + z * 130);
        pixel.input(send)
        x++;
      });
      x = 0;
      y++;
    });
    y = 0;
    z++;
  });
}
