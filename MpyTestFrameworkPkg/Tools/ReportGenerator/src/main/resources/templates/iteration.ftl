<html>
<head>
  <title>ETS Report</title>
  <link rel="stylesheet" href="../../../resources/css/style.css">
  <script src="../../../resources/js/Chart.bundle.min.js"></script>
  <script src="../../../resources/js/jquery-3.3.1.js"></script>
</head>
<body>

<body>
  <div id="header">
    <h2>Edk2 Test System Report</h2>
  </div>

  <div class="main-flex-container">
    <div>
      <ul class="breadcrumb">
        <li><a href="../../index.html">Home</a></li>
        <li><a href="../${iteration.parentName}.html">${iteration.parentName}</a></li>
        <li>#${iteration.iterationOrder}</li>
      </ul>
    </div>

    <div id="title">
      <img src="../../../resources/img/testcase.png" alt="TestScript"></img>
      <h3>Suite Name: </h3>
      <h3>${iteration.parentName}</h3>
    </div>
  </div>
  
  <div class="side-toolbar">
  <div id="fail-item" clicked="false" class="toolbar-item">
  Fail
  <span class="dot">${iteration.failNumber}</span>
  </div>
  <div id="warn-item" clicked="false" class="toolbar-item">
  Warn
  <span class="dot">${iteration.warnNumber}</span>
  </div>
  <div id="pass-item" clicked="false" class="toolbar-item">
  Pass
  <span class="dot">${iteration.passNumber}</span>
  </div>
  </div> 
  
  <div id="fail-script-content" class="script-content">
	  <#assign fail_flag = "false">
      <#list 0..iteration.scripts?size-1 as i>
          <#if iteration.scripts[i].result == "FAIL">
		  <#assign fail_flag = "true">
		  <div class="script-block">
			  <div class="script-block-fold">
				  <span class="script-name">${iteration.scripts[i].name}</span>
				  <img class="fold-unfold" src="../../../resources/img/unfold.png" alt="fold"></img> 
			  </div> 
			  <div class="script-block-unfold">
				  <#list 0..iteration.scripts[i].log?size-1 as j>
				  <p class="statement">[${iteration.scripts[i].log[j].time_stamp}] [${iteration.scripts[i].log[j].level}]: ${iteration.scripts[i].log[j].message} </p> 
				  </#list>
			  </div>
		  </div>
		  </#if>
	  </#list>
	  <#if fail_flag == "false">
	  	<h2 class="none-prompt">No test case in fail category</h2>
	  </#if>
  </div>
  
  <div id="warn-script-content" class="script-content">
	  <#assign warn_flag = "false">
      <#list 0..iteration.scripts?size-1 as i>
          <#if iteration.scripts[i].result == "WARN">
		  <#assign warn_flag = "true">
		  <div class="script-block">
			  <div class="script-block-fold">
				  <span class="script-name">${iteration.scripts[i].name}</span>
				  <img class="fold-unfold" src="../../../resources/img/unfold.png" alt="fold"></img> 
			  </div> 
			  <div class="script-block-unfold">
				  <#list 0..iteration.scripts[i].log?size-1 as j>
				  <p class="statement">[${iteration.scripts[i].log[j].time_stamp}] [${iteration.scripts[i].log[j].level}]: ${iteration.scripts[i].log[j].message} </p> 
				  </#list>
			  </div>
		  </div>
		  </#if>
	  </#list>
	  <#if warn_flag == "false">
	  	<h2 class="none-prompt">No test case in warn category</h2>
	  </#if>
  </div>
  
  <div id="pass-script-content" class="script-content">
	  <#assign pass_flag = "false">
      <#list 0..iteration.scripts?size-1 as i>
          <#if iteration.scripts[i].result == "PASS">
		  <#assign pass_flag = "true">
		  <div class="script-block">
			  <div class="script-block-fold">
				  <span class="script-name">${iteration.scripts[i].name}</span>
				  <img class="fold-unfold" src="../../../resources/img/unfold.png" alt="fold"></img> 
			  </div> 
			  <div class="script-block-unfold">
				  <#list 0..iteration.scripts[i].log?size-1 as j>
				  <p class="statement">[${iteration.scripts[i].log[j].time_stamp}] [${iteration.scripts[i].log[j].level}]: ${iteration.scripts[i].log[j].message} </p> 
				  </#list>
			  </div>
		  </div>
		  </#if>
	  </#list>
	  <#if pass_flag == "false"> 
	  	<h2 class="none-prompt">No test case in pass category</h2>
	  </#if>
	  <#assign pass_flag = "pass">
  </div>

  <script>
  	$("div.toolbar-item").click(toolbar_item_listener);
  	
  	var script_content_list = ["pass-script-content", "warn-script-content", "fail-script-content"];
  	
  	function toolbar_item_listener(){
  		if($(this).attr("clicked") == "false"){
			var items = $(".toolbar-item");
			for(var i = 0; i < items.length; i++){
				  var item = items.eq(i);
				  if(item.attr("clicked") == "true"){
						item.animate(
						{
							width: '-=30%'
						}
						);
						item.attr("clicked", "false");
				  }
			}
			$(this).animate(
				{
					width: '+=30%'
				}
			);
			$(this).attr("clicked", "true");
			
			var content_id = $(this).attr("id").substring(0,4) + "-script-content";
			console.log(content_id)
			console.log($("#" + content_id));
			console.log($("#" + content_id).attr('class'));
			$("#" + content_id).css("display", "inline-block");
			for(var i = 0; i < script_content_list.length; i++){
				if(script_content_list[i] == content_id){
					$("#" + script_content_list[i]).css("display", "inline-block");
				}else{
					$("#" + script_content_list[i]).css("display", "none");
				}
			}
  		}
  	}	
  	

	$("img.fold-unfold").click(fold_unfold_listener);
	var unfold = false;
  	function fold_unfold_listener(){
  	    if(unfold == false){
  	    	  $(this).parent().parent().find(".script-block-unfold").slideDown();
			  unfold = true;
	 	}else{
	 		  unfold = false;
  	    	  $(this).parent().parent().find(".script-block-unfold").slideUp();
	 	}
  	}

  </script>
  
</body>
</html>