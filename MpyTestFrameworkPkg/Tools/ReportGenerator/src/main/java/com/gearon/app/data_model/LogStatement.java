package com.gearon.app.data_model;

public class LogStatement {
	private String time_stamp;
	private String level;
	private String message;
	
	public String getTime_stamp() {
		return time_stamp;
	}
	public void setTime_stamp(String time_stamp) {
		this.time_stamp = time_stamp;
	}
	public String getLevel() {
		return level;
	}
	public void setLevel(String level) {
		this.level = level;
	}
	
	public String getMessage() {
		return message;
	}
	public void setMessage(String message) {
		this.message = message;
	}
}
