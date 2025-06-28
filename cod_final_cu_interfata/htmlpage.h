const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Stație Meteo</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 10px; }
    #ico { font-size: 48px; margin: 10px; }
    table, th, td { border: 1px solid black; border-collapse: collapse; margin: 0 auto; }
    th, td { padding: 5px 10px; }
    canvas { max-width: 80%; height: 300px; margin-top: 20px; }
    #recomandari { margin-top: 20px; font-weight: bold; }
  </style>
</head>
<body>
  <h1>Stație Meteo</h1>
  <div id="ora">Ora curentă: --:--</div>
  <div id="ico">☁️</div>
  <div id="live">Se încarcă datele...</div>
  <button onclick="inregistreaza()">Înregistrează valori</button>
  <button onclick="stergeTabel()">Șterge tabelul</button>
  <table id="tabel">
    <thead><tr><th>#</th><th>Data/Ora</th><th>Temp</th><th>Umid</th>
    <th>Pres</th><th>Ploaie</th><th>Lumină</th></tr></thead>
    <tbody></tbody>
  </table>
  <canvas id="grafic"></canvas>
  <div id="recomandari">Recomandări: -</div>

  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <script>
    let nr = 0;
    let date = [];
    let chart;

    async function updateData() {
      const res = await fetch('/valori');
      const d = await res.json();
      document.getElementById('ora').innerText = `Ora curentă: ${d.ora}`;
      document.getElementById('live').innerText = 
      `Temp: ${d.temperatura} °C | Umid: ${d.umiditate} % | Pres: ${d.presiune} hPa | ` +
        `Ploaie: ${d.ploaie ? 'Plouă' : 'Nu plouă'} | Lumină: ${d.lumina ? 'Zi' : 'Noapte'}`;
      document.getElementById('ico').innerText = d.ploaie ? '🌧️' :
       (!d.lumina ? '🌙' : (d.temperatura < 20 ? '🌤️' : '☀️'));
      return d;
    }

    function stergeTabel() {
      nr = 0;
      date = [];
      document.querySelector('#tabel tbody').innerHTML = '';
      document.getElementById('recomandari').innerText = 'Recomandări: -';
      chart.data.labels = [];
      chart.data.datasets.forEach(ds => ds.data = []);
      chart.update();
    }

    async function inregistreaza() {
      const d = await updateData();
      if (nr >= 7) return;
      nr++;
      date.push(d);
      const now = new Date();
      const timestamp = now.toLocaleDateString() + ' ' + now.toLocaleTimeString().slice(0, 5);
      const row = `<tr><td>${nr}</td><td>${timestamp}</td><td>${d.temperatura}</td><td>${d.umiditate}
      </td><td>${d.presiune}</td><td>${d.ploaie ? 'Da' : 'Nu'}
      </td><td>${d.lumina ? 'Zi' : 'Noapte'}</td></tr>`;
      document.querySelector('#tabel tbody').innerHTML += row;
      chart.data.labels.push("L" + nr);
      chart.data.datasets[0].data.push(d.temperatura);
      chart.data.datasets[1].data.push(d.umiditate);
      chart.update();
      analiza();
    }

    function analiza() {
      if (date.length == 0) return;
      const d = date[date.length - 1];
      let text = '';
      if (d.umiditate < 30 && d.temperatura > 25) text = 'Arșiță, se recomandă udare artificială';
      else if (d.umiditate > 85) text = 'Umiditate ridicată, nu se recomandă udare';
      else text = 'Valori normale';
      document.getElementById('recomandari').innerText = 'Recomandări: ' + text;
    }

    window.onload = () => {
      const ctx = document.getElementById('grafic').getContext('2d');
      chart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: [],
          datasets: [
            { label: 'Temp (°C)', data: [], borderWidth: 2 },
            { label: 'Umid (%)', data: [], borderWidth: 2 }
          ]
        },
        options: {
          responsive: true,
          scales: {
            y: {
              beginAtZero: true,
              ticks: {
                stepSize: 1
              }
            }
          }
        }
      });
      updateData();
      setInterval(updateData, 3000);
    }
  </script>
</body>
</html>
)rawliteral";
