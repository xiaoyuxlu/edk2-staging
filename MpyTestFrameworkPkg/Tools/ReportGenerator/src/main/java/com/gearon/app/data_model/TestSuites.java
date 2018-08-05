package com.gearon.app.data_model;

import java.util.*;

public class TestSuites {
	private String startedTime;
	private List<TestSuite> testSuites;

	private int passNumber;
	private int warnNumber;
	private int failNumber;
	
	public String getStartedTime() {
		return startedTime;
	}
	public void setStartedTime(String startedTime) {
		this.startedTime = startedTime;
	}
	public List<TestSuite> getTestSuites() {
		return testSuites;
	}
	public void setTestSuites(List<TestSuite> testSuites) {
		this.testSuites = testSuites;
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
