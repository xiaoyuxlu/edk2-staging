package com.gearon.app.data_model;

import java.util.List;

public class TestSuite {
	private String name;
	private List<Iteration> iterations;
	private int iterationNumber;
	private String startedTime;
	private String result;

	private int passNumber;
	private int warnNumber;
	private int failNumber;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public List<Iteration> getIterations() {
		return iterations;
	}
	public void setIterations(List<Iteration> iterations) {
		this.iterations = iterations;
	}
	public int getIterationNumber() {
		return iterationNumber;
	}
	public void setIterationNumber(int iterationNumber) {
		this.iterationNumber = iterationNumber;
	}
	public String getResult() {
		return result;
	}
	public void setResult(String result) {
		this.result = result;
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
	public String getStartedTime() {
		return startedTime;
	}
	public void setStartedTime(String startedTime) {
		this.startedTime = startedTime;
	}
}
