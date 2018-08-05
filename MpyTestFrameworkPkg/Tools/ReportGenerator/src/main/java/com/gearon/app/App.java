package com.gearon.app;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.gearon.app.configuration.ConfigurationHolder;
import com.gearon.app.data_model.*;

import freemarker.template.*;

public class App {
	public static void main(String[] args) {
		try {
			File root = new File("").getAbsoluteFile();
			
			String reportFolder = root.getParent() + "/" + "Report";
			//String reportFolder = root + "/" + "Report";
			System.out.println(reportFolder);
			List<String> existedReports = new LinkedList<String>(Arrays.asList(new File(reportFolder).list()));

			String logFolder = root.getParent() + "/" + "Log";
			//String logFolder = root + "/" + "Log";
			System.out.println(logFolder);
			List<String> existedLogs = new LinkedList<String>(Arrays.asList(new File(logFolder).list()));
			if(existedLogs.isEmpty()){
				System.out.println("No logs to be parsed...");
				return;
			}

			List<String> toRemove = new ArrayList<String>();

			for(String log: existedLogs){
				if(existedReports.contains(log) && existedLogs.contains(log)){
					toRemove.add(log);
				}
			}
			
			existedLogs.removeAll(toRemove);
			
			App app = new App();

			for(String log: existedLogs){
				if(log.startsWith("TEST_SUITES")){
					app.generateTestSuitesReport(logFolder, log);
				}else if(log.startsWith("TEST_SUITE")){
					app.generateSequenceReport(logFolder, log);
				}else if(log.startsWith("TEST_CASE")){
					app.generateScriptReport(logFolder, log);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Something bad happens at main method...");
		}
	}
	
	public void generateTestSuitesReport(String logFolder, String log) throws Exception{
		Configuration cfg = ConfigurationHolder.getInstance().getConfiguration();
		TestSuites testSuitesDM = getTestSuitesData(logFolder, log);
		Template template = cfg.getTemplate("index.ftl");
		Map<String, TestSuites> dm = new HashMap<String, TestSuites>();
		dm.put("default", testSuitesDM);
		String reportFolder = logFolder.substring(0, logFolder.length() - 3) + "Report";
		File concreteFolder = new File(reportFolder + "/" + log);
		if(!concreteFolder.exists()){
			concreteFolder.mkdir();
		}
		OutputStream os = new FileOutputStream(new File(concreteFolder.getAbsolutePath() + "/" + "index.html"));
		Writer out = new OutputStreamWriter(os);
		template.process(dm, out);
		
		for(TestSuite sequence: testSuitesDM.getTestSuites()){
			File sequenceFolder = new File(concreteFolder.getAbsolutePath() + "/" + "SEQUENCE__" + sequence.getName()+ "__" + sequence.getStartedTime());
			if(!sequenceFolder.exists()){
				sequenceFolder.mkdir();
			}
			Template rsTemplate = cfg.getTemplate("recurrentSequence.ftl"); 
			Map<String, TestSuite> rsDM = new HashMap<String, TestSuite>();
			rsDM.put("sequence", sequence);
			rsTemplate.process(rsDM, new OutputStreamWriter(new FileOutputStream(new File(sequenceFolder.getAbsolutePath() + "/" + sequence.getName() + ".html"))));
			rsDM.clear();
			int count = 1;
			for(Iteration iteration: sequence.getIterations()){
				File iterationFolder = new File(sequenceFolder.getAbsolutePath() + "/" + "Number" + count );
				if(!iterationFolder.exists()){
					iterationFolder.mkdirs();
				}

				Template iterationTemplate = cfg.getTemplate("iteration.ftl");
				Map<String, Iteration>	iterationDM = new HashMap<String, Iteration>();
				iteration.setIterationOrder(count);
				iteration.setParentName(sequence.getName());
				iterationDM.put("iteration", iteration);
				iterationTemplate.process(iterationDM, new OutputStreamWriter(new FileOutputStream(new File(iterationFolder.getAbsolutePath() + "/" + "iteration" + count + ".html"))));
				count++;
			}
		}
	}
	
	public void generateSequenceReport(String logFolder, String log) throws Exception{
		Configuration cfg = ConfigurationHolder.getInstance().getConfiguration();
	}

	public void generateScriptReport(String logFolder, String log) throws Exception{
		Configuration cfg = ConfigurationHolder.getInstance().getConfiguration();
	}

	public TestSuites getTestSuitesData(String parent, String path) {

		TestSuites testSuitesDM = new TestSuites();
		String regex = "TEST_SUITES__(.*)";
		Pattern pattern = Pattern.compile(regex);
		Matcher timestampMatcher = pattern.matcher(path);
		String startedTime = "";
		if (timestampMatcher.find()) {
			startedTime = timestampMatcher.group(1);
		}
		testSuitesDM.setStartedTime(startedTime);

		List<TestSuite> testSuites = new ArrayList<TestSuite>();

		File logRoot = new File(parent + "/" + path);

		if (logRoot.list() != null) {
			for (String testSuitePath: logRoot.list()) {
				testSuites.add(getTestSuiteData(logRoot.getAbsolutePath(), testSuitePath));
			}
		}
		
		final SimpleDateFormat sdf = new SimpleDateFormat("yyyy_MM_dd__HH_mm_ss");
		
		Collections.sort(testSuites, new Comparator<TestSuite>(){

			public int compare(TestSuite o1, TestSuite o2) {
				// TODO Auto-generated method stub
				try{
					Date d1 = sdf.parse(o1.getStartedTime());
					Date d2 = sdf.parse(o2.getStartedTime());
					if(d2.after(d1)){
						return -1;
					}else{
						return 1;
					}
				}catch(Exception e){
					System.out.println("No, it won't happen");
				}
				return 0;
			}
			
		});

		testSuitesDM.setTestSuites(testSuites);
		int passNumber = 0;
		int warnNumber = 0;
		int failNumber = 0;
		for(TestSuite testSuite: testSuites){
			if(testSuite.getResult().equals(Constants.PASS)){
				passNumber++;
			}

			if(testSuite.getResult().equals(Constants.WARN)){
				warnNumber++;
			}

			if(testSuite.getResult().equals(Constants.FAIL)){
				failNumber++;
			}
		}
		testSuitesDM.setPassNumber(passNumber);
		testSuitesDM.setWarnNumber(warnNumber);
		testSuitesDM.setFailNumber(failNumber);
		
		return testSuitesDM;
	}

	public TestSuite getTestSuiteData(String parent, String path) {
		TestSuite testSuite = new TestSuite();
		String r1 = "TEST_SUITE__(.*?)__(.*)";
		Pattern p1 = Pattern.compile(r1);
		Matcher sm = p1.matcher(path);
		if (sm.find()) {
			testSuite.setName(sm.group(1));
			testSuite.setStartedTime(sm.group(2));
		}

		File testSuitePath = new File(parent + "/" + path);
		List<Iteration> iterations = new ArrayList<Iteration>();
		if (testSuitePath.list() != null) {
			int iterationNumber = testSuitePath.list().length;
			testSuite.setIterationNumber(iterationNumber);
			for (String iFolder : testSuitePath.list()) {
				File iterationPath = new File(parent + "/" + path + "/" + iFolder);
				Iteration iteration = new Iteration();
				if (iterationPath.list() != null) {
					iteration = new Iteration();
					List<TestCase> scripts = new ArrayList<TestCase>();
					for(String scriptPath: iterationPath.list()){
						scripts.add(getScriptData(iterationPath.getAbsolutePath(), scriptPath));
					}
					int passNumber = 0;
					int warnNumber = 0;
					int failNumber = 0;
					
					for(TestCase script: scripts){
						if(script.getResult().equals(Constants.PASS)){
							passNumber++;
						}

						if(script.getResult().equals(Constants.WARN)){
							warnNumber++;
						}

						if(script.getResult().equals(Constants.FAIL)){
							failNumber++;
						}
					}
					iteration.setPassNumber(passNumber);
					iteration.setWarnNumber(warnNumber);
					iteration.setFailNumber(failNumber);
					
					iteration.setScripts(scripts);
				}
				iterations.add(iteration);
			}
		}
		testSuite.setIterations(iterations);
		
		String result = Constants.PASS;
		
		outerloop:
		for(Iteration iteration: iterations){
			for(TestCase script: iteration.getScripts()){
				if(script.getResult().equals(Constants.FAIL)){
					result = Constants.FAIL;
					break outerloop;
				}
				
				if(script.getResult().equals(Constants.WARN)){
					result = Constants.WARN;
				}
			}
		}
		testSuite.setResult(result);
		
		for(Iteration iteration: iterations){
			String iterationResult = Constants.PASS;
			for(TestCase script: iteration.getScripts()){
				if(script.getResult().equals(Constants.FAIL)){
					iterationResult = Constants.FAIL;
					break;
				}
				
				if(script.getResult().equals(Constants.WARN)){
					iterationResult = Constants.WARN;
				}
			}
			iteration.setResult(iterationResult);
		}
		
		int passNumber = 0;
		int warnNumber = 0;
		int failNumber = 0;
		
		for(Iteration iteration: testSuite.getIterations()){
			if(iteration.getResult().equals(Constants.PASS)){
				passNumber++;
			}

			if(iteration.getResult().equals(Constants.WARN)){
				warnNumber++;
			}

			if(iteration.getResult().equals(Constants.FAIL)){
				failNumber++;
			}
		}
		
		testSuite.setPassNumber(passNumber);
		testSuite.setWarnNumber(warnNumber);
		testSuite.setFailNumber(failNumber);
		
		return testSuite;
	}

	public TestCase getScriptData(String parent, String path) {
		TestCase script = new TestCase();
		String filePath = parent + "/" + path;

		ObjectMapper objectMapper = new ObjectMapper();
		File file = new File(filePath);

		try{
			script = objectMapper.readValue(file, TestCase.class);
		}catch(Exception e){
			e.printStackTrace();
			System.out.println("The file " + filePath + " is not a valid JSON file.");
		}
		
		String regex = "(.*?)__(.*?)\\.json";
		Pattern pattern = Pattern.compile(regex);
		Matcher matcher = pattern.matcher(path);
		
		if(matcher.find()){
			String name = matcher.group(1);
			String startedTime = matcher.group(2);
			script.setName(name);
			script.setStartedTime(startedTime);
		}
		String result = Constants.PASS;
		if(script.getLog() != null){
			for(LogStatement logStatement: script.getLog()){
				if(logStatement.getLevel().toUpperCase().equals(Constants.FAIL)){
					result = Constants.FAIL;
					break;
				}
				
				if(logStatement.getLevel().toUpperCase().equals(Constants.WARN)){
					result = Constants.WARN;
				}
			}
		}
		script.setResult(result);
		return script;
	}
}
