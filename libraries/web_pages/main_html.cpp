
namespace {
constexpr char main_page[] = 
R"~(<!DOCTYPE html>
<html>
<head>
<title>Ultrasonic wind sensor</title>
<script src="compass.js" type="text/javascript"></script>
</head>
<body>
<div id="windangle">0.0</div>
<div id="windspeed">0.0</div>
<canvas id="theCanvas" width="600" height="600" style="border:1px solid #000000;">
Your browser does not support the HTML canvas tag.
</canvas>
<p>Received data = <span id="received_data">---</span> </p>
</body>
<script>
var Socket;
function onSocketRxEvent(event) 
{
   const wind_values = JSON.parse(event.data);
   draw_wind(wind_values.windangle,wind_values.windspeed);
   document.getElementById("received_data").innerHTML = event.data;
   document.getElementById("windangle").innerHTML = wind_values.windangle;
   document.getElementById("windspeed").innerHTML = wind_values.windspeed;
}

window.onload = function(e)
{ 
   Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
   Socket.onmessage = onSocketRxEvent;
   draw_wind(0,0);
}
</script>
</html>
)~";

} // ~namespace

const char* get_web_page_main_html()
{
   return main_page;
}
