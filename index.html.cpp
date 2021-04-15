#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>KRK Monitor Led Control</title>
  <link rel="stylesheet" href="main.css" />
  <script type="text/javascript" src="main.js"></script>
</head>

<body oncontextmenu="return false;" onselectstart="return false;">
  <h1>KRK Monitor Led Control</h1>

  <div class="param-item">
    <button type="button" class="primary" id="on" onclick="on()">LIGAR</button>
    <button type="button" class="primary" id="off" onclick="off()">DESLIGAR</button><br />
  </div>
  <hr />

  <div id="pressets" class="moes param-item"></div>
  <hr />

  <div class="param-item">
    <label>Cor:</label>
    <input id="color" type="color" oninput="onColor(this.value)" />
  </div>

  <div class="param-item">
    <label>Brilho:</label>
    <input id="brightness" type="range" min="0" max="255" value="100" step="5" oninput="onBrightness(this.value)" />
    <span id="brightness-text"></span>
  </div>

  <div class="param-item">
    <label>Velocidade:</label>
    <input id="speed" type="range" min="5" max="5000" step="5" oninput="onSpeed(this.value)" />
    <span id="speed-text"></span>
  </div>

  <div class="param-item">
    <select id="effects" oninput="onEffect(this.value)"></select>
  </div>

  <div>
    <button type="button" onclick="savePresset()">Salvar configuração</button><br />
  </div>

  <hr />
  <div class="param-item">
    <button type="button" class="danger" onclick="wifi_reset()">DESCONECTAR DO WIFI</button>
    <button type="button" class="danger" onclick="restart()">REINICIAR</button><br />
  </div>
</body>

</html>
)=====";
