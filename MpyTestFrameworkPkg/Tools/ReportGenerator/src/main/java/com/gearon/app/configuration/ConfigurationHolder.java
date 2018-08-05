package com.gearon.app.configuration;

import freemarker.template.*;
import freemarker.template.Configuration;

public class ConfigurationHolder {
	
	private static ConfigurationHolder holder;
	private Configuration cfg;

	static{
		holder = new ConfigurationHolder();
	}
	
	public static ConfigurationHolder getInstance(){
		return holder;
	}

	public ConfigurationHolder(){
		try{
	        /* ------------------------------------------------------------------------ */
	        /* You should do this ONLY ONCE in the whole application life-cycle:        */

	        /* Create and adjust the configuration singleton */
	        cfg = new Configuration();
	        cfg.setClassForTemplateLoading(this.getClass(), "/templates/");
	        cfg.setDefaultEncoding("UTF-8");
	        cfg.setTemplateExceptionHandler(TemplateExceptionHandler.RETHROW_HANDLER);
	        
	        
			
		}catch(Exception e){
			System.out.println("Something bad happends when initialize the Configuration object");
		}
	}
	
	public Configuration getConfiguration(){
		return cfg;
	}
}
