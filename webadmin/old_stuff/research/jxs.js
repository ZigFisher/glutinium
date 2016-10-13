function $(id){return document.getElementById(id);}
function $F(id){return $(id).value;}

function cmdfocus(str) { $("cmd").focus();}
function cmdset(str) { $("cmd").value = str; cmdfocus();}

function makeCall() {
	var url='/cgi-bin/ajaxcmd.cgi?cmd=' + $F("ajaxcmd");
	$("loading").style.display='block';
	jx.load(url,handler,'text','POST');
	return false;
}
function handler(data) {
	$("loading").style.display='none';
	$("code").innerHTML=data;
	dp.SyntaxHighlighter.HighlightAll('code');
}
//
//
//V2.01.A - http://www.openjs.com/scripts/jx/
jx = {
	http:false, //HTTP Object
	format:'text',
	callback:function(data){},
	handler:false,
	error: false,
	opt:new Object(),
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
	//	method - GET or POST. Default 'GET'
	load : function (url,callback,format,method) {
		this.init(); //The XMLHttpRequest object is recreated at every call - to defeat Cache problem in IE
		if(!this.http||!url) return;
		//XML Format need this for some Mozilla Browsers
		if (this.http.overrideMimeType) this.http.overrideMimeType('text/xml');

		this.callback=callback;
		if(!method) var method = "GET";//Default method is GET
		if(!format) var format = "text";//Default return type is 'text'
		this.format = format.toLowerCase();
		method = method.toUpperCase();
		var ths = this;//Closure
		
		//Kill the Cache problem in IE.
		var now = "uid=" + new Date().getTime();
		url += (url.indexOf("?")+1) ? "&" : "?";
		url += now;

		var parameters = null;

		if(method=="POST") {
			var parts = url.split("\?");
			url = parts[0];
			parameters = parts[1];
		}
		this.http.open(method, url, true);

		if(method=="POST") {
			this.http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			this.http.setRequestHeader("Content-length", parameters.length);
			this.http.setRequestHeader("Connection", "close");
		}

		if(this.handler) { //If a custom handler is defined, use it
			this.http.onreadystatechange = this.handler;
		} else {
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

						} else if(ths.format.charAt(0) == "x") { //XML Return
							result = http.responseXML;
						}

						//Give the data to the callback function.
						if(ths.callback) ths.callback(result);
					} else {
						if(ths.opt.loadingIndicator) document.getElementsByTagName("body")[0].removeChild(ths.opt.loadingIndicator); //Remove the loading indicator
						if(ths.opt.loading) document.getElementById(ths.opt.loading).style.display="none"; //Hide the given loading indicator.
						
						if(ths.error) ths.error(http.status);
					}
				}
			}
		}
		this.http.send(parameters);
	},
	bind : function(user_options) {
		var opt = {
			'url':'', 			//URL to be loaded
			'onSuccess':false,	//Function that should be called at success
			'onError':false,	//Function that should be called at error
			'format':"text",	//Return type - could be 'xml','json' or 'text'
			'method':"GET",		//GET or POST
			'update':"",		//The id of the element where the resulting data should be shown. 
			'loading':"",		//The id of the loading indicator. This will be set to display:block when the url is loading and to display:none when the data has finished loading.
			'loadingIndicator':"" //HTML that would be inserted into the document once the url starts loading and removed when the data has finished loading. This will be inserted into a div with class name 'loading-indicator' and will be placed at 'top:0px;left:0px;'
		}
		for(var key in opt) {
			if(user_options[key]) {//If the user given options contain any valid option, ...
				opt[key] = user_options[key];// ..that option will be put in the opt array.
			}
		}
		this.opt = opt;
		if(!opt.url) return; //Return if a url is not provided
		if(opt.onError) this.error = opt.onError;

		var div = false;
		if(opt.loadingIndicator) { //Show a loading indicator from the given HTML
			div = document.createElement("div");
			div.setAttribute("style","position:absolute;top:0px;left:0px;");
			div.setAttribute("class","loading-indicator");
			div.innerHTML = opt.loadingIndicator;
			document.getElementsByTagName("body")[0].appendChild(div);
			this.opt.loadingIndicator=div;
		}
		if(opt.loading) document.getElementById(opt.loading).style.display="block"; //Show the given loading indicator.
		
		this.load(opt.url,function(data){
			if(opt.onSuccess) opt.onSuccess(data);
			if(opt.update) document.getElementById(opt.update).innerHTML = data;
			
			if(div) document.getElementsByTagName("body")[0].removeChild(div); //Remove the loading indicator
			if(opt.loading) document.getElementById(opt.loading).style.display="none"; //Hide the given loading indicator.
		},opt.format,opt.method);
	},
	init : function() {this.http = this.getHTTPObject();}
}
