//点击链接后锚点内容居中并反色

var hlanchors=document.links;

for (var i=0;i<hlanchors.length;i++) {
	hlanchors[i].onclick=function(){k=0;showme(this)}
}
function showme(o) {
	var o=o
	var anchorhref=o.href.split("v")[1];
	window.scrollTo(0,$('#'+anchorhref).offset().top-250);
	document.getElementById(anchorhref).style.background="#D2FFA5";
	Reset(document.getElementById(anchorhref));
}

function Reset(obj) {
        setTimeout(function(){obj.style.backgroundColor='#fff'},2000);
}