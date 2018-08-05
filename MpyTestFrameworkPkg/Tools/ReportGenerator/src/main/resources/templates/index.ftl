<html>
<head>
  <title>ETS Report</title>
  <link rel="stylesheet" href="../resources/css/style.css">
  <script src="../resources/js/Chart.bundle.min.js"></script>
  <script src="../resources/js/jquery-3.3.1.js"></script>
</head>

<body>
  <div id="header">
    <h2>Edk2 Test System Report</h2>
  </div>

  <div class="main-flex-container">
    <div>
      <ul class="breadcrumb">
        <li>Home</li>
      </ul>
    </div>

    <div id="title">
      <img src="../resources/img/default.png" alt="Default Sequences"></img>
      <h3>Summary</h3>
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
  <div class='sub-title'>Detailed Result</div>
  <div class="table-wrapper">
  <table id="sequence_table">
	<tr>
      <th>Order
      <th>Suite Name
      <th>Started
      <th>Result
      <th>Recurrence
    </tr>
	<#list 0..default.testSuites?size-1 as i>
  	<tr onclick="rowListener('${default.testSuites[i].name}', '${default.testSuites[i].startedTime}')">
  		<td>${i + 1}	
  		<td>${default.testSuites[i].name}
		<td>${default.testSuites[i].startedTime}
		<#if default.testSuites[i].result == "PASS">
		<td style="color:#7eddb5;">${default.testSuites[i].result}
		</#if>
		<#if default.testSuites[i].result == "WARN">
		<td style="color:#ffa105;">${default.testSuites[i].result}
		</#if>
		<#if default.testSuites[i].result == "FAIL">
		<td style="color:#b94b3b;">${default.testSuites[i].result}
		</#if>
		<td>${default.testSuites[i].iterationNumber}
  	</tr>
  	</#list>
  </table>
  </div>
  </div>

  <script>
  	function rowListener(name, startedTime){
  		console.log(name);
  		console.log(startedTime);
  		var sequence_folder = "SEQUENCE" + "__" + name + "__" + startedTime;
  		window.location.href = sequence_folder + "/" + name + ".html";
  	}
  </script>

  <script>
  var ctx = document.getElementById('pieChart').getContext('2d');
  new Chart(ctx,
    {"type":"doughnut",
    "data":{"labels":["Pass","Fail","Warn"],
    "datasets":[
      {
        "data":[${default.passNumber},${default.failNumber},${default.warnNumber}],
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
      data: [${default.passNumber}, ${default.failNumber}, ${default.warnNumber}],
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
