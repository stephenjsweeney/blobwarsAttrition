$(document).ready(function() {
	var toc = $('#toc');
	var i = 1;

	$('h3,h4').each(function(i, e) {

		var link = 'link' + i;
		var a = $('<a>').attr('href', '#' + link).append(e.innerText);

		$(e).before($('<a>').attr('name', link));

		if (e.tagName == 'H3')
		{
			toc.append("&blacksquare; <a href='#" + link + "'>" + e.innerText + "</a><br />");
		}
		
		if (e.tagName == 'H4')
		{
			toc.append("<span style='text-indent: 2em; display: inline-block'>&#9643 <a href='#" + link + "'>" + e.innerText + "</a></span><br />");
		}
	});
});
