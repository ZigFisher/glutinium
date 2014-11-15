isIE=(navigator.appName.indexOf("Microsoft")!=-1)?1:0;
function $(id){return document.getElementById(id);}
function $F(id){return $(id).value;}
function cmdfocus() { if($("ajaxcmd"))$("ajaxcmd").focus();}
function cmdset(str) { $("ajaxcmd").value = str; cmdfocus();}
function urlencode(str) {
	str = str.replace(/%/g, '%25').replace(/\+/g, '%2B').replace(/%20/g, '+').replace(/\*/g, '%2A').replace(/\//g, '%2F').replace(/@/g, '%40');
	return str.replace(/&/g, '%26').replace(/;/g, '%3B').replace(/\$/g, '%24').replace(/\?/g, '%3F');
}

function runcmd(cmd) {
	window.curcmd=cmd;
	var url='/cgi-bin/ajaxcmd.cgi?cmd=' + urlencode(cmd);
	$("loading").style.display='block';
	jx.load(url,handler,'text','POST');
	cmdfocus();
	return false;
}
function handler(data) {
	$("loading").style.display='none';
	if (isIE)
		$("code").innerText=data; 
	else
		$("code").innerHTML=data; 
}
function autocmd() {
	if ($('autocmd').checked) {
		runcmd(window.curcmd);
		setTimeout('autocmd()', 3000);
	}
}
jx={http:false,format:"text",callback:function(A){},handler:false,error:false,opt:new Object(),getHTTPObject:function(){var A=false;if(typeof ActiveXObject!="undefined"){try{A=new ActiveXObject("Msxml2.XMLHTTP")}catch(C){try{A=new ActiveXObject("Microsoft.XMLHTTP")}catch(B){A=false}}}else{if(XMLHttpRequest){try{A=new XMLHttpRequest()}catch(C){A=false}}}return A},load:function(url,callback,format,method){this.init();if(!this.http||!url){return }if(this.http.overrideMimeType){this.http.overrideMimeType("text/xml")}this.callback=callback;if(!method){var method="GET"}if(!format){var format="text"}this.format=format.toLowerCase();method=method.toUpperCase();var ths=this;var now="uid="+new Date().getTime();url+=(url.indexOf("?")+1)?"&":"?";url+=now;var parameters=null;if(method=="POST"){var parts=url.split("?");url=parts[0];parameters=parts[1]}this.http.open(method,url,true);if(method=="POST"){this.http.setRequestHeader("Content-type","application/x-www-form-urlencoded");this.http.setRequestHeader("Content-length",parameters.length);this.http.setRequestHeader("Connection","close")}if(this.handler){this.http.onreadystatechange=this.handler}else{this.http.onreadystatechange=function(){if(!ths){return }var http=ths.http;if(http.readyState==4){if(http.status==200){var result="";if(http.responseText){result=http.responseText}if(ths.format.charAt(0)=="j"){result=result.replace(/[\n\r]/g,"");result=eval("("+result+")")}else{if(ths.format.charAt(0)=="x"){result=http.responseXML}}if(ths.callback){ths.callback(result)}}else{if(ths.opt.loadingIndicator){document.getElementsByTagName("body")[0].removeChild(ths.opt.loadingIndicator)}if(ths.opt.loading){document.getElementById(ths.opt.loading).style.display="none"}if(ths.error){ths.error(http.status)}}}}}this.http.send(parameters)},bind:function(A){var C={"url":"","onSuccess":false,"onError":false,"format":"text","method":"GET","update":"","loading":"","loadingIndicator":""};for(var B in C){if(A[B]){C[B]=A[B]}}this.opt=C;if(!C.url){return }if(C.onError){this.error=C.onError}var D=false;if(C.loadingIndicator){D=document.createElement("div");D.setAttribute("style","position:absolute;top:0px;left:0px;");D.setAttribute("class","loading-indicator");D.innerHTML=C.loadingIndicator;document.getElementsByTagName("body")[0].appendChild(D);this.opt.loadingIndicator=D}if(C.loading){document.getElementById(C.loading).style.display="block"}this.load(C.url,function(E){if(C.onSuccess){C.onSuccess(E)}if(C.update){document.getElementById(C.update).innerHTML=E}if(D){document.getElementsByTagName("body")[0].removeChild(D)}if(C.loading){document.getElementById(C.loading).style.display="none"}},C.format,C.method)},init:function(){this.http=this.getHTTPObject()}}
