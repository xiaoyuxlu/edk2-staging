package com.gearon.app.data_model;

import java.util.List;

public class Iteration {
	
	private List<TestCase> scripts;
	private String result;
	private int iterationOrder;
	private String parentName;
	
	private int passNumber;
	private int warnNumber;
	private int failNumber;
	
	public List<TestCase> getScripts() {
		return scripts;
	}
	public void setScripts(List<TestCase> scripts) {
		this.scripts = scripts;
	}
	
	public String getResult() {
		return result;
	}
	public void setResult(String result) {
		this.result = result;
	}

	public int getIterationOrder() {
		return iterationOrder;
	}
	public void setIterationOrder(int iterationOrder) {
		this.iterationOrder = iterationOrder;
	}

	public String getParentName() {
		return parentName;
	}
	public void setParentName(String parentName) {
		this.parentName = parentName;
	}
	public int getPassNumber() {
		return passNumber;
	}
	public void setPassNumber(int passNumber) {
		this.passNumber = passNumber;
	}
	public int getWarnNumber() {
		return warnNumber;
	}
	public void setWarnNumber(int warnNumber) {
		this.warnNumber = warnNumber;
	}
	public int getFailNumber() {
		return failNumber;
	}
	public void setFailNumber(int failNumber) {
		this.failNumber = failNumber;
	}
	
	
}
