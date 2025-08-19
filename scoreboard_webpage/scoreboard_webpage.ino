String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'>"; 
html += "<title>RoboRail Scoreboard</title>"; 
html += "<meta name='viewport' content='width=device-width,initial-scale=1'>"; 
html += "<link 
href='https://fonts.googleapis.com/css2?family=Poppins:wght@600&display=swap' 
rel='stylesheet'>"; 
html += "<style>"; 
html += "body{" 
          "font-family:'Poppins',Arial,sans-serif;" 
          "text-align:center;" 
          "min-height:100vh;" 
          "background:linear-gradient(135deg,#241a5a 0%,#673ab7 40%,#0dcafc 100%);" 
          "color:#fff;" 
          "display:flex;" 
          "flex-direction:column;" 
          "justify-content:flex-start;" 
          "align-items:center;" 
          "padding-top:8vh;" 
          "}"; 
html += "h1{" 
          "font-size:2.3rem;" 
          "margin-bottom:2.5rem;" 
          "color:#fa6afc;" 
          "text-shadow:0 0 25px #c053ff,0 0 60px #7ab8f7;" // glowing 
          "letter-spacing:2px;" 
          "}"; 
html += ".score{" 
          "font-size:2.1rem;" 
          "font-weight:700;" 
          "margin:22px auto;" 
          "padding:35px 0;" 
          "width:87vw;max-width:340px;" 
          "border-radius:22px;" 
          "background:linear-gradient(90deg,#30005e,#7c36ff,#00f0fc,#fc00c8);" 
          "box-shadow:0 4px 30px 2px #2b0e86cc;" 
          "color:#fff;" 
          "transition:transform 0.3s,color 0.4s;" 
          "text-shadow:0 2px 22px #25fff6cc;" 
          "}"; 
html += ".score:hover{" 
          "transform:scale(1.06);" 
          "color:#ffe066;" 
          "background:linear-gradient(270deg,#0050d0,#00ffe1,#ff46da);" 
          "}"; 
html += "@media(max-width:500px){" 
          ".score{font-size:1.25rem;padding:24px 0;max-width:95vw;}" 
          "h1{font-size:1.36rem;}" 
          "body{padding-top:6vh;}" 
          "}"; 
html += "</style></head><body>"; 
html += "<h1>       
Robo Rail Football Game       
</h1>"; 
html += "<div class='score'>Player 1: " + score1 + "</div>"; 
html += "<div class='score'>Player 2: " + score2 + "</div>"; 
html += "</body></html>";
