<html>
<head>
  <title>ETS Report</title>
  <link rel="stylesheet" href="../../resources/css/style.css">
  <script src="../../resources/js/Chart.bundle.min.js"></script>
  <script src="../../resources/js/jquery-3.3.1.js"></script>
</head>

<body>
  <div id="header">
    <h2>Edk2 Test System Report</h2>
  </div>

  <div class="main-flex-container">
    <div>
      <ul class="breadcrumb">
        <li><a href="../index.html">Home</a></li>
        <li>${sequence.name}</li>
      </ul>
    </div>

    <div id="title">
      <img src="../../resources/img/recurrent.png" alt="Recurrent Suites"></img>
      <h3>Recurrent Suite: </h3>
      <h3>${sequence.name}</h3>
    </div>
  </div>

  <div id="chart">
    <div class='sub-title'>Overview Statistics</div>
    <div style='display:inline-block'>
      <canvas id="pieChart"></canvas>
    </div>
    <div style='display: inline-block; margin-left: 10%;'>
      <canvas id="barChart"></canvas>
    </div>
  </div>
  
  <div class="table-area">
  <div class='sub-title'>Detailed Result ( ${sequence.iterationNumber} recurrences )</div>
  <div class="table-wrapper">
  <table id="script_table">
	<tr>
      <th>Iteration
      <th>Result
    </tr>
  	
  	<#list sequence.iterations?size-1..0 as i>
  	<tr onclick = "rowListener('${i + 1}')">
  		<td>${i + 1}	
		<#if sequence.iterations[i].result == "PASS">
		<td style="color:#7eddb5;">${sequence.iterations[i].result}
		</#if>
		<#if sequence.iterations[i].result == "WARN">
		<td style="color:#ffa105;">${sequence.iterations[i].result}
		</#if>
		<#if sequence.iterations[i].result == "FAIL">
		<td style="color:#b94b3b;">${sequence.iterations[i].result}
		</#if>
  	</tr>
  	</#list>
  </table>
  </div>
  </div>
  
  <script>
  	function rowListener(iteration_order){
  		console.log(iteration_order);
  		var iteration_folder = "Number" + iteration_order;
  		window.location.href = iteration_folder + "/" + "iteration" + iteration_order + ".html";
  	}
  </script>

  <script>
  var ctx = document.getElementById('pieChart').getContext('2d');
  new Chart(ctx,
    {"type":"doughnut",
    "data":{"labels":["Pass","Fail","Warn"],
    "datasets":[
      {
        "data":[${sequence.passNumber},${sequence.failNumber},${sequence.warnNumber}],
        "backgroundColor":["#7eddb5","#d94b3b","#ffa105"]
      }
    ]
  },
  "options": {"maintainAspectRatio": false}
});
</script>

<script>
var ctx = document.getElementById("barChart").getContext("2d");
var data = {
  labels: [
    "Pass",
    "Fail",
    "Warn"
  ],
  datasets: [
    {
      label: "Pass Number",
      data: [${sequence.passNumber}, ${sequence.failNumber}, ${sequence.warnNumber}],
      backgroundColor: ["#7eddb5", "#d94b3b", "#ffa105" ],
    }]
  };

  var barChart = new Chart(ctx, {
    type: 'horizontalBar',
    data: data,
    options: {
      scales: {
        yAxes: [{
          stacked: true
        }]
      },
      title: {
        display: true,
        text: 'Detailed numbers'
      }
    }
  });
  </script>
</body>
</html>
