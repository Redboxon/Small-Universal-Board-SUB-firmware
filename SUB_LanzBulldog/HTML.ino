// This file contains the HTML data for the ESP32.

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=.8, maximum-scale=1, user-scalable=no">
  <style>
body {
  margin: 0;
  padding: 0;
  background-color: black;
  overflow: hidden;
  font-family: sans-serif;
}

h1 {
  margin: 10px 0 0 0;
  font-size: 1.8em;
  text-align: center;
  color: white;
}

.noselect {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  user-select: none;
}

/*Throttle slider @ top center*/
.top-center {
  display: flex;
  justify-content: center;
  padding: 10px 10px 30px 10px;
}

.slidecontainer {
  width: 90%;
}

.slider {
  -webkit-appearance: none;
  appearance: none;
  width: 100%;
  height: 20px;
  border-radius: 5px;
  background: #d2d2d2;
  outline: none;
  opacity: 0.7;
  transition: opacity 0.2s;
}

.slider:hover {
  opacity: 1;
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 85px;
  height: 85px;
  border-radius: 50%;
  background: brown;
  border: none;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: 60px;
  height: 40px;
  border-radius: 50%;
  background: white;
  cursor: pointer;
}

.bottom-controls {
  position: fixed;
  bottom: 120px;
  left: 60px;
}
.top-controls {
  position: fixed;
  bottom: 120px;
  right: -90px;
}

.aux-steering-group {
  display: flex;
  flex-direction: row;
  align-items: center;
  gap: 0px; /* Gap between steering buttons and slider*/
}

.aux-buttons {
  display: flex;
  flex-direction: column;
  gap: 85px; /* Gap between adjacent buttons*/
  align-items: flex-start;
}

.auxButton {
  background-color: white;
  box-shadow: 5px 5px #888888;
  color: grey;
  padding: 30px 35px;
  border: none;
  border-radius: 20%;
  font-size: 24px;
  cursor: pointer;
  transform: rotate(90deg);
  transform-origin: left center;
  width: 150px;
}

.auxButton:active {
  transform: translate(-5px, 5px) rotate(90deg);
  transform-origin: left center;
  box-shadow: none;
}


.vertical-slider-container {
  height: 50px;
  display: none;
  align-items: center;
  justify-content: center;
  transform:  translateX(-100px) translateY(-50px);
}

.vertical-slider {
  writing-mode: bt-lr;
  width: 250px;
  height: 20px;
  -webkit-appearance: none;
  appearance: none;
  background: #d2d2d2;
  outline: none;
  opacity: 0.7;
  transition: opacity 0.2s;
  transform: rotate(270deg);
}

.vertical-slider:hover {
  opacity: 1;
}

.vertical-slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  border-radius: 25%;
  width: 65px;
  height: 65px;
  background-color: brown;
  border: none;
  cursor: pointer;
}

.vertical-slider::-moz-range-thumb {
  width: 60px;
  height: 40px;
  border-radius: 50%;
  background: white;
  cursor: pointer;
}


    </style>
  
  </head>
<body class="noselect">
  <h1>Lanz Bulldog</h1> <!-- customize Name here too -->
	<br/>
	<br/>
  <div class="top-center">
    <div class="slidecontainer">
      <input type="range" min="-255" max="255" value="0" class="slider" id="throttle"
        oninput='sendButtonInput("throttle", value)' ontouchend='resetThrottleSlider()'>
    </div>
  </div>

  <div class="bottom-controls">
    <div class="aux-steering-group">
      <div class="aux-buttons">
        <button class="auxButton"
          ontouchstart='sendButtonInput("steering", "105")'
          onmousedown='sendButtonInput("steering", "105")'
          ontouchend='sendButtonInput("steering", "86")'
          onmouseup='sendButtonInput("steering", "86")'>&#8678;LEFT</button>

        <button class="auxButton"
          ontouchstart='sendButtonInput("steering", "67")'
          onmousedown='sendButtonInput("steering", "67")'
          ontouchend='sendButtonInput("steering", "86")'
          onmouseup='sendButtonInput("steering", "86")'>RIGHT&#8680;</button>        
      </div>

      <div class="vertical-slider-container">
        <input type="range" min="67" max="105" value="86" class="vertical-slider" id="steering"
          oninput='sendButtonInput("steering", value)' ontouchend='resetSteeringSlider()' >
      </div>
    </div>
  </div>
  <div class="top-controls">
	<div class="aux-buttons">
		<button class="auxButton" id="toggleControlMode" onclick="toggleControlMode()">Toggle UI</button>
		<button class="auxButton"
          ontouchstart='sendButtonInput("coupling", "")'
          onmousedown='sendButtonInput("coupling", "")'>COUPLE</button>
	</div>
  </div>
</body>


  
    <script>
		let useSlider = false;

	  function toggleControlMode() {
		useSlider = !useSlider;

		const slider = document.querySelector('.vertical-slider-container');
		const buttons = document.querySelector('.aux-buttons');
		const allButtons = buttons.querySelectorAll('button');
		const steeringSlider = document.getElementById('steering');

		if (useSlider) {
		  // show slider hide buttons
		  slider.style.display = 'flex';
		  buttons.style.display = 'none';

		  // Buttons inactive
		  allButtons.forEach(btn => btn.disabled = true);

		  // Slider active
		  steeringSlider.disabled = false;

		  document.getElementById('toggleControlMode').innerText = 'Buttons';
		} else {
		  // hide slider, show buttons
		  slider.style.display = 'none';
		  buttons.style.display = 'flex';

		  // Buttons active
		  allButtons.forEach(btn => btn.disabled = false);

		  // Slider inactive
		  steeringSlider.disabled = true;

		  document.getElementById('toggleControlMode').innerText = 'Slider';
		}
	  }

	  // Initially: Slider hidden
	  window.onload = function() {
		document.querySelector('.vertical-slider-container').style.display = 'none';
		document.getElementById('steering').disabled = true;
	  };
	  
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      
      var websocketCarInput;
      const throttleSlider = document.getElementById('throttle');
      const steeringSlider = document.getElementById('steering');

      function resetThrottleSlider() 
      {
       sendButtonInput("throttle", 0);
       throttle.value = 0;
       sendButtonInput("throttle", 0);
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
