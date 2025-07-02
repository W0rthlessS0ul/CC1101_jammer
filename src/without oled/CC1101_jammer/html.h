const char* html_sampling_settings = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Value = parseInt(document.getElementById('Value').value);
			console.log(Value)
            if (isNaN(Value) || Value < 10 || Value > 500) {
                alert('Value must be a number between 10 and 500');
                return;
            }
            location.href = `/sampleinterval?Value=${Value}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Sampling Interval</h1>
        <input id="Value" class="input" type="number" placeholder="Interval size (10-500)" max="500" min="10" />
        <button class="button" onclick="validateAndRedirect()">Set</button>
    </div>
</body>
</html>
)rawliteral";
const char* html_rec_play = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>RAW Recorder/Player</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        h1 {
            color: #007bff;
            margin-top: 0;
            margin-bottom: 20px;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 14px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
            height: 48px;
            box-sizing: border-box;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .progress-bar {
            width: 100%;
            height: 20px;
            background-color: #333;
            border-radius: 10px;
            margin: 20px 0;
            overflow: hidden;
        }

        .progress {
            height: 100%;
            background-color: #007bff;
            width: 0%;
        }

        .status {
            margin: 10px 0;
            min-height: 24px;
        }

        .buttons {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            width: 100%;
        }

        button {
            flex: 1;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        button:active {
            transform: translateY(1px);
        }

        button:disabled {
            background-color: #555;
            cursor: not-allowed;
            transform: none;
            box-shadow: none;
        }

        .stop-btn {
            background-color: #dc3545;
            width: 100%;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>RAW Recorder/Player</h1>
        
        <div class="progress-bar">
            <div id="progress" class="progress"></div>
        </div>
        <div id="status" class="status">Ready</div>
        
        <input id="frequencyInput" class="input" type="number" 
               placeholder="Frequency (300.00-928.00) MHz" 
               step="0.01" min="300" max="928" value="433.92">
        
        <div class="buttons">
            <button id="recordBtn" onclick="startRecording()">Record</button>
            <button id="playBtn" onclick="startPlayback()">Play</button>
        </div>
        
        <button id="stopBtn" class="stop-btn" onclick="stopOperation()" disabled>Stop</button>
    </div>

    <script>
        let activeOperation = false;
        let updateInterval = 100;
        
        function setActive(active) {
            activeOperation = active;
            document.getElementById('recordBtn').disabled = active;
            document.getElementById('playBtn').disabled = active;
            document.getElementById('stopBtn').disabled = !active;
        }
        
        function updateProgress() {
            if (!activeOperation) return;
            
            fetch('/rec_play_status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('progress').style.width = data.progress + '%';
                    
                    let status = 'Ready';
                    if (data.operation === 'recording') status = 'Recording: ' + data.progress + '%';
                    if (data.operation === 'replaying') status = 'Playing: ' + data.progress + '%';
                    document.getElementById('status').textContent = status;
                    
                    if (data.operation !== 'none') {
                        setTimeout(updateProgress, updateInterval);
                    } else {
                        setActive(false);
                    }
                });
        }
        
        function startRecording() {
            const frequency = document.getElementById('frequencyInput').value;
            if (frequency < 300 || frequency > 928) {
                alert('Frequency must be between 300 and 928 MHz');
                return;
            }
            
            setActive(true);
            document.getElementById('status').textContent = 'Starting...';
            
            fetch('/record?frequency=' + frequency)
                .then(response => {
                    if (response.ok) updateProgress();
                    else throw new Error();
                })
                .catch(() => {
                    document.getElementById('status').textContent = 'Error starting';
                    setActive(false);
                });
        }
        
        function startPlayback() {
            const frequency = document.getElementById('frequencyInput').value;
            if (frequency < 300 || frequency > 928) {
                alert('Frequency must be between 300 and 928 MHz');
                return;
            }
            
            setActive(true);
            document.getElementById('status').textContent = 'Starting...';
            
            fetch('/play?frequency=' + frequency)
                .then(response => {
                    if (response.ok) updateProgress();
                    else throw new Error();
                })
                .catch(() => {
                    document.getElementById('status').textContent = 'Error starting';
                    setActive(false);
                });
        }
        
        function stopOperation() {
          setActive(true);
          document.getElementById('status').textContent = 'Stopping...';
  
          fetch('/stop_operation', { method: 'POST' })
            .then(response => {
              if (!response.ok) throw new Error();
            })
            .catch(() => {
              document.getElementById('status').textContent = 'Error stopping';
              setActive(false);
            });
        }
    </script>
</body>
</html>
)rawliteral";
const char* html_hopper_jammer = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 400px;
        }

        h1 {
            text-align: center;
            margin-bottom: 20px;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 14px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 12px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
            height: 48px;
            box-sizing: border-box;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }

        .add-button {
            background-color: #28a745;
            width: 50%;
            padding: 12px;
            font-size: 14px;
            margin: 0 0 15px 0;
            align-self: flex-start;
        }

        .add-button:hover {
            background-color: #218838;
        }

        .delete-button {
            background-color: #dc3545;
            width: 48px;
            height: 48px;
            padding: 0;
            margin-left: 12px;
            font-size: 20px;
            border-radius: 8px;
            flex-shrink: 0;
        }

        .delete-button:hover {
            background-color: #c82333;
        }

        .input-group {
            display: flex;
            align-items: center;
            width: 100%;
            margin-bottom: 12px;
        }

        .input-wrapper {
            flex-grow: 1;
        }

        .jam-button {
            padding: 12px;
            width: 100%;
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Set Frequency Values</h1>
        <div id="inputs-container">
            <div class="input-group">
                <div class="input-wrapper">
                    <input class="input" type="number" placeholder="Frequency (300.00-928.00) MHz" max="928.00" min="300.00" step="0.01" />
                </div>
                <button class="button delete-button" onclick="removeInputField(this)" disabled>×</button>
            </div>
        </div>
        <button class="button add-button" onclick="addInputField()">Add Frequency</button>
        <button class="button jam-button" onclick="validateAndRedirect()">Jam</button>
    </div>

    <script>
        function addInputField() {
            const container = document.getElementById('inputs-container');
            const inputGroup = document.createElement('div');
            inputGroup.className = 'input-group';
            
            const inputWrapper = document.createElement('div');
            inputWrapper.className = 'input-wrapper';
            
            const newInput = document.createElement('input');
            newInput.className = 'input';
            newInput.type = 'number';
            newInput.placeholder = 'Frequency (300.00-928.00) MHz';
            newInput.max = '928.00';
            newInput.min = '300.00';
            newInput.step = '0.01';
            
            const deleteButton = document.createElement('button');
            deleteButton.className = 'button delete-button';
            deleteButton.innerHTML = '×';
            deleteButton.onclick = function() { removeInputField(this); };
            
            inputWrapper.appendChild(newInput);
            inputGroup.appendChild(inputWrapper);
            inputGroup.appendChild(deleteButton);
            container.appendChild(inputGroup);

            const allDeleteButtons = document.querySelectorAll('.delete-button');
            allDeleteButtons.forEach(btn => btn.disabled = false);
        }

        function removeInputField(button) {
            const inputGroup = button.parentElement;
            inputGroup.remove();

            const allDeleteButtons = document.querySelectorAll('.delete-button');
            if (allDeleteButtons.length === 1) {
                allDeleteButtons[0].disabled = true;
            }
        }

        function validateAndRedirect() {
            const inputs = document.querySelectorAll('.input');
            let values = [];
            let isValid = true;

            inputs.forEach(input => {
                const value = parseFloat(input.value);
                if (isNaN(value) || value < 300.00 || value > 928.00) {
                    isValid = false;
                    input.style.boxShadow = '0 0 8px rgba(255, 0, 0, 0.5)';
                } else {
                    input.style.boxShadow = '';
                    values.push(value.toFixed(2));
                }
            });

            if (!isValid || values.length === 0) {
                alert('All values must be numbers between 300 and 928 MHz');
                return;
            }

            location.href = `/hopper_jam?Value=${values.join('|')}`;
        }
    </script>
</body>
</html>
)rawliteral";
const char* html_payload_settings = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Value = parseInt(document.getElementById('Value').value);
			console.log(Value)
            if (isNaN(Value) || Value < 1 || Value > 254) {
                alert('Value must be a number between 1 and 254');
                return;
            }
            location.href = `/payload?Value=${Value}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Payload Size</h1>
        <input id="Value" class="input" type="number" placeholder="Payload size (1-254)" max="254" min="1" />
        <button class="button" onclick="validateAndRedirect()">Set</button>
    </div>
</body>
</html>
)rawliteral";
const char* html_range_jammer = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Start = parseFloat(document.getElementById('Start').value);
			      var Stop = parseFloat(document.getElementById('Stop').value);
            if (isNaN(Start) || Start < 300.00 || Start > 928.00 || isNaN(Stop) || Stop < 300.00 || Stop > 928.00) {
                alert('Values must be a number between 300 and 928 MHz');
                return;
            }
			if (Stop <= Start) {
                alert('The stop value must be greater than the Start value. ');
                return;
            }
            location.href = `/range_jam?Start=${Start}&Stop=${Stop}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Frequency Values</h1>
        <input id="Start" class="input" type="number" placeholder="Start Frequency (300.00-928.00) MHz" max="928.00" min="300.00" />
		<input id="Stop" class="input" type="number" placeholder="Stop Frequency (300.00-928.00) MHz" max="928.00" min="300.00" />
        <button class="button" onclick="validateAndRedirect()">Jam</button>
    </div>
</body>
</html>f
)rawliteral";
const char* html_step_setting = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Value = parseFloat(document.getElementById('Input').value);
			
            if (isNaN(Value) || Value < 0.01 || Value > 254) {
                alert('Value must be a number between 0.01 and 254');
                return;
            }
            location.href = `/stepinterval?Value=${Value}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Step Interval Value</h1>
        <input id="Input" class="input" type="number" placeholder="Step (0.01-254)" max="254" min="0.01" />
        <button class="button" onclick="validateAndRedirect()">Set</button>
    </div>
</body>
</html>
)rawliteral";
const char* html_jamming_time_setting = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }

        .hint {
            font-size: 12px;
            color: #888;
            margin-top: -10px;
            margin-bottom: 15px;
            width: 100%;
            text-align: center;
        }

        .infinity {
            color: #ff5555;
            font-weight: bold;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Value = parseInt(document.getElementById('Input').value);
            
            if (isNaN(Value) || Value < 0 || Value > 254) {
                alert('Value must be a number between 0 and 254 seconds');
                return;
            }
            location.href = `/editdelay?Value=${Value}`;
        }

        function updateHint() {
            var value = parseInt(document.getElementById('Input').value);
            var hint = document.getElementById('hint');
            
            if (value === 0) {
                hint.innerHTML = 'Value <span class="infinity">0</span> means jamming will run <span class="infinity">∞ (infinitely)</span> until manually stopped';
            } else {
                hint.innerHTML = '';
            }
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Delay Value</h1>
        <input id="Input" class="input" type="number" placeholder="Delay (0-254) seconds" 
               max="254" min="0" oninput="updateHint()" />
        <div id="hint" class="hint"></div>
        <button class="button" onclick="validateAndRedirect()">Set</button>
    </div>
    <script>
        document.getElementById('Input').addEventListener('input', updateHint);
    </script>
</body>
</html>
)rawliteral";
const char* html_pls_reboot = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Please reboot</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
        }, 1000);
    </script>
</body>
</html>
)rawliteral";
const char* html = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
            opacity: 0;
            animation: fadeIn 1s forwards;
        }

        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
            transform: translateY(20px);
            animation: slideIn 0.5s forwards;
        }

        @keyframes slideIn {
            from { transform: translateY(20px); opacity: 0; }
            to { transform: translateY(0); opacity: 1; }
        }

        .header {
            margin-bottom: 20px;
            animation: logoMovement 1s infinite alternate;
        }

        @keyframes logoMovement {
            0% { transform: translateY(0); }
            100% { transform: translateY(-5px); }
        }

        .header img {
            width: 100px;
            height: auto;
            margin-bottom: 10px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px;
            margin-bottom: 15px;
        }

        .button, .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover, .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px) scale(1.05);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active, .dropdown-button:active {
            transform: translateY(2px);
        }

        .button:focus, .dropdown-button:focus {
            outline: none;
        }

        .jammers-button {
            background-color: #007bff;
        }

        .repeaters-button {
            background-color: #007bff;
        }

        .settings-button {
            background-color: #28a745;
        }

        .dropdown {
            position: relative;
            background-color: rgba(30, 30, 30, 0.9);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
            margin-top: 5px;
        }

        .dropdown.show {
            display: flex;
            animation: dropdownExpand 0.6s forwards;
        }

        .nested-dropdown {
            position: relative;
            margin-top: 5px;
        }

        @keyframes dropdownExpand {
            0% { opacity: 0; transform: scaleY(0); }
        }

        .dropdown-button {
            transition: opacity 0.3s ease, transform 0.3s ease;
            opacity: 1;
            transform: translateY(0);
        }
        
        .dropdown-button.hidden {
            opacity: 0;
            transform: translateY(-20px);
            pointer-events: none;
        }
    </style>
    <script>
        function toggleDropdown(dropdownId, event) {
            event.stopPropagation();
            const dropdown = document.getElementById(dropdownId);
            const isNested = dropdown.classList.contains('nested-dropdown');
            const wasVisible = dropdown.classList.contains('show');
            
            if (!isNested) {
                document.querySelectorAll('.dropdown:not(.nested-dropdown)').forEach(d => {
                    if (d.id !== dropdownId) {
                        d.classList.remove('show');
                        d.querySelectorAll('.dropdown-button').forEach(btn => {
                            btn.classList.add('hidden');
                        });
                    }
                });
            }
            
            if (wasVisible) {
                dropdown.classList.remove('show');
                dropdown.querySelectorAll('.dropdown-button').forEach(btn => {
                    btn.classList.add('hidden');
                });
            } else {
                dropdown.classList.add('show');
                const items = dropdown.querySelectorAll('.dropdown-button');
                items.forEach((item, index) => {
                    setTimeout(() => {
                        item.classList.remove('hidden');
                    }, index * 50);
                });
            }
        }

        window.addEventListener('click', function() {
            document.querySelectorAll('.dropdown:not(.nested-dropdown)').forEach(dropdown => {
                dropdown.classList.remove('show');
                dropdown.querySelectorAll('.dropdown-button').forEach(item => {
                    item.classList.add('hidden');
                });
            });
        });

        document.querySelectorAll('.dropdown').forEach(dropdown => {
            dropdown.addEventListener('click', function(e) {
                e.stopPropagation();
            });
        });

        document.addEventListener('DOMContentLoaded', function() {
            document.querySelectorAll('.dropdown-button').forEach(btn => {
                btn.classList.add('hidden');
            });
        });
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
		    <img src="data:image/png;base64,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" alt="Logo"></img>
        </div>
        <div class="buttons">
            <button class="button repeaters-button" onclick="toggleDropdown('repeatersDropdown', event)">Repeaters</button>
            <div id="repeatersDropdown" class="dropdown">
                <button onclick="location.href='/rec_play'" class="dropdown-button hidden">RAW Repeater</button>
                <button class="dropdown-button settings-button hidden" onclick="toggleDropdown('settingsDropdownRepeaters', event)">Settings</button>
                <div id="settingsDropdownRepeaters" class="dropdown nested-dropdown">
                    <button onclick="location.href='/setting_sampling'" class="dropdown-button hidden">Sampling Interval</button>
                </div>
            </div>
        
            <button class="button jammers-button" onclick="toggleDropdown('jammersDropdown', event)">Jammers</button>
            <div id="jammersDropdown" class="dropdown">
                <button onclick="location.href='/keyfob_jammer'" class="dropdown-button hidden">KeyFob jammer</button>
                <button onclick="location.href='/spot_jammer'" class="dropdown-button hidden">Spot jammer</button>
                <button onclick="location.href='/range_jammer'" class="dropdown-button hidden">Range jammer</button>
                <button onclick="location.href='/hopper_jammer'" class="dropdown-button hidden">Hopper jammer</button>
                <button class="dropdown-button settings-button hidden" onclick="toggleDropdown('settingsDropdownJammers', event)">Settings</button>
                <div id="settingsDropdownJammers" class="dropdown nested-dropdown">
                    <button onclick="location.href='/setting_jamming_time'" class="dropdown-button hidden">Jamming Time</button>
                    <button onclick="location.href='/setting_range_step'" class="dropdown-button hidden">Range Step</button>
                    <button onclick="location.href='/setting_payload_size'" class="dropdown-button hidden">Payload Size</button>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";
const char* html_spot_jammer = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            var Value = parseFloat(document.getElementById('Input').value);
			
            if (isNaN(Value) || Value < 300.00 || Value > 928.00) {
                alert('Value must be a number between 300 and 928 MHz');
                return;
            }
            location.href = `/spot_jam?Value=${Value}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Frequency Value</h1>
        <input id="Input" class="input" type="number" placeholder="Frequency (300.00-928.00) MHz" max="928.00" min="300.00" />
        <button class="button" onclick="validateAndRedirect()">Jam</button>
    </div>
</body>
</html>
)rawliteral";
const char* html_frequency_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script>
        const urlParams = new URLSearchParams(window.location.search);
        const jamDelay = urlParams.get('delay') || 0;

        if (jamDelay > 0) {
            setTimeout(function() {
                window.location.href = '/';
            }, jamDelay * 1000);
        }

        setInterval(function() {
            fetch('/jamming_status')
                .then(response => response.text())
                .then(status => {
                    if (status === 'stopped') {
                        window.location.href = '/';
                    }
                });
        }, 2000);
    </script>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
			margin-bottom: 20px; 
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }
		
        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Jamming selected frequency</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>

        <button onclick="location.href='/stop_jamming'" class="button button">Stop Jamming</button>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
        }, 1000);
    </script>
</body>
</html>
)rawliteral";
const char* html_keyfob_jammer = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
		function Redirect(frequency) {
            location.href = `/keyfob_jam?frequency=${frequency}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Set KeyFob Frequency</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="Redirect(303.00)">303 MHz</button>
			<button class="button" onclick="Redirect(310.00)">310 MHz</button>
			<button class="button" onclick="Redirect(315.00)">315 MHz</button>
			<button class="button" onclick="Redirect(330.00)">330 MHz</button>
			<button class="button" onclick="Redirect(350.00)">350 MHz</button>
			<button class="button" onclick="Redirect(370.00)">370 MHz</button>
			<button class="button" onclick="Redirect(390.00)">390 MHz</button>
			<button class="button" onclick="Redirect(418.00)">418 MHz</button>
			<button class="button" onclick="Redirect(433.92)">433.92 MHz</button>
        </div>
    </div>
</body>
</html>
)rawliteral";