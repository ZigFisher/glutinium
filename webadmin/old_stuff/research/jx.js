//V3.00.A - http://www.openjs.com/scripts/jx/
jx = {
	http:false, //HTTP Object
	format:'text',
	callback:function(data){},
	error:false,
	//Create a xmlHttpRequest object - this is the constructor. 
	getHTTPObject : function() {
		var http = false;
		//Use IE's ActiveX items to load the file.
		if(typeof ActiveXObject != 'undefined') {
			try {http = new ActiveXObject("Msxml2.XMLHTTP");}
			catch (e) {
				try {http = new ActiveXObject("Microsoft.XMLHTTP");}
				catch (E) {http = false;}
			}
		//If ActiveX is not available, use the XMLHttpRequest of Firefox/Mozilla etc. to load the document.
		} else if (XMLHttpRequest) {
			try {http = new XMLHttpRequest();}
			catch (e) {http = false;}
		}
		return http;
	},
	// This function is called from the user's script. 
	//Arguments - 
	//	url	- The url of the serverside script that is to be called. Append all the arguments to 
	//			this url - eg. 'get_data.php?id=5&car=benz'
	//	callback - Function that must be called once the data is ready.
	//	format - The return type for this function. Could be 'xml','json' or 'text'. If it is json, 
	//			the string will be 'eval'ed before returning it. Default:'text'
	load : function (url,callback,format) {
		this.init(); //The XMLHttpRequest object is recreated at every call - to defeat Cache problem in IE
		if(!this.http||!url) return;
		if (this.http.overrideMimeType) this.http.overrideMimeType('text/xml');

		this.callback=callback;
		if(!format) var format = "text";//Default return type is 'text'
		this.format = format.toLowerCase();
		var ths = this;//Closure
		
		//Kill the Cache problem in IE.
		var now = "uid=" + new Date().getTime();
		url += (url.indexOf("?")+1) ? "&" : "?";
		url += now;

		this.http.open("GET", url, true);

		this.http.onreadystatechange = function () {//Call a function when the state changes.
			if(!ths) return;
			var http = ths.http;
			if (http.readyState == 4) {//Ready State will be 4 when the document is loaded.
				if(http.status == 200) {
					var result = "";
					if(http.responseText) result = http.responseText;
					//If the return is in JSON format, eval the result before returning it.
					if(ths.format.charAt(0) == "j") {
						//\n's in JSON string, when evaluated will create errors in IE
						result = result.replace(/[\n\r]/g,"");
						result = eval('('+result+')'); 
					}
	
					//Give the data to the callback function.
					if(ths.callback) ths.callback(result);
				} else { //An error occured
					if(ths.error) ths.error(http.status);
				}
			}
		}
		this.http.send(null);
	},
	init : function() {this.http = this.getHTTPObject();}
}