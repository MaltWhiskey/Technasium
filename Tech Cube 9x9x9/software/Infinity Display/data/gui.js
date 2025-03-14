// config holds gui.json configuration object
var config = null;
// websocket holds connection to the server
var websocket = null;

// Creates a label with or without an id and some content
function createLabel(text, key) {
  return `<label ${key ? "id=" + key : ""}>${text}</label>`;
}
// Creates a slider with an id, some values and update method
function createSlider(val, key, lbl) {
  return `<input type="range" min=${val.min} max=${val.max}
              value=${val.value} step=${val.step}
              id=${key} oninput="update('${key}', value, '${lbl}')"/>`;
}
// Creates a number with an id, some values and update method
function createNumber(val, key) {
  return `<input type="number" min=${val.min} max=${val.max}
              value=${val.value} step=${val.step}
              id=${key} oninput="update('${key}', value)"/>`;
}
// Creates a switch with an id, checked status and update method
function createSwitch(val, key) {
  return `<label class="switch">
              <input id=${key} type="checkbox"
              ${val.value ? "checked" : ""}
              oninput="update('${key}', ${key}.checked)"/>
              <span class="switch-slider"></span></label>`;
}
// Creates an input with an id, value and update method
function createInput(val, key) {
  return `<input type="text" id=${key}
              oninput="update('${key}', value)"
              value="${val.value}">`;
}

// Create an element of specific type
function createType(val, key) {
  let gui = '<div class="input">';
  if (val.type == "slider") {
    // create a label for the name of the property
    gui += createLabel(val.name) + " ";
    // create a label for the value with id = l+id
    gui += createLabel(val.value, "l" + key);
    // create the slider
    gui += createSlider(val, key, "l" + key);
  } else if (val.type == "number") {
    gui += createLabel(val.name);
    gui += createNumber(val, key);
  } else if (val.type == "checkbox") {
    gui += createLabel(val.name);
    gui += createSwitch(val, key);
  } else if (val.type == "text") {
    gui += createLabel(val.name);
    gui += createInput(val, key);
  }
  return gui + '</div>';
}

// Create the selector at the top of the screen
function createHeader() {
  let gui = "";
  let aidx = 0;
  let keys = Object.keys(config);
  let vals = Object.values(config);
  gui += `<select id="selector" onchange=selectorChanged(value)>`;
  for (let i = 0; i < keys.length; i++) {
    gui += `<optgroup label="${vals[i].name}">`;
    let subkeys = Object.keys(vals[i]);
    let subvals = Object.values(vals[i]);
    for (let j = 0; j < subkeys.length; j++) {
      if (typeof subvals[j] == "object") {
        let id = keys[i] + "." + subkeys[j];
        gui += `<option value="${id}">${subvals[j].name}</option>`;
      }
    }
  }
  gui += `</select>`;
  document.getElementById("header").innerHTML = gui;
}

// get the selector as name
function getSelectorItemName() {
  return document.getElementById("selector").value;
}
// get the selector as object
function getSelectorItemObject() {
  let keys = getSelectorItemName().split(".");
  return config[keys[0]][keys[1]];
}

// Creates the specified user interface
function createGui() {
  let gui = "";
  let item = getSelectorItemObject();
  let keys = Object.keys(item);
  let vals = Object.values(item);
  for (let i = 0; i < keys.length; i++) {
    if (typeof vals[i] == "object") {
      gui += createType(vals[i], keys[i]);
    }
  }
  document.getElementById("gui").innerHTML = gui;
}

// Save the settings on the webserver
function createFooter() {
  let gui = `<input id="save_btn" type="submit" value="Save" onclick="saveConfig()"/>`;
  document.getElementById("footer").innerHTML = gui;
}
function saveConfig() {
  executeCommand(`{"event":"save"}`);
}

// Reset the settings on the webserver
function createReset() {
  let gui = `<input id="reset_btn" type="submit" value="Factory Reset" onclick="resetConfig()"/>`;
  document.getElementById("reset").innerHTML = gui;
}
function resetConfig() {
 let text = "Are you sure you want to Factory Reset and reboot?";
  if (confirm(text) == true) {
    executeCommand(`{"event":"reset"}`);
  }
}

// Creates joyStick in the user interface
function createJoyStick() {
  let params = {};
  params.internalFillColor = "#addaff";
  params.internalStrokeColor = "#aaaaaa";
  params.externalStrokeColor = "#ffffff";
  var joy = new JoyStick('joystick', params, function(stick) {
    // Send the joystick postitions to the webserver
    executeCommand(`{"event":"joystick","x":${stick.x},"y":${stick.y}}`);
  });
}

// Changeing the option selector creates a new gui
// But also new values need to be send through the
// WebSocket, to potentially change the animation
// if one is selected.
function selectorChanged(value) {
  // update the userinterface form selector change
  createGui();
  // send command to webserver if animation is chosen
  let item = getSelectorItemObject();
  if (item.index >= 0) {
    executeCommand(`{"event":"activate","target":${item.index}}`);
  }
}

// Update config, label and execute update command
function update(id, value, lbl = "") {
  // Update the new value in the config
  let item = getSelectorItemObject();
  item[id].value = value;
  // Set the label belonging to id
  if (lbl != "") {
    document.getElementById(lbl).innerHTML = value;
  }
  // Execute the update command on the webserver
  let key = getSelectorItemName();
  // check the type specified in the json
  if (item[id].type == "text") {
    value = '"' + value + '"';
  }
  let keys = key.split(".");
  executeCommand(`{"event":"update","${keys[0]}":{"${keys[1]}":{"${id}":{"value":${value}}}}}`
  );
}

// Call this to connect to the WebSocket server
function wsConnect() {
  let protocol =
    window.location.protocol === "https:" ? "wss://" : "ws://";
  let location = window.location.host + ":1337";
  try {
    websocket = new WebSocket(protocol + location);
    websocket.onopen = function (evt) {
      console.log("Opend: " + evt);
    };
    websocket.onclose = function (evt) {
      console.log("Closed: " + evt);
      // if websocket is closed immediately make a new connection
      wsConnect();
    };
    websocket.onmessage = function (evt) {
      console.log("Received: " + evt.data);
      let obj = JSON.parse(evt.data);          
      if(obj.event == "update") {
        delete obj.event;
        let key1 = Object.keys(obj)[0];
        let val1 = Object.values(obj)[0];
        let key2 = Object.keys(val1)[0];
        let val2 = Object.values(val1)[0];
        let key3 = Object.keys(val2)[0];
        let val3 = Object.values(val2)[0];
        let key4 = Object.keys(val3)[0];
        let val4 = Object.values(val3)[0];
        if(config[key1][key2][key3][key4] != val4) {
          config[key1][key2][key3][key4] = val4;
          createGui();
        }
      }
    };
    websocket.onerror = function (evt) {
      console.log("ERROR: " + evt);
    };
  }
  catch(e) {
    console.log("ERROR: " + e);
  }
}

// WebSocket communications
function executeCommand(command) {
  if (websocket && websocket.readyState == WebSocket.OPEN) {
    websocket.send(command);
    console.log(command);
  }
}

// Generating the complete gui from code
function loadGui() {
  let request = new XMLHttpRequest();
  request.onload = function () {
    config = request.response;
    // first create the header selector
    createHeader();
    // gui is dependent on selector, create gui next
    createGui();
    // create the save button
    createFooter();
    // create JoyStick
    createJoyStick();
    // create reset button
    createReset();
  };
  request.open("GET", "gui.json");
  request.responseType = "json";
  request.send();
}