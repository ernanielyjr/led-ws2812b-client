const baseURL = 'http://192.168.0.123'
const debounceDelay = 200;

// TODO: get from remote
const modes = ["Static", "Blink", "Breath", "Color Wipe", "Color Wipe Inverse", "Color Wipe Reverse", "Color Wipe Reverse Inverse", "Color Wipe Random", "Random Color", "Single Dynamic", "Multi Dynamic", "Rainbow", "Rainbow Cycle", "Scan", "Dual Scan", "Fade", "Theater Chase", "Theater Chase Rainbow", "Running Lights", "Twinkle", "Twinkle Random", "Twinkle Fade", "Twinkle Fade Random", "Sparkle", "Flash Sparkle", "Hyper Sparkle", "Strobe", "Strobe Rainbow", "Multi Strobe", "Blink Rainbow", "Chase White", "Chase Color", "Chase Random", "Chase Rainbow", "Chase Flash", "Chase Flash Random", "Chase Rainbow White", "Chase Blackout", "Chase Blackout Rainbow", "Color Sweep Random", "Running Color", "Running Red Blue", "Running Random", "Larson Scanner", "Comet", "Fireworks", "Fireworks Random", "Merry Christmas", "Fire Flicker", "Fire Flicker (soft)", "Fire Flicker (intense)", "Circus Combustus", "Halloween", "Bicolor Chase", "Tricolor Chase", "Custom 0", "Custom 1", "Custom 2", "Custom 3", "Custom 4", "Custom 5", "Custom 6", "Custom 7"];

const defaultConfig = {
  mode: 0,
  color: '#FF00FF',
  brightness: 255,
  speed: 200,
  autoCycle: false,
  cycleTime: 5000, // TODO:
};

let debounceTimer;
const lastConfig = JSON.parse(localStorage.getItem('last-config') || '{}');

// TODO: get from remote
let config = {
  ...defaultConfig,
  ...lastConfig,
  // get from remote config
};

let lastBrightness = config.brightness || 255;

function map(x, in_min, in_max, out_min, out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

function sendConfigs() {
  localStorage.setItem("last-config", JSON.stringify(config))

  let query = '';
  query += `m=${config.mode}&`;
  query += `c=${parseInt((config.color || '').replace(/#/g, ""), 16)}&`;
  query += `b=${config.brightness}&`;
  query += `s=${config.speed}&`;

  // fazer um debounce
  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', `${baseURL}/set?${query}`, true);
  xhttp.send();

}

function setConfig(newConfig) {
  config = newConfig;

  document.getElementById(`mode-${config.mode || 0}`).checked = true;
  document.getElementById("color").value = config.color;
  document.getElementById("brightness").value = config.brightness;

  document.getElementById("brightness-text").innerText = `${parseInt(map(config.brightness, 0, 255, 0, 100))}%`;
  document.getElementById("speed").value = config.speed;
  document.getElementById("speed-text").innerText = `${config.speed} ms`;

  const onButton = document.getElementById("on");
  const offButton = document.getElementById("off");

  if (config.brightness <= 0) {
    onButton.style.display = 'inline';
    offButton.style.display = 'none';
  } else {
    onButton.style.display = 'none';
    offButton.style.display = 'inline';
  }

  if (debounceTimer) {
    clearTimeout(debounceTimer);
  }
  debounceTimer = setTimeout(() => sendConfigs(), debounceDelay);
}

function on() {
  setConfig({
    ...config,
    brightness: lastBrightness,
  });
}

function off() {
  lastBrightness = config.brightness;
  setConfig({
    ...config,
    brightness: 0,
  });
}

function resetConfig() {
  setConfig(defaultConfig);
}

function onMode(mode) {
  setConfig({
    ...config,
    mode,
  });
}

function onColor(color) {
  setConfig({
    ...config,
    color,
  });
}

function onBrightness(brightness) {
  setConfig({
    ...config,
    brightness,
  });
}

function onSpeed(speed) {
  setConfig({
    ...config,
    speed,
  });
}

window.addEventListener('DOMContentLoaded', (e) => {
  const parentElement = document.getElementById('modes');

  for (let i = 0; i < modes.length; i++) {
    const mode = modes[i];

    const radio = document.createElement("input");
    radio.id = `mode-${i}`;
    radio.name = "mode-radio";
    radio.type = "radio";
    radio.value = i;
    radio.onclick = () => onMode(i);

    const text = document.createElement("span");
    text.innerText = mode;

    const label = document.createElement("label");
    label.appendChild(radio);
    label.appendChild(text);

    parentElement.appendChild(label);
  }

  setConfig(config);
});
