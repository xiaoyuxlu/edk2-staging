package com.gearon.app.data_model;

import java.util.List;

public class TestCase {
	
	private String name;
	private String startedTime;
	private String result;
	private List<LogStatement> log;

	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getStartedTime() {
		return startedTime;
	}
	public void setStartedTime(String startedTime) {
		this.startedTime = startedTime;
	}
	public String getResult() {
		return result;
	}
	public void setResult(String result) {
		this.result = result;
	}
	public List<LogStatement> getLog() {
		return log;
	}
	public void setLog(List<LogStatement> log) {
		this.log = log;
	}

}
