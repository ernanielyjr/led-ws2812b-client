#include <pgmspace.h>

char main_js[] PROGMEM = R"=====(
const baseURL = '/'; // http://192.168.0.123/

let debounceTimer;
const debounceDelay = 200;

let touchTimer;
const touchDuration = 500;

const lastConfig = JSON.parse(localStorage.getItem('last-config') || '{}');

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

function request(method, url,) {
  return new Promise((resolve, reject) => {
    var xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.onload = () => resolve(xhttp.response);
    xhttp.onerror = () => reject();
    xhttp.send();
  })
}

async function sendConfigs() {
  localStorage.setItem("last-config", JSON.stringify(config))

  let query = '';
  query += `effect=${config.effect}&`;
  query += `color=${parseInt((config.color || '').replace(/#/g, ""), 16)}&`;
  query += `brightness=${config.brightness}&`;
  query += `speed=${config.speed}&`;

  await request("POST", `${baseURL}config?${query}`);
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

window.addEventListener('DOMContentLoaded', async (e) => {
  const effectsParent = document.getElementById('effects');

  let activeEffects = [];

  try {
    const response = JSON.parse(await request("GET", `${baseURL}config`));
    activeEffects = response.effects
      // UNDO: .filter(effect => effect.enabled)
      .sort((a, b) => a.name.localeCompare(b.name));
  } catch (error) {
    console.log('error', error);
  }

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
