const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Rope Skipping</title>
    <style>
        .button {
            background-color: #4CAF50;
            border-radius: 8px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 22px;
            border: none;
            outline: none;
            font-family: fantasy
        }
        .button:hover {
            background-color: #FFFFCC;
            color: black;
            border: 3px solid #4CAF50;
        }
    </style>

</head>
<body onload="initSocket()">
<div style="text-align: center; height: 100%;width: 100%;background: #FFFFCC;position: absolute;">
    <p id="cnt" style="font-size: 180px;padding-top: 18%;margin:0 0 5% 0;font-family: fantasy">
        0
    </p>
    <button class="button" onclick="resetCnt()">RESET</button>
</div>
<div></div>
</body>
<script>
    let webSocket;
    let data = 0;

    function initSocket() {
        webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
        webSocket.onmessage = function(event) {
            console.log(event.data);
            data++;
            const skipCntP = document.getElementById("cnt");
            skipCntP.innerText = data.toString();
        }
    }
    function resetCnt(){
        const skipCntP = document.getElementById("cnt");
        skipCntP.innerText = "0";
        data = 0;
        console.log(data);
    }
</script>
</html>
)=====";