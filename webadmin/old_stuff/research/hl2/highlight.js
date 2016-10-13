/* Javascript Syntax Highlighter
 * Highlights many different programming languages
 * Outputs valid XHTML, so can be used in XHTML pages!
 * Written by: Michiel van Eerd
 */
 
 /* IE doesn't understand indexOf() on arrays, so add it */
if (!Array.prototype.indexOf) {
        Array.prototype.indexOf = function(val) {
                var len = this.length;
                for (var i = 0; i < len; i++) {
                        if (this[i] == val) return i;
                }
                return -1;
        }
}

var SyntaxHighlighter = {};

SyntaxHighlighter.language = {};

SyntaxHighlighter.xmlEntities = {
		"&" : "&amp;",
		"<" : "&lt;",
		">" : "&gt;",
		"\"" : "&quot;",
		"'" : "&#39;"
	};

SyntaxHighlighter.strToXHTML = function(str)
	{
		var addLen = 0;
		
		for (var key in SyntaxHighlighter.xmlEntities)
		{
			str = str.replace(new RegExp(key, "g"),
				function(match, offset, s)
				{
					addLen += (SyntaxHighlighter.xmlEntities[key].length - 1);
					return SyntaxHighlighter.xmlEntities[key];
				}
			);
		}
		return {
			"str" : str,
			"addLen" : addLen
		}
	};

SyntaxHighlighter.copyObject = function(ob) {
		var newOb = {};
		for (var prop in ob) {
			newOb[prop] = ob[prop];
		}
		return newOb;
	};

SyntaxHighlighter.highlightDocument = function(showLineNumbers)
	{
		var codeList = document.getElementsByTagName("code");
		for (var i = 0, len = codeList.length; i < len; i++)
		{
			if (codeList[i].className && SyntaxHighlighter.language[codeList[i].className])
			{
				SyntaxHighlighter.highlight(codeList[i], showLineNumbers);
			}
		}
	};

SyntaxHighlighter.highlight = function(codeEl, showLineNumbers)
	{
		var lang = SyntaxHighlighter.language[codeEl.className];
		if (!lang)
		{
			return;
		}

		var span_comment_len = "<span class='comment'></span>".length;
		var span_quote_len = "<span class='quote'></span>".length;
		var span_operator_len = "<span class='operator'></span>".length;
		var span_keyword_len = "<span class='keyword'></span>".length;
		
		var str = "";
		for (var i = 0; i < codeEl.childNodes.length; i++)
		{
			str += codeEl.childNodes[i].data; // data rewrites HTML entities to real characters
		}
		var lines = (str.indexOf("\n") != -1) ? str.split("\n") : str.split("\r"); // FF or IE
		
		var beginMultiCommentIndex = -1;
		
		forLineLoop:
		for (var lineIndex = 0, lineCount = lines.length; lineIndex < lineCount; lineIndex++)
		{
			var line = lines[lineIndex];
			var prop = {};
			
			forCharLoop:
			for (var charIndex = 0, lineLen = line.length; charIndex < lineLen; charIndex++)
			{
				var c = line.charAt(charIndex);
				
				// End multiline comment
				if (beginMultiCommentIndex != -1)
				{
					var endMultiCommentIndex = -1;
					for (; charIndex < lineLen; charIndex++)
					{
						c = line.charAt(charIndex);
						if (c == "\\")
						{
							charIndex++;
							continue;
						}
						if (c == lang.comment.multi.end.charAt(0))
						{
							endMultiCommentIndex = charIndex;
							for (var i = 0; i < lang.comment.multi.end.length; i++)
							{
								if (line.charAt(charIndex + i) != lang.comment.multi.end.charAt(i))
								{
									endMultiCommentIndex = -1;
									break;
								}
							}
							if (endMultiCommentIndex != -1)
							{
								charIndex += (lang.comment.multi.end.length - 1);
								endMultiCommentIndex = charIndex;
								break;
							}
						}
					}
					var realEndIndex = (endMultiCommentIndex != -1) ? endMultiCommentIndex : lineLen - 1;
					//var addLen = "<span class='comment'></span>".length;
					var substrOb = SyntaxHighlighter.strToXHTML(line.substr(beginMultiCommentIndex, realEndIndex - beginMultiCommentIndex + 1));
					line = line.substr(0, beginMultiCommentIndex) + "<span class='comment'>" + substrOb.str + "</span>" + line.substr(realEndIndex + 1);
					charIndex += (span_comment_len + substrOb.addLen);
					lineLen += (span_comment_len + substrOb.addLen);
					prop[beginMultiCommentIndex] = span_comment_len + substrOb.str.length;
					beginMultiCommentIndex = (endMultiCommentIndex != -1) ? -1 : 0;
					continue forCharLoop;
				}
				
				// Begin multiline comment
				if (lang.comment.multi && c == lang.comment.multi.start.charAt(0))
				{
					beginMultiCommentIndex = charIndex;
					for (var i = 0; i < lang.comment.multi.start.length; i++)
					{
						if (line.charAt(charIndex + i) != lang.comment.multi.start.charAt(i))
						{
							beginMultiCommentIndex = -1;
							break;
						}
					}
					if (beginMultiCommentIndex != -1)
					{
						charIndex += lang.comment.multi.start.length - 1;
						if (charIndex == lineLen - 1)
						{
							charIndex--;
						}
						continue forCharLoop;
					}
				}
				
				// Single comment
				if (lang.comment.single && c == lang.comment.single.start.charAt(0))
				{
					var beginSingleCommentIndex = charIndex;
					// Eventueel het begin van een single comment
					for (var i = 0; i < lang.comment.single.start.length; i++)
					{
						if (line.charAt(charIndex + i) != lang.comment.single.start.charAt(i))
						{
							beginSingleCommentIndex = -1
							break;
						}
					}
					if (beginSingleCommentIndex != -1)
					{
						// Alles totaan einde van de regel is comment
						var substrOb = SyntaxHighlighter.strToXHTML(line.substr(beginSingleCommentIndex));
						//var addLen = "<span class='comment'></span>".length;
						line = line.substr(0, beginSingleCommentIndex) + "<span class='comment'>" + substrOb.str + "</span>";
						charIndex = line.length - 1;
						prop[beginSingleCommentIndex] = span_comment_len + substrOb.str.length;
						continue forCharLoop;
					}
				}
				
				// Quotes
				if (c == "\"" || c == "'")
				{
					var quote = c;
					var beginQuoteIndex = charIndex;
					// Hier doorgaan naar einde quote
					for (charIndex += 1; charIndex < lineLen; charIndex++)
					{
						c = line.charAt(charIndex);
						if (c == "\\")
						{
							charIndex++;
							continue;
						}
						if (c == quote)
						{
							// Einde
							var substrOb = SyntaxHighlighter.strToXHTML(line.substr(beginQuoteIndex, charIndex - beginQuoteIndex + 1));
							//var addLen = "<span class='quote'></span>".length;
							line = line.substr(0, beginQuoteIndex) + "<span class='quote'>" + substrOb.str + "</span>" + line.substr(charIndex + 1);
							prop[beginQuoteIndex] = span_quote_len + substrOb.str.length;
							charIndex += (span_quote_len + substrOb.addLen);
							lineLen += (span_quote_len + substrOb.addLen);
							continue forCharLoop;
						}
					}
				}
				
				// Operators
				if (lang.operator.indexOf(c) != -1)
				{
					c = (SyntaxHighlighter.xmlEntities[c]) ? SyntaxHighlighter.xmlEntities[c] : c;
					//var addLen = "<span class='operator'></span>".length + (c.length - 1);
					var addLen = span_operator_len + (c.length - 1);
					line = line.substr(0, charIndex) + "<span class='operator'>" + c + "</span>" + line.substr(charIndex + 1);
					prop[charIndex] = addLen + c.length;
					charIndex += addLen;
					lineLen += addLen;
					continue forCharLoop;
				}
			}
			
			// Keywords - not for each char, but each line
			for (var i = 0; i < lang.keyword.length; i++)
			{
				var keyword = lang.keyword[i];
				var keywordIndex = line.indexOf(keyword);
				while (keywordIndex != -1)
				{
					if (/\w/.test(line.charAt(keywordIndex - 1)) || /\w/.test(line.charAt(keywordIndex + keyword.length))) {
						keywordIndex = line.indexOf(keyword, keywordIndex + 1);
						continue;
					}
					
					var isKeyword = true;
					for (var key in prop) {
						if (keywordIndex >= parseInt(key) && keywordIndex < (parseInt(key) + parseInt(prop[key]))) {
							isKeyword = false;
							break;
						}
					}
					if (isKeyword) {
						//var addString = "<span class='keyword'></span>";
						//var addLen = addString.length; // dit is erbij gekomen
						//var addLen = span_keyword_len;
						line = line.substr(0, keywordIndex) + "<span class='keyword'>" + keyword + "</span>" + line.substr(keywordIndex + keyword.length);
						prop[keywordIndex] = keyword.length + span_keyword_len;
						var tmpOb = new Object();
						for (var key in prop) {
							if (parseInt(key) > keywordIndex) {
								var newIndex = parseInt(key) + span_keyword_len;
								tmpOb[newIndex] = prop[key];
							} else {
								tmpOb[key] = prop[key];
							}
						}
						prop = SyntaxHighlighter.copyObject(tmpOb);
						keywordIndex = line.indexOf(keyword, keywordIndex + span_keyword_len + keyword.length);
					} else {
						keywordIndex = line.indexOf(keyword, keywordIndex + 1);
					}
					
				}
			}
			
			//line.prop = prop;
			lines[lineIndex] = line;
		}
		
		// Print the lines
		var joinString = "";
		var showLineNumbersThisTime = showLineNumbers;
		var newLines = null;
		
		if (codeEl.parentNode.nodeName.toLowerCase() != "pre")
		{
			showLineNumbersThisTime = false;
		}
		
		if (showLineNumbersThisTime)
		{
			newLines = ["<ol>"];
			for (var i = 0; i < lineCount; i++) {
				newLines.push("<li><span>" + lines[i] + "</span> </li>");
			}
			newLines.push("</ol>");
		}
		else
		{
			newLines = lines;
			if (codeEl.parentNode.nodeName.toLowerCase() == "pre")
			{
				joinString = "\n";
			}
		}
		
		if (codeEl.outerHTML && codeEl.parentNode.nodeName.toLowerCase() == "pre")
		{
			codeEl.outerHTML = "<pre><code class='" + codeEl.className + "'>" + newLines.join("\r") + "</code></pre>"
		}
		else
		{
			//codeEl.setAttribute("xmlns", "http://www.w3.org/1999/xhtml");
			codeEl.innerHTML = newLines.join(joinString);
			//codeEl.innerHTML = "<code xmlns='http://www.w3.org/1999/xhtml'>" + newLines.join(joinString) + "</code>";
			// Als je dit in XUL zou gebruiken, dan moet je namespace meenemen
			//jsdom.innerXHTML(codeEl, "<code xmlns='http://www.w3.org/1999/xhtml'>" + newLines.join(joinString) + "</code>");
		}
	};




