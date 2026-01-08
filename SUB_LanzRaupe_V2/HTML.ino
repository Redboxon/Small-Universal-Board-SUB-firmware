// This file contains the HTML data for the ESP32.

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<style>
body {
  margin: 0;
  padding: 0;
  background-color: black;
  font-family: sans-serif;
  color: white;
  overflow: hidden;
}

h1 {
  font-size: clamp(1.5rem, 2.5vw, 3rem);
  text-align: center;
  margin: 5px;
}

input[type=range] {
  touch-action: none;
}

.rotated {
	transform: rotate(90deg) translate(2px, 2px);
}

.noselect {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  user-select: none;
}

.container {
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  height: 85vh;
  padding-top: 0vh;
  padding: 1vh;
}

/* Slider Layout */
.slider {
  -webkit-appearance: none;
  appearance: none;
  width: 90%;
  max-width: 800px;
  height: 2vh;
  min-height: 15px;
  border-radius: 5px;
  background: #d2d2d2;
  outline: none;
  margin: 0 auto;
  margin-top: 8vh;
  margin-bottom: 8vh;
  transform: rotate(180deg)
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: clamp(55px, 12vw, 80px);
  height: clamp(55px, 12vw, 80px);
  border-radius: 50%;
  background: brown;
  border: none;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: clamp(40px, 8vw, 80px);
  height: clamp(40px, 8vw, 80px);
  border-radius: 50%;
  background: white;
  cursor: pointer;
}

.button-row {
  display: flex;
  justify-content: center;
  gap: 8vw;
}

.control-button {
  background-color: white;
  color: black;
  padding: clamp(8px, 1.5vh, 15px) clamp(25px, 4vh, 40px);
  border: none;
  border-radius: 10px;
  font-size: clamp(1rem, 2vh, 1.5rem);
  font-weight: bold;
  cursor: pointer;
  box-shadow: 3px 3px #888;
  transform: rotate(90deg);
}

.control-button:active {
  transform: rotate(90deg) translate(2px, 2px);
  box-shadow: none;
}
</style>
</head>
<body class="noselect">
<h1>Lanz Raupe</h1>

<div class="container">
  <!-- Left (Upper) Slider -->
  <input type="range" min="-255" max="255" value="0" class="slider" id="Lthrottle"
    oninput='sendButtonInput("LeftThrottle", value)' ontouchend='resetLeftThrottleSlider()'>

  <!-- Blade Buttons -->
  <div class="button-row">
	<button class="control-button"
      onmousedown='sendButtonInput("bladeDown", "1")'
      onmouseup='sendButtonInput("bladeDown", "0")'
      ontouchstart='sendButtonInput("bladeDown", "1")'
      ontouchend='sendButtonInput("bladeDown", "0")'>
      Blade &#8681;
    </button>
  
    <button class="control-button"
      onmousedown='sendButtonInput("bladeUp", "1")'
      onmouseup='sendButtonInput("bladeUp", "0")'
      ontouchstart='sendButtonInput("bladeUp", "1")'
      ontouchend='sendButtonInput("bladeUp", "0")'>
      Blade &#8679; 
    </button>
	
	<h1 class="rotated">Lanz Raupe</h1>
  </div>

  <!-- Right (lower) Slider -->
  <input type="range" min="-255" max="255" value="0" class="slider" id="Rthrottle"
    oninput='sendButtonInput("RightThrottle", value)' ontouchend='resetRightThrottleSlider()'>
</div>

<script>
function sendButtonInput(cmd, value) {
  fetch(`/${cmd}/${value}`);
}

function resetLeftThrottleSlider() {
  document.getElementById('Lthrottle').value = 0;
  sendButtonInput("LeftThrottle", 0);
}

function resetRightThrottleSlider() {
  document.getElementById('Rthrottle').value = 0;
  sendButtonInput("RightThrottle", 0);
}
</script>

</body>
</html>




  
    <script>
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      
      var websocketCarInput;
      const leftthrottleSlider = document.getElementById('Lthrottle');
      const rightthrottleSlider = document.getElementById('Rthrottle');
      const steeringSlider = document.getElementById('steering');

      function resetLeftThrottleSlider() 
      {
       sendButtonInput("LeftThrottle", 0);
       Lthrottle.value = 0;
       sendButtonInput("LeftThrottle", 0);
      }
      function resetRightThrottleSlider() 
      {
       sendButtonInput("RightThrottle", 0);
       Rthrottle.value = 0;
       sendButtonInput("RightThrottle", 0);
      }

      function resetSteeringSlider() 
      {
       steering.value = 86;
       sendButtonInput("steering", 86);
      }
      
      function initCarInputWebSocket() 
      {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
        websocketCarInput.onmessage = function(event){};        
      }


      function sendButtonInput(key, value) 
      {
       var data = key + "," + value;
       websocketCarInput.send(data);
      }
      let intervalId = null;

    function startSendingButtonInput(action, value) {
    sendButtonInput(action, value); // Send the initial input when the button is pressed
    intervalId = setInterval(function() {
        sendButtonInput(action, value); // Continuously send the input as long as the button is pressed
    }, 10); // You can adjust the interval (in milliseconds) to control the rate of sending 
    }

    function stopSendingButtonInput() {
    clearInterval(intervalId); // Stop sending the input when the button is released
}
      function handleKeyDown(event) {
        
        } 
      function handleKeyUp(event) {

        }
      
  
      window.onload = initCarInputWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });
      document.addEventListener('keydown', handleKeyDown);
      document.addEventListener('keyup', handleKeyUp); 
           
    </script>
  </body>    
</html>

)HTMLHOMEPAGE";
