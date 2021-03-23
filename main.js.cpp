#include <pgmspace.h>

char main_js[] PROGMEM = R"=====(
const baseURL = '/'; // http://192.168.0.123/

let debounceTimer;
const debounceDelay = 200;

let touchTimer;
const touchDuration = 500;

const lastConfig = JSON.parse(localStorage.getItem('last-config') || '{}');

// TODO: get from remote
const effects = [
  { id: 0, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Static" },
  { id: 1, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Blink" },
  { id: 2, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Breath" },
  { id: 3, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Color Wipe" },
  { id: 4, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Color Wipe Inverse" },
  { id: 5, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Color Wipe Reverse" },
  { id: 6, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Color Wipe Reverse Inverse" },
  { id: 7, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Color Wipe Random" },
  { id: 8, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Random Color" },
  { id: 9, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Single Dynamic" },
  { id: 10, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Multi Dynamic" },
  { id: 11, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Rainbow" },
  { id: 12, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Rainbow Cycle" },
  { id: 13, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Scan" },
  { id: 14, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Dual Scan" },
  { id: 15, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Fade" },
  { id: 16, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Theater Chase" },
  { id: 17, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Theater Chase Rainbow" },
  { id: 18, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Running Lights" },
  { id: 19, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Twinkle" },
  { id: 20, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Twinkle Random" },
  { id: 21, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Twinkle Fade" },
  { id: 22, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Twinkle Fade Random" },
  { id: 23, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Sparkle" },
  { id: 24, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Flash Sparkle" },
  { id: 25, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Hyper Sparkle" },
  { id: 26, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Strobe" },
  { id: 27, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Strobe Rainbow" },
  { id: 28, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Multi Strobe" },
  { id: 29, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Blink Rainbow" },
  { id: 30, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Chase White" },
  { id: 31, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Chase Color" },
  { id: 32, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Chase Random" },
  { id: 33, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Chase Rainbow" },
  { id: 34, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Chase Flash" },
  { id: 35, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Chase Flash Random" },
  { id: 36, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Chase Rainbow White" },
  { id: 37, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Chase Blackout" },
  { id: 38, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Chase Blackout Rainbow" },
  { id: 39, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Color Sweep Random" },
  { id: 40, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Running Color" },
  { id: 41, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Running Red Blue" },
  { id: 42, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Running Random" },
  { id: 43, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Larson Scanner" },
  { id: 44, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Comet" },
  { id: 45, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Fireworks" },
  { id: 46, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Fireworks Random" },
  { id: 47, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Merry Christmas" },
  { id: 48, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Fire Flicker" },
  { id: 49, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Fire Flicker (soft)" },
  { id: 50, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Fire Flicker (intense)" },
  { id: 51, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Circus Combustus" },
  { id: 52, enabled: true, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Halloween" },
  { id: 53, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Bicolor Chase" },
  { id: 54, enabled: true, singleColor: true, speedRange: { min: 5, max: 5000 }, name: "Tricolor Chase" },
  { id: 55, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 0" },
  { id: 56, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 1" },
  { id: 57, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 2" },
  { id: 58, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 3" },
  { id: 59, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 4" },
  { id: 60, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 5" },
  { id: 61, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 6" },
  { id: 62, enabled: false, singleColor: false, speedRange: { min: 5, max: 5000 }, name: "Custom 7" },
];

const defaultConfig = {
  effect: 0,
  color: '#FF00FF',
  brightness: 255,
  speed: 200,
};

// TODO: get from remote
let config = {
  ...defaultConfig,
  ...lastConfig,
  // TODO: get from remote config
};

let lastBrightness = config.brightness || 255;

function map(x, in_min, in_max, out_min, out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

function sendConfigs() {
  localStorage.setItem("last-config", JSON.stringify(config))

  let query = '';
  query += `m=${config.effect}&`;
  query += `c=${parseInt((config.color || '').replace(/#/g, ""), 16)}&`;
  query += `b=${config.brightness}&`;
  query += `s=${config.speed}&`;

  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', `${baseURL}set?${query}`, true);
  xhttp.send();
}

function setConfig(newConfig) {
  config = newConfig;

  document.getElementById("effects").value = config.effect;
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

function onEffect(effect) {
  console.log('effect', effect)
  setConfig({
    ...config,
    effect,
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

function savePresset(defaultName) {
  const name = prompt("Nome da configuração:", defaultName || "");
  if (!name) {
    return;
  }

  const id = `presset_${name}`;
  if (localStorage.getItem(id) && !confirm("Este nome já está em uso. Deseja sobrescrever?")) {
    return;
  }

  localStorage.setItem(id, JSON.stringify(config));
  listPressets();
}

function createElement(tag, children, attrs) {
  const element = document.createElement(tag);
  if (typeof attrs === 'object' && Object.keys(attrs).length) {
    for (const attr in attrs) {
      element[attr] = attrs[attr];
    }
  }

  if (children) {
    if (typeof children === 'string') {
      element.innerHTML = children;
    } else {
      children = Array.isArray(children) ? children : [children];
      for (const child of children) {
        element.appendChild(child);
      }
    }
  }

  return element;
}

function listPressets() {
  const pressetsParent = document.getElementById('pressets');
  pressetsParent.innerHTML = '';

  const lsItems = [{
    id: 'presset_default',
    name: '[RESTAURAR PADRÃO]',
    presset: defaultConfig,
  }];

  for (var i = 0; i < localStorage.length; i++) {
    const id = localStorage.key(i) || '';
    if (id.startsWith('presset_')) {
      let presset;
      try {
        presset = JSON.parse(localStorage.getItem(id));
      } catch (error) {
        console.error('error', error);
      }

      if (presset) {
        const name = id.substring(8);
        lsItems.push({ id, name, presset });
      }
    }
  }

  for (const item of lsItems) {
    const radio = createElement("input", null, {
      id: `presset-${i}`,
      name: "presset-radio",
      type: "radio",
      value: item.id,
      onclick: () => setConfig(item.presset),
    });

    function clearTimer() {
      if (touchTimer) {
        clearTimeout(touchTimer);
      }
    }

    function deleteItem() {
      if (item.id === 'presset_default') {
        return;
      }

      if (!confirm(`Deseja excluir "${item.name}"?`)) {
        return;
      }

      localStorage.removeItem(item.id)
      listPressets();
    }

    function onStart(e) {
      e.preventDefault();
      e.stopPropagation();
      clearTimer();

      const event = e || window.event;
      if (
        ("which" in event && event.which === 3) ||
        ("button" in event && event.button === 2)
      ) {
        deleteItem();
        return false;
      }

      touchTimer = setTimeout(() => deleteItem(), touchDuration);
    }

    pressetsParent.appendChild(
      createElement("label", [
        radio,
        createElement("span", item.name)
      ], {
        ontouchstart: onStart,
        onmousedown: onStart,
        ontouchend: clearTimer,
        onmouseup: clearTimer,

      })
    );
  }

}

window.addEventListener('DOMContentLoaded', (e) => {
  const effectsParent = document.getElementById('effects');

  const activeEffects = effects
    .filter(effect => effect.enabled)
    .sort((a, b) => a.name.localeCompare(b.name));

  for (let i = 0; i < activeEffects.length; i++) {
    const effect = activeEffects[i];
    effectsParent.appendChild(
      createElement("option", effect.name, { value: effect.id })
    );
  }

  setConfig(config);

  listPressets();
});
)=====";
