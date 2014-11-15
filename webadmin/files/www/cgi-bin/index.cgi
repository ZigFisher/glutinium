#!/usr/bin/haserl
<?
	echo -e "Content-type: text/html\r\n\r\n"
		basedir=/www
	page=${FORM_page:-info}
	export PATH=/usr/local/bin:/usr/local/sbin:/bin:/sbin:/usr/bin:/usr/sbin
	export hostname=`hostname`
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<title>FlyRouter</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<link rel="shortcut icon" href="/favicon.ico" />
<link type="text/css" rel="stylesheet" href="/css/simpletree.css" />
<link type="text/css" rel="stylesheet" href="/css/style.css" />
<script language="javascript" src="/js/simpletreemenu.js"></script>
<script language="javascript" src="/js/jxs.js"></script>
</head>

<body onload='if($("treemenu1"))ddtreemenu.createTree("treemenu1", true)'>
<div id="main">
	<div id="logo">
		<h1><a href="/"><? echo $hostname; ?></a></h1>
	</div>
	<div id="menu">
		<ul>
			<li><a href="?page=info" <? [ "$page" = info ] && echo 'class="selected"' ?>>info</a></li>
			<li><a href="?page=show" <? [ "$page" = show ] && echo 'class="selected"' ?>>show</a></li>
			<li><a href="?page=edit" <? [ "$page" = edit ] && echo 'class="selected"' ?>>edit</a></li>
			<li><a href="?page=do" <? [ "$page" = do ] && echo 'class="selected"' ?>>do</a></li>
			<li><a href="?page=config" <? [ "$page" = config ] && echo 'class="selected"' ?>>config</a></li>
			<li><a href="?page=misc" <? [ "$page" = misc ] && echo 'class="selected"' ?>>misc</a></li>
		</ul>
	</div>
	<div id="site_content">
		<div id="first_column">
			<div class="disignBoxFirst">
				<div class="boxFirstHeader">	</div>
				<div class="boxContent">
<?
		[ -f "$basedir/menu/${page}.html" ] && cat "$basedir/menu/${page}.html"
		[ -x "$basedir/menu/${page}.sh" ] && . "$basedir/menu/${page}.sh"
?>
				</div>
			</div>
		</div>
		<div id="content">
			<div class="disignBoxSecond">
				<div class="boxContent">
					<? [ -n "$header" ] && echo "<h1>$header</h1>" ?>
<?	
		[ -x "${basedir}/page/$page.sh" ] && . ${basedir}/page/${page}.sh 
?>
				</div>
			</div>
		</div>
	</div>
	<div id="footer">
		<div id="copy_r"><a href='http://flyrouter.net' target="_blank">http://flyrouter.net</a></div>
	</div>
</div>
</body>
</html>
