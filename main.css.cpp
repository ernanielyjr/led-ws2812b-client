#include <pgmspace.h>

char main_css[] PROGMEM = R"=====(
body {
  font-family: Arial, sans-serif;
  margin: 20px;
  padding: 0;
  background-color: #202020;
  color: #CCC;
  text-align: left;
  font-size: 13px;
  cursor: default;
}

h1 {
  color: #fff;
  border-bottom: solid 2px #444;
  padding-bottom: 10px;
}

hr {
  height: 2px;
  border: none;
  background: #444;
  margin: 0;
  padding: 0;
}

.param-item {
  margin: 20px 0;
}

button,
#pressets > label > span {
  color: #CCC;
  font-size: 13px;
  display: inline-block;
  border: solid 1px #444;
  background: #333;
  padding: 5px 10px;
  margin: 5px;
  border-radius: 5px;
  cursor: pointer;
}

button.primary {
  background-color: #00C814;
  color: #FFF;
  font-weight: bold;
  border-color: #005000;
}

#pressets > label > input {
  display: none;
}

#pressets > label > input:checked + span {
  border-color: #000;
  background: #fff;
  color: #000;
}

#pressets {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
}

.nowrap {
  white-space: nowrap;
}

.left {
  text-align: left;
}
)=====";
