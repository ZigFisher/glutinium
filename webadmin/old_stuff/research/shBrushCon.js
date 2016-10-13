dp.sh.Brushes.Console = function()
{
	var goodwords	=	'RX TX UP BROADCAST RUNNING MULTICAST MTU ACCEPT INPUT OUTPUT FORWARD PREROUTING POSTROUTING';
	var badwords	=	'fail errors? DROP DROPLOG';
	var ipaddr		=	'';

	var keywords =	'Link eth[0-9] gre[0-9] tun[0-9] teql[0-9] tunl[0-9]';

	this.regexList = [
		{ regex: new RegExp('#(.*)$', 'gm'),						css: 'comment' },
		{ regex: new RegExp('(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(/[0-9]+)?', 'gm'),						css: 'ipaddr' },
		{ regex: dp.sh.RegexLib.DoubleQuotedString,					css: 'string' },
		{ regex: dp.sh.RegexLib.SingleQuotedString,					css: 'string' },
		{ regex: new RegExp(this.GetKeywords(goodwords), 'gmi'),	css: 'goodwords' },
		{ regex: new RegExp(this.GetKeywords(badwords), 'gmi'),		css: 'badwords' },
		{ regex: new RegExp(this.GetKeywords(keywords), 'gmi'),		css: 'keyword' }
		];

	this.CssClass = 'dp-con';
	this.Style =	'.dp-con .goodwords { color: #77BB77; }' +
					'.dp-con .ipaddr { color: darkgreen; font-weight: bold; }' +
					'.dp-con .badwords { color: #AA2020; }' +
					'.dp-con .op { color: #808080; }';
}

dp.sh.Brushes.Console.prototype	= new dp.sh.Highlighter();
dp.sh.Brushes.Console.Aliases	= ['con'];
